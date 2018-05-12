/*
 * Description: Produce undistorted image from raw image captured from the camera.
 * Input:
 *			    |fx 0  cx|
 *			K = |0  fy cy|
 *				|0  0   1|
 *
 *		dist_coef = |k1, k2, p1, p2|
 * Option#0 - Using cv::getOptimalNewCameraMatrix and cv::initUndistortRectifyMap, which automatically
 *			produce the undistorted image for us.
 * Option#1 - Perform the reverse distortion in which an undistorted image is created by interpolating
 *			the pixel values from the distorted (input/raw) image using cv::remap.
 *			
 */

#include <string>
#include <iostream>
#include <cstdlib>

#include <opencv2/opencv.hpp>

// Compute the image size of the undistorted image
void computeUndistortedBoundary(/*in*/cv::Size &image_size, 
								/*in*/cv::Mat &K,
								/*in*/cv::Mat &distortionCoef,
								/*out*/cv::Mat &undistortedCorners,
								/*out*/double &dminx,
								/*out*/double &dminy,
								/*out*/double &dmaxx,
								/*out*/double &dmaxy);


// Produce distorted point (out_x, out_y) from undistort (in_x, in_y) point
void distortPoint(	cv::Mat &K, cv::Mat &distortionCoef, 
					double in_x, double in_y, 
					/*out*/double &out_x, /*out*/double &out_y);

int main(int argc, char *argv[])
{
	// Image to perform undistort
	std::string image_path = "G:/raw_image.jpg";
	// Intrinsic parameters
	double fx = 1738.06409;
	double fy = 1736.96128;
	double cx = 965.22200;
	double cy = 666.61850;
	// Distortion coefficients
	double k1 = -0.34592;
	double k2 =  0.16969;
	double p1 = -0.00279;
	double p2 = 0.00235;

	// 1. Construct the K matrix and distortion matrix
	cv::Mat K = (cv::Mat_<double>(3,3) <<	fx, 0.0, cx,
											0.0, fy, cy,
											0.0, 0.0, 1.0);
	cv::Mat distortionCoef = (cv::Mat_<double>(1,4) << k1, k2, p1, p2);

	// 2. Read-in the input image
	cv::Mat distorted_image = cv::imread(image_path, CV_LOAD_IMAGE_COLOR);
	if (distorted_image.data == NULL)
	{
		std::cout << "Failed to read the image: " << image_path << ", terminating..." << std::endl;
		return -1;
	}
	cv::Size image_size = distorted_image.size();

	// 3. Compute the new image size of the undistorted image
	//		Compute the (min_x, min_y) and (max_x, max_y) of the undistorted image from the 4 boundary points.
	cv::Mat undistortedCorners;
	double dminx, dminy, dmaxx, dmaxy;
	computeUndistortedBoundary(image_size, K, distortionCoef,
				/*out*/undistortedCorners, dminx, dminy, dmaxx, dmaxy);
	int new_image_width = int(dmaxx+0.5) - int(dminx-0.5);
	int new_image_height = int(dmaxy+0.5) - int(dminy-0.5);
	cv::Size new_image_size = cv::Size(new_image_width, new_image_height); // for undistorted_image

	// ------- OPTION#0 --------
	{
		// 4. Use the OpenCV's functions to compute undistorted image
		// 4.1 Get the new K matrix with 0 distortion
		cv::Mat new_K = cv::getOptimalNewCameraMatrix(K, distortionCoef, image_size, /*alpha*/1, new_image_size);
		// 4.2 Compute the undistortion and rectification transformation map
		cv::Mat map_x, map_y;
		cv::initUndistortRectifyMap(K, distortionCoef, cv::Mat(), new_K, new_image_size, CV_32FC1, map_x, map_y);
		cv::Mat undistorted_image_out1; // output undistorted image
		cv::remap(/*in*/distorted_image, /*out*/undistorted_image_out1, map_x, map_y, CV_INTER_LINEAR, cv::BORDER_TRANSPARENT);
		cv::imwrite("undistorted_out1.jpg", undistorted_image_out1);
	}

	// ------- OPTION#1 --------
	{
		// 5. Perform the reverse distortion and create the undistorted image using cv::remap
		// 5.1 Initialise the map matrices
		cv::Mat map_x(new_image_height, new_image_width, CV_32F);
		cv::Mat map_y(new_image_height, new_image_width, CV_32F);
		// 5.2 Construct the corresponding maps between undistort and distort point
		for (int y = 0; y < new_image_size.height; y++)
		{
			for (int x = 0; x < new_image_width; x++)
			{
				// Coordinates of undistorted points
				double v = dminy + y;
				double u = dminx + x;
			
				// Compute the distorted point
				double distortedx, distortedy;
				distortPoint(K, distortionCoef, u, v, distortedx, distortedy);
			
				// Construct map_x, map_y
				map_x.at<float>(y,x) = (float)distortedx;
				map_y.at<float>(y,x) = (float)distortedy;
			}
		}
		// 5.3 Construct the undistorted image
		cv::Mat undistorted_image_out2; // output undistorted image
		cv::remap(/*in*/distorted_image, /*out*/undistorted_image_out2, map_x, map_y, CV_INTER_LINEAR, cv::BORDER_TRANSPARENT);
		cv::imwrite("undistorted_out2.jpg", undistorted_image_out2);
	}

	return 0;
}

