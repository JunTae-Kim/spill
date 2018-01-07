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

	int tag;
	Point pt1, pt2, pt3, pt4;
	float theta1, theta2;

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

	vector<Vec2f> lines;
	vector<Mat> ROI_planes;

	while (1) {
		cam.grab();
		cam.retrieve(image);

		if (do_flip)
			flip(image, image, -1);

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

		HoughLines(edgeimg, lines, 1, CV_PI / 180, 30, 0, 0);

		for (size_t i = 0; i < lines.size(); i++)
		{
			float rho = lines[i][0], theta = lines[i][1];
			float rho1, rho2;
			int length = 800;
			
			if (tag != 0)
			{
				pt1.x=0;
				pt1.y=0;
				pt2.x=0;
				pt2.y=0;
				pt3.x=0;
				pt3.y=0;
				pt4.x=0;
				pt4.y=0;
			}

			if (theta<1.5 && theta>=0)
			{
				theta1 = theta;
				rho1 = rho;
				double a1 = cos(theta1), b1 = sin(theta1);
				double x0 = a1*rho1, y0 = b1*rho1;

				pt1.x = cvRound(x0 - length * (-b1));
				pt1.y = cvRound(y0 - length * (a1));
				pt2.x = cvRound(x0 + length * (-b1));
				pt2.y = cvRound(y0 + length * (a1));

				tag = 0;
			}

			else if (theta<3.14 && theta>=2.0)
			{
				theta2 = theta;
				rho2 = rho;
				double a2 = cos(theta2), b2 = sin(theta2);
				double x02 = a2*rho2, y02 = b2*rho2;

				pt3.x = cvRound(x02 - length * (-b2));
				pt3.y = cvRound(y02 - length * (a2));
				pt4.x = cvRound(x02 + length * (-b2));
				pt4.y = cvRound(y02 + length * (a2));

				tag = 0;
			}
		}


		if (waitKey(30) == 27) {
			cout << "esc key is pressed by user" << endl;
		}

		else if (pt1.x != 0 && pt3.x != 0) { //forward 

			/* 소실점 검출 */

			// 왼쪽 차선의 1차 방정식
			float leftLineA = (float)(pt2.y - pt1.y) / (float)(pt2.x - pt1.x);	//기울기
			float leftLineB = pt2.y - leftLineA * pt2.x;						//y절편

			// 오른쪽 차선의 1차 방정식
			float rightLineA = (float)(pt4.y - pt3.y) / (float)(pt4.x - pt3.x);
			float rightLineB = pt4.y - rightLineA * pt3.x;

			// 1차 소실점: 좌우 1차선의 교점
			banishP.x = (int)((rightLineB - leftLineB) / (leftLineA - rightLineA));
			banishP.y = (int)(leftLineA * banishP.x + leftLineB);

			line(image, pt1, banishP, Scalar(255, 0, 0), 2, CV_AA);
			line(image, pt4, banishP, Scalar(0, 0, 255), 2, CV_AA);
			tag = 1;
			cout << "1" << endl;
		}

		else if (pt1.x != 0 && pt3.x == 0) { //left 
			line(image, pt1, pt2, Scalar(255, 0, 0), 2, CV_AA);
			tag = 2;
			cout << "2" << endl;
		}

		else if (pt1.x == 0 && pt3.x != 0) { //right 
			line(image, pt3, pt4, Scalar(0, 0, 255), 2, CV_AA);
			tag = 3;
			cout << "3" << endl;
		}

		else {
		}


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
