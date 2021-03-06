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

#define SERVO 26
#define PWM 1
#define DIR 22
#define RANGE 200
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
	int ROI_heightH = floor(height/2);
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

	if (wiringPiSetup() == -1) return 1; //wiringPi error

//	softServoSetup(SERVO, -1, -1, -1, -1, -1, -1, -1);
	pinMode(SERVO, OUTPUT);
	pinMode(PWM, PWM_OUTPUT);
	pinMode(DIR, OUTPUT);

	softPwmCreate(SERVO, 0, RANGE);
	softPwmCreate(PWM, 0, RANGE);

	digitalWrite(DIR, HIGH);

	Mat image, edgeimg, curve_edgeimg;
	Mat ROIimg(height, width, CV_8UC1, Scalar(0));

	/* ROI image */
	for (int y=0; y<height; y++){
		for (int x=0; x<width; x++){
			if (y >= ROI_heightH && y <= ROI_heightL){
				if (x >= (ROI_widthL - (y - ROI_heightH)) && x <= (ROI_widthR + (y - ROI_heightH))){
					ROIimg.at<uchar>(y,x) = 255;
				}
			}
		}
	}
	/* ROI image end */

	bool do_flip = false;

	vector<Vec2f> lines;

	while (1) {
		t1 = getTickCount();

		cam.grab();
		cam.retrieve(image);

		if (do_flip)
			flip(image, image, -1);

		cvtColor(image, image, CV_BGR2GRAY);
		GaussianBlur(image, image, Size(3, 3), 0, 0);
		Canny(image, edgeimg, 100, 200);


		for (int y=ROI_heightH; y<ROI_heightL; y++){
			for (int x = width/2 ; x > ROI_widthL - (y - ROI_heightH); x--){
				if (edgeimg.at<uchar>(y,x) == 255) {
					x1 = x;
					y1 = y;
					break;
				}
				x1 = x;
				y1 = y;
			}
			for (int x = width/2; x < ROI_widthR + (y - ROI_heightH); x++){
				if (edgeimg.at<uchar>(y,x) == 255) {
					x2 = x;
					break;
				}
				x2 = x;
			}
			if ((x1 != ROI_widthR + (y - ROI_heightH) + 1) && (x2 != ROI_widthL - (y - ROI_heightH) - 1)){
				for (x1-=1; x1 > 0; x1--){
					edgeimg.at<uchar>(y1,x1) = 0;
				}
				for (x2+=1; x2 < width; x2++){
					edgeimg.at<uchar>(y1,x2) = 0;
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

		/* Houghline detection */
		HoughLines(edgeimg, lines, 1, CV_PI / 180, 20, 0, 0);

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
			float gradientL = (float)(pt2.y - pt1.y) / (float)(pt2.x - pt1.x);		// gradient 
			float interceptL = pt2.y - gradientL * pt2.x;							// y-intercept

			// rightLine : first linear equation
			float gradientR = (float)(pt4.y - pt3.y) / (float)(pt4.x - pt3.x);		// gradient
			float interceptR = pt4.y - gradientR * pt4.x;							// y-intercept

			// banishPoint : nodePoint of two equation
			banishP.x = (int)((interceptR - interceptL) / (gradientL - gradientR));
			banishP.y = (int)(gradientL * banishP.x + interceptL);
			line(image, pt2, banishP, Scalar(255, 0, 0), 2, CV_AA);
			line(image, pt3, banishP, Scalar(0, 0, 255), 2, CV_AA);

			value = (160 - banishP.x) * 2;
			softPwmWrite(PWM, 20);

			printf("***********Both Line Detect***********\n");
			printf("banishP.x : %d, value : %d\n", banishP.x, value);
			tag = 1;
		}

		// left 
		else if (pt1.x != 0 && pt3.x == 0) { 

			// left Point detection 

			// leftLine : first linear equation
			float gradientL = (float)(pt2.y - pt1.y) / (float)(pt2.x - pt1.x);		// gradient 
			float interceptL = pt2.y - gradientL * pt2.x;							// y-intercept

			// leftPoint : nodePoint of two equation
			leftP.x = (int)(interceptL / gradientL);
			leftP.y = (int)(gradientL * leftP.x + interceptL);
			line(image, pt2, leftP, Scalar(255, 0, 0), 2, CV_AA);

			value = (160 - leftP.x) * 2;
			softPwmWrite(PWM, 20);

			printf("***********Left Line Detect***********\n");
			printf("leftP.x : %d, value : %d\n", leftP.x, value);
			tag = 2;
		}

		// right 
		else if (pt1.x == 0 && pt3.x != 0) { 

			// right Point detection 

			// rightLine : first linear equation
			float gradientR = (float)(pt4.y - pt3.y) / (float)(pt4.x - pt3.x);		// gradient
			float interceptR = pt4.y - gradientR * pt4.x;							// y-intercept

			// rightPoint : nodePoint of two equation
			rightP.x = (int)(interceptR / gradientR);
			rightP.y = (int)(gradientR * rightP.x + interceptR);

			value = -(160 - rightP.x) * 2;

			line(image, pt3, rightP, Scalar(255, 0, 0), 2, CV_AA);
			softPwmWrite(PWM, 20);

			printf("***********Right Line Detect***********\n");
			printf("rightP.x : %d, value : %d\n", rightP.x, value);
			tag = 3;
		}

		// value normalization
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
		else {
			value = 0;
		}

		softPwmWrite(SERVO, 0);

		if (b_value !=  value) {
			printf("value : %d, thetaL : %0.2f, thetaR : %0.2f\n", value, thetaL, thetaR);
			softPwmWrite(SERVO, (standard + value));
			delay(100);
		}

		b_value = value;
		/* Servo controll end */

		imshow("Camera1", image);
		imshow("Camera2", edgeimg);
//		imshow("Camera4", ROIimg);

		t2 = getTickCount();
		cout << "It took " << (t2 - t1) * 1000 / getTickFrequency() << " ms." << endl;

		int k = waitKey(1);
		if (k == 27)
			break;
		else if (k == 'f' || k == 'F')
			do_flip = !do_flip;

	}

	softPwmWrite(PWM, 0);
	cam.release();
	destroyAllWindows();

	return 0;
}