// Compute the image size of the undistorted image
void computeUndistortedBoundary(/*in*/cv::Size &image_size, 
								/*in*/cv::Mat &K,
								/*in*/cv::Mat &distortionCoef,
								/*out*/cv::Mat &undistortedCorners,
								/*out*/double &dminx,
								/*out*/double &dminy,
								/*out*/double &dmaxx,
								/*out*/double &dmaxy)
{
	#define LARGE_IMAGE_COORD_D	(999999999.0)

	// Define the 4 image corners (original);
	cv::Mat distortedCorners(4, 1, CV_64FC2);
	distortedCorners.at<cv::Vec2d>(0,0)[0] = 0;						// Top-left
	distortedCorners.at<cv::Vec2d>(0,0)[1] = 0;
	distortedCorners.at<cv::Vec2d>(1,0)[0] = image_size.width-1;	// Top-right
	distortedCorners.at<cv::Vec2d>(1,0)[1] = 0;
	distortedCorners.at<cv::Vec2d>(2,0)[0] = image_size.width-1;	// Bottom-right
	distortedCorners.at<cv::Vec2d>(2,0)[1] = image_size.height-1;
	distortedCorners.at<cv::Vec2d>(3,0)[0] = 0;						// Bottom-left
	distortedCorners.at<cv::Vec2d>(3,0)[1] = image_size.height-1;

	// Undistorted the image corners
	cv::undistortPoints(distortedCorners, undistortedCorners, K, distortionCoef);

	// Compute the extreme corners
	dminx = LARGE_IMAGE_COORD_D;	dmaxx = -LARGE_IMAGE_COORD_D;
	dminy = LARGE_IMAGE_COORD_D;	dmaxy = -LARGE_IMAGE_COORD_D;

	for (int cor = 0; cor < 4; cor++)
	{
		// Un-normalized point coordinates
		double normalized_x, normalized_y, non_normalized_x, non_normalized_y;
		normalized_x = undistortedCorners.at<cv::Vec2d>(cor,0)[0]; 
		normalized_y = undistortedCorners.at<cv::Vec2d>(cor,0)[1];
		non_normalized_x = normalized_x * K.at<double>(0, 0) + K.at<double>(0, 2);
		non_normalized_y = normalized_y * K.at<double>(1, 1) + K.at<double>(1, 2);

		// Assign the undistorted corners the non-normalized values
		undistortedCorners.at<cv::Vec2d>(cor,0)[0] = non_normalized_x;
		undistortedCorners.at<cv::Vec2d>(cor,0)[1] = non_normalized_y;

		// Determine the two extreme points as the top left and bottom right corners
		if (non_normalized_x < dminx)
			dminx = non_normalized_x;
		if (non_normalized_y < dminy)
			dminy = non_normalized_y;
		if (non_normalized_x > dmaxx)
			dmaxx = non_normalized_x;
		if (non_normalized_y > dmaxy)
			dmaxy = non_normalized_y;
	}
}

// Produce distorted point (out_x, out_y) from undistort (in_x, in_y) point
void distortPoint(cv::Mat &K, cv::Mat &distortionCoef, 
				double in_x, double in_y, 
				/*out*/double &out_x, /*out*/double &out_y)
{
	// Camera parameters and distortion coefficients
	double fx = K.at<double>(0, 0);
	double fy = K.at<double>(1, 1);
	double cx = K.at<double>(0, 2);
	double cy = K.at<double>(1, 2);
	double k1 = distortionCoef.at<double>(0, 0);
	double k2 = distortionCoef.at<double>(0, 1);
	double p1 = distortionCoef.at<double>(0, 2);
	double p2 = distortionCoef.at<double>(0, 3);

	// Compute distorted points
	double x = (in_x - cx)/fx;
	double y = (in_y - cy)/fy;
	double r2 = x*x + y*y;
	double kcoef = 1 + k1*r2 + k2*r2*r2;
	double x_ = x*kcoef + 2*p1*x*y + p2*(r2 + 2*x*x);
	double y_ = y*kcoef + p1*(r2 + 2*y*y) + 2*p2*x*y;
	out_x = x_*fx + cx;
	out_y = y_*fy + cy;
}