/*
 * Description: Produce undistorted image from raw image captured from the camera.
 * Input:
 *			    |fx 0  cx|
 *			K = |0  fy cy|
 *				|0  0   1|
 *
 *		dist_coef = |k1, k2, p1, p2|
 */

#include <string>
#include <iostream>

#include <opencv2/opencv.hpp>

int main(int argc, char *argv[])
{
	// Image to perform undistort
	std::string image_path = "G:/YORK-Project/Experiment/gaist_data/bradley_shorten/2017/20170111/130311/Carriageway_raw/Carriageway_37.jpg";
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

	// 3. Use the OpenCV's functions to compute undistorted image (with the same image_size)
	// 3.1 Get the new K matrix with 0 distortion
	cv::Mat new_K = cv::getOptimalNewCameraMatrix(K, distortionCoef, image_size, /*alpha*/1);
	// 3.2 Compute the undistortion and rectification transformation map
	cv::Mat map_x, map_y;
	cv::initUndistortRectifyMap(K, distortionCoef, cv::Mat(), new_K, image_size, CV_32FC1, map_x, map_y);
	cv::Mat undistorted_image_out1; // output undistorted image
	cv::remap(/*in*/distorted_image, /*out*/undistorted_image_out1, map_x, map_y, CV_INTER_LINEAR, cv::BORDER_TRANSPARENT);
	cv::imwrite("undistorted_out1.jpg", undistorted_image_out1);

	return 0;
}