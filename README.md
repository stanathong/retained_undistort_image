# Undistort Image - Retain All Source Pixels

A simple project to produce undistorted image in which all source image pixels are retained.<br>

![preview](https://github.com/stanathong/retained_undistort_image/blob/master/figure/preview.jpg)

The implementation is done using `cv::initUndistortRectifyMap` and `cv::getOptimalNewCameraMatrix`.<br>

## Implementation
The code is implemented using MS Visual Studio 2012. The project files and the source code are in the folder `retained_undistort_image`. Source code is in the file `main.cpp`. Using MS Visual Studio as the IDE, this requires some [project setting](https://github.com/stanathong/retained_undistort_image/blob/master/project_setting.md) (i.e. include/lib for OpenCV) in order to build the code.
