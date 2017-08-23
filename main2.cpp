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

#define SERVO 1
#define RANGE1 100
#define RANGE2 200
#define PWM 21
#define DIR 22
#define ENABLE 23

using namespace cv;
using namespace std;

int main()
{
	int width = 320;
	int height = 240;
	int fps = 20;
	Size framesize(width, height);

	raspicam::RaspiCam_Cv cam;

	cam.set(CV_CAP_PROP_FORMAT, CV_8UC3);
	cam.set(CV_CAP_PROP_FRAME_WIDTH, width);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, height);

	VideoWriter oVideoWriter("/home/pi/spill/test.avi",CV_FOURCC('P','I','M','1'),fps,framesize, true);

	if (!oVideoWriter.isOpened())
	{
		cout << "ERROR : failed to write the video" << endl;
	}

	if (!cam.open()) {
		cerr << "Camera open failed!" << endl;
		return -1;
	}	

	if (wiringPiSetup() == -1) return 1; //wiringPi error

	Mat image, edgeimg, to_hsv1, lower_red_hue_range, upper_red_hud_range;
	Mat red_hue_image, hsv, ROIframe, ROIimg, mask;
//	int64 t1, t2;
	bool do_flip = false;
	int tag; int tag2 = 0;
	int n = 3;
	Point pt1, pt2, pt3, pt4;
	float theta1, theta2;

	pinMode(PWM,PWM_OUTPUT);
	pinMode(SERVO,PWM_OUTPUT);
	pinMode(DIR,OUTPUT);
	pinMode(ENABLE,OUTPUT);


	softPwmCreate(SERVO,0,RANGE1);
	softPwmCreate(PWM,0,RANGE2);

	vector<Vec2f> lines;
	vector<Mat> ROI_planes;

	while (1) {
		cam.grab();
		cam.retrieve(image);

		cvtColor(image, image, CV_BGR2GRAY);
		GaussianBlur(image, image, Size(3, 3), 0, 0);
		Canny(image, edgeimg, 300, 350);

//		cvtColor(ROIframe, hsv, CV_BGR2HSV);
//		cvtColor(ROIframe, to_hsv1, CV_BGR2HSV);

//		inRange(hsv, Scalar(0, 10, 10), Scalar(15, 255, 255), red_hue_image);
//		inRange(to_hsv1, Scalar(5, 70, 230), Scalar(70, 255, 255), ROIframe);

//		int element_shape = MORPH_RECT;
//		Mat element = getStructuringElement(element_shape, Size(n,n));
//		dilate(red_hue_image, red_hue_image, element);

//		vertices = array([[(50, height), (width/2-45, height/2+60), (width/2+45, height/2+60), (width-50, height)]]);

//		mask = edgeimg;
//		fillPoly(mask, vertices, Scalar(255,255,255));
		

		HoughLines(edgeimg, lines, 1, CV_PI / 180, 30, 10, 20);

//		t1 = getTickCount();

		for (size_t i = 0; i < lines.size(); i++) // °ËÃâµÈ Æ÷ÀÎÆ®žŠ Â÷Œ±Àž·Î ¿¬°á. 
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
			line(image, pt1, pt2, Scalar(255, 0, 0), 2, CV_AA);
			line(image, pt3, pt4, Scalar(0, 0, 255), 2, CV_AA);

			if (tag2 != 1)
			{
				printf("forward\n");
			}
			if (theta1 > 0.79 && theta1 < 0.83) {
				softPwmWrite(SERVO, 15);
				printf("15, theta1 : %f\n",theta1);
			}
			else if (theta1 <= 0.79) {
				softPwmWrite(SERVO, 16);
				printf("14, theta1 : %f\n",theta1);
			}
			else if (theta1 >= 0.83) {
				softPwmWrite(SERVO, 14);
				printf("16, theta1 : %f\n",theta1);
			}

			softPwmWrite(PWM, 50);
			digitalWrite(DIR, LOW);
			digitalWrite(ENABLE, LOW);
			tag = 1;
			tag2 = 1;

		}

		else if (pt1.x != 0 && pt3.x == 0) { //left 
			line(image, pt1, pt2, Scalar(255, 0, 0), 2, CV_AA);

			if (tag2 != 2)
			{
				printf("right turn\ntheta1 : %f\n",theta1);
			}
			if (theta1 > 0.79 && theta1 < 0.83) {
				softPwmWrite(SERVO, 16);
				printf("right, theta1 : %f\n",theta1);
			}
			else if (theta1 <= 0.79) {
				softPwmWrite(SERVO, 17);
				printf("right, theta1 : %f\n",theta1);
			}
			else if (theta1 >= 0.83) {
				softPwmWrite(SERVO, 15);
				printf("right, theta1 : %f\n",theta1);
			}

			softPwmWrite(PWM, 70);
			digitalWrite(DIR, LOW);
			digitalWrite(ENABLE, LOW);
			tag = 2;
			tag2 = 2;
		}

		else if (pt1.x == 0 && pt3.x != 0) { //right 
			line(image, pt3, pt4, Scalar(0, 0, 255), 2, CV_AA);

			if (tag2 != 3)
			{
				printf("left turn\ntheta1 : %f\n", theta1);
			}
			if (theta2 > 2.3 && theta2 < 2.6) {
				softPwmWrite(SERVO, 14);
				printf("left, theta2 : %f\n",theta2);
			}
			else if (theta2 <= 2.3) {
				softPwmWrite(SERVO, 16);
				printf("left, theta2 : %f\n",theta2);
			}
			else if (theta2 >= 2.6) {
				softPwmWrite(SERVO, 10);
				printf("left, theta2 : %f\n",theta2);
			}
			softPwmWrite(PWM, 70);
			digitalWrite(DIR, LOW);
			digitalWrite(ENABLE, LOW);
			tag = 3;
			tag2 = 3;
		}
