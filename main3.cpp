#include <opencv2/opencv.hpp>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <opencv/cv.hpp>
#include <opencv2/highgui.hpp>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <softServo.h>

using namespace cv;
using namespace std;

int main()
{
	int width = 320;
	int height = 240;
	int x1 = 0, y1 = 0, x2 = 0;

	Size framesize(width, height);

	raspicam::RaspiCam_Cv cam;

	cam.set(CV_CAP_PROP_FORMAT, CV_8UC3);
	cam.set(CV_CAP_PROP_FRAME_WIDTH, width);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, height);

	if (!cam.open()) {
		cerr << "Camera open failed!" << endl;
		return -1;
	}	

	if (wiringPiSetup() == -1) return 1; //wiringPi error

	Mat image, edgeimg;
	Mat ROIimg(height, width, CV_8UC1, Scalar(0));

	for (int y=0; y<height; y++){
		for (int x=0; x<width; x++){
			if (y >= 60 && y <= 180){
				if (x >= (80 - (y - 60)) && x <= (240 + (y - 60))){
					ROIimg.at<uchar>(y,x) = 255;
				}
			}
		}
	}
		
			

	bool do_flip = false;

	while (1) {
		cam.grab();
		cam.retrieve(image);

		cvtColor(image, image, CV_BGR2GRAY);
		GaussianBlur(image, image, Size(3, 3), 0, 0);
		Canny(image, edgeimg, 320, 350);

		for (int y=60; y<180; y++){
			for (int x = 80 - (y - 60); x <= 240 + (y - 60); x++){
				if (edgeimg.at<uchar>(y,x) == 255) {
					x1 = x;
					y1 = y;
					break;
				}
			}
			for (int x = 240 + (y - 60); x >= 80 - (y - 60); x--){
				if (edgeimg.at<uchar>(y,x) == 255) {
					x2 = x;
					break;
				}
			}
			if ((x1 != 240 + (y - 60) + 1) && (x2 != 80 - (y - 60) - 1)){
				for (x1+=1; x1 < x2; x1++){
					edgeimg.at<uchar>(y1,x1) = 0;
				}
			}
			
		}

		for (int y=0; y<height; y++){
			for (int x=0; x<width; x++){
				edgeimg.at<uchar>(y,x) = ROIimg.at<uchar>(y,x) & edgeimg.at<uchar>(y,x);
			}
		}

		x1 = 0;
		x2 = 0;
		y1 = 0;

		imshow("Camera1", image);
		imshow("Camera2", edgeimg);
		imshow("Camera3", ROIimg);

		int k = waitKey(1);
		if (k == 27)
			break;
		else if (k == 'f' || k == 'F')
			do_flip = !do_flip;

	}

	cam.release();
	destroyAllWindows();

	return 0;
}
