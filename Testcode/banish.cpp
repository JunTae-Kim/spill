#include <opencv2/opencv.hpp>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <opencv/cv.hpp>
#include <opencv2/highgui.hpp>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define standard 8
#define pi 3.141592

using namespace cv;
using namespace std;

int main()
{
	int width = 320;
	int height = 240;

	int ROI_widthL = floor(width/4);
	int ROI_widthR = floor(width*3/4);
	int ROI_heightH = floor(height/4);
	int ROI_heightL = floor(height*3/4);

	int value = 0;
	int b_value = 0;
	int tag;
	int64 t1, t2;
	float theta1 = 0, theta2 = 0;
	float thetaL, thetaR;
	Point pt1, pt2;		//left line  : up_point, down_point
	Point pt4, pt3;		//right line : up_point, down_point
	Point leftP;		//left point
	Point rightP;		//right point
	Point banishP;		//banish point
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

	Mat image, grayimg, edgeimg, blurimg, closeimg, openimg, dilimg, erimg;
	Mat ROIimg(height, width, CV_8UC1, Scalar(0));

	/* ROI image */
	for (int y=0; y<height; y++){
		for (int x=0; x<width; x++){
			if (y >= 120 && y < 240){
				if (x >= (80 - (y - 120)) && x <= (240 + (y - 120))){
					ROIimg.at<uchar>(y,x) = 255;
				}
			}
		}
	}
	/* ROI image end */


	bool do_flip = false;

	vector<Vec2f> lines;

	while (1) {
		cam.grab();
		cam.retrieve(image);

		if (do_flip)
			flip(image, image, -1);

		cvtColor(image, grayimg, CV_BGR2GRAY);
		GaussianBlur(grayimg, blurimg, Size(3, 3), 0, 0);

		Canny(blurimg, edgeimg, 100, 300);

		int element_shape = MORPH_RECT;
		Mat element = getStructuringElement(element_shape, Size(3,3));
		Mat element2 = getStructuringElement(element_shape, Size(5,5));


		dilate(edgeimg, dilimg, element);
		dilate(dilimg, dilimg, element);
		morphologyEx(dilimg, closeimg, MORPH_CLOSE, element);
		erode(closeimg, erimg, element2);
		erode(erimg, erimg, element2); 

		t1 = getTickCount();

		for (int y=0; y<height; y++){
			for (int x=0; x<width; x++){
				erimg.at<uchar>(y,x) = ROIimg.at<uchar>(y,x) & erimg.at<uchar>(y,x);
			}
		}

		x1 = 0;
		x2 = 0;
		y1 = 0;

		/* Houghline detection */
		HoughLines(erimg, lines, 1, CV_PI / 180, 30, 0, 0);

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

			else if (theta<3.14 && theta>=1.57)
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
		theta1 = 1.57 - theta1;
		theta2 = 4.71 - theta2;
		thetaL = theta1 * 180 / pi;
		thetaR = theta2 * 180 / pi;
		/* Houghline detection end */

		/* Servo controll */
		if (waitKey(30) == 27) {
			cout << "esc key is pressed by user" << endl;
		}

		// forward 
		else if (pt1.x != 0 && pt3.x != 0) { 

			// banish Point detection 

			// leftLine : first linear equation
			float leftLineA = (float)(pt2.y - pt1.y) / (float)(pt2.x - pt1.x);	//기울기
			float leftLineB = pt2.y - leftLineA * pt2.x;						//y절편

			// rightLine : first linear equation
			float rightLineA = (float)(pt4.y - pt3.y) / (float)(pt4.x - pt3.x);
			float rightLineB = pt4.y - rightLineA * pt4.x;

			// banishPoint : nodePoint of two equation
			banishP.x = (int)((rightLineB - leftLineB) / (leftLineA - rightLineA));
			banishP.y = (int)(leftLineA * banishP.x + leftLineB);

			value = (160 - banishP.x)*2;

			line(image, pt2, banishP, Scalar(255, 0, 0), 2, CV_AA);
			line(image, pt3, banishP, Scalar(0, 0, 255), 2, CV_AA);

			printf("***********Both Line Detect***********\n");
			tag = 1;
		}

		// left 
		else if (pt1.x != 0 && pt3.x == 0) { 
			line(image, pt1, pt2, Scalar(255, 0, 0), 2, CV_AA);

			value = floor(abs((300 / 15)*(48 - thetaL)));

			printf("***********Left Line Detect***********\n");
			tag = 2;
		}

		// right 
		else if (pt1.x == 0 && pt3.x != 0) { 
			line(image, pt3, pt4, Scalar(0, 0, 255), 2, CV_AA);

			value = -floor(abs((340 / 15)*(131 - thetaR)));

			printf("***********Right Line Detect***********\n");
			tag = 3;
		}

		if (value >= -50 && value <= 50) {
			value = 0;
		}
		else if (value >= -150 && value < -50) {
			value = -1;
		}
		else if (value >= -250 && value < -150) {
			value = -2;
		}
		else if (value < -250) {
			value = -3;
		}
		else if (value > 50 && value <= 150) {
			value = 1;
		}
		else if (value > 150 && value <= 250) {
			value = 2;
		}
		else if (value > 250) {
			value = 3;
		}
	//	else {
	//		value = 0;
	//	}

		int input = standard + value;

		if (b_value !=  value) {
			printf("input : %d, thetaL : %0.2f, thetaR : %0.2f\n", input, thetaL, thetaR);
		}

		b_value = value;
		thetaL=0;
		thetaR=0;
		tag=1;

		t2 = getTickCount();
		cout << "It took " << (t2 - t1) * 1000 / getTickFrequency() << " ms." << endl;


		imshow("image", image);
		imshow("closeimg", closeimg);
		imshow("edgeimg", edgeimg);
		imshow("erimg", erimg);
		imshow("ROIimg", ROIimg);

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