/*
		else {
			line(image, pt1, pt2, Scalar(255, 0, 0), 2, CV_AA);
			line(image, pt3, pt4, Scalar(0, 0, 255), 2, CV_AA);

			if ((theta1 > 0.79 && theta1 < 0.83) || (theta2 > 2.3 && theta2 < 2.6)) {
				printf("forword\n");
				softPwmWrite(SERVO, 15);
				softPwmWrite(PWM, 70);
				printf("15, theta1 : %f, theta2 : %f\n",theta1, theta2);
			}
			else if (theta1 >= 0.7 && theta1 <= 0.79) {
				softPwmWrite(SERVO, 14);
				softPwmWrite(PWM, 60);
				printf("14, theta1 : %f, theta2 : %f\n",theta1, theta2);
			}
			else if (theta2 <= 2.8 && theta2 >= 2.6) {
				softPwmWrite(SERVO, 16);
				softPwmWrite(PWM, 60);
				printf("16, theta1 : %f, theta2 : %f\n",theta1, theta2);
			}
			else if ((theta1 >= 0.5 && theta1 < 0.7) || (theta2 <= 2.6 && theta2 > 2.5))	{
				softPwmWrite(SERVO, 13);
				softPwmWrite(PWM, 50);
				printf("13, theta1 : %f, theta2 : %f\n",theta1, theta2);
			}
			else if ((theta1 >= 0.3 && theta1 < 0.5) || (theta2 <= 2.6 && theta2 > 2.4))	{
				softPwmWrite(SERVO, 12);
				softPwmWrite(PWM, 40);
				printf("12, theta1 : %f, theta2 : %f\n",theta1, theta2);
			}
			else if (theta1 < 0.3 || (theta2 > 2.4))	{
				softPwmWrite(SERVO, 11);
				softPwmWrite(PWM, 40);
				printf("11, theta1 : %f, theta2 : %f\n",theta1, theta2);
			}
			else if (theta2 > 2.7) {
				softPwmWrite(SERVO, 17);
				softPwmWrite(PWM, 50);
				printf("17, theta1 : %f, theta2 : %f\n",theta1, theta2);
			}

			digitalWrite(DIR, LOW);
			digitalWrite(ENABLE, LOW);

		}
*/

//		t2 = getTickCount();
//		cout << "It took " << (t2 - t1) * 1000 / getTickFrequency() << " ms." << endl;

		oVideoWriter.write(image);

		imshow("Camera1", image);
		imshow("Camera2", edgeimg);

		int k = waitKey(1);
		if (k == 27)
			break;
		else if (k == 'f' || k == 'F')
			do_flip = !do_flip;

	} // While End 

	digitalWrite(ENABLE, HIGH);
	cam.release();
	destroyAllWindows();

	return 0;
}
