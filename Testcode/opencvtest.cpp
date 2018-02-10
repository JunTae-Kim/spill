#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv/cv.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

int main()
(
	Mat img = imread('road.jpg');
	Mat src;
	src.create(size(img), CV_8UC3);

	dilate(img, dst);
	imshow("input",img);
	imshow("output",dst);
	WaitKey(0);

	ReleaseImage(&img);
	ReleaseImage(&dst);

	return 0;

}