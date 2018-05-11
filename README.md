# Undistort Image - Retain All Source Pixels

A C++ project using the OpenCV library to produce undistorted image in which all source image pixels are retained.<br>

![preview](https://github.com/stanathong/retained_undistort_image/blob/master/figure/preview.jpg)

## Implementation
The code is implemented using MS Visual Studio 2012. The project files and the source code are in the folder `retained_undistort_image`. Source code is in the file `main.cpp`. Using MS Visual Studio as the IDE, this requires some [project setting](https://github.com/stanathong/retained_undistort_image/blob/master/project_setting.md) (i.e. include/lib for OpenCV) in order to build the code.<br>

## Code Explanation

We implement two methods to produce undistorted image. The first one is a simple and effortless method using `cv::initUndistortRectifyMap` and `cv::getOptimalNewCameraMatrix`. The second one is the reverse distortion technique in which we replicate the way OpenCV's `undistort()` function works (I believe?) in order to produce the undistorted image. <br>

__Input:__ input image (undistorted), $3x3$ intrinsics matrix (`K`) and distortion matrix <br>
__Output:__ undistorted images saved into files named `undistorted_out1.jpg` `and undistorted_out2.jpg` <br>

__Steps__<br>

1. Due to the distortion factors, it is most likely that the undistorted image will be bigger or smaller than the original image. To retain the resolution of the input image, we have to compute the image size of the undistorted image. With the four extreme corners of the input image: top-left (0,0), top-right (image_size.width-1, 0), bottom-right (image_size.width-1, image_size.height-1) and bottom-left (0, image_size.height-1), we undistort them to get the corresponding coords in the undistorted image.<br>

2. We create undistort image using the effortless method by calling `cv::initUndistortRectifyMap` and `cv::getOptimalNewCameraMatrix`.<br>

3. We use another technique to create undistort image. That is to do reverse distortion in which we create map matrices that define the correspondence between the undistorted coordinate and distorted (original image) coordinates. Then we call `cv::remap` to create the undistorted results.<br>

The equation to compute distorted coordinates from undistorted coordinates is<br>

![equation](https://github.com/stanathong/retained_undistort_image/blob/master/figure/equation.jpg)

_Note:_ In the code, I only define the distortion coefficients for k1, k2, p1 and p2.
