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

#define SERVO 26
//#define RANGE1 100
#define RANGE2 200
#define PWM 1
#define DIR 22
//#define ENABLE 23
#define pi 3.141592
#define standard_L 55
#define standard_R 125
#define standard 580


using namespace cv;
using namespace std;

int main()
{
	int width = 320;
	int height = 240;
	int value = 0;
	int b_value = 580;
	int  k;
	int num=5;
	double thetaL_array[num-1];
	double thetaR_array[num-1];
	float thetaR, thetaL;
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

	softServoSetup(SERVO, -1, -1, -1, -1, -1, -1, -1);

	Mat image, edgeimg, to_hsv1, lower_red_hue_range, upper_red_hud_range;
	Mat red_hue_image, hsv, ROIframe, ROIimg;
	int64 t1, t2;
	bool do_flip = false;
	int tag; int tag2 = 0;
	int n = 3;
	Point pt1, pt2, pt3, pt4;
	float theta1 = 0; float theta2 = 0;

	pinMode(PWM,PWM_OUTPUT);
	pinMode(SERVO,OUTPUT);
	pinMode(DIR,OUTPUT);
//	pinMode(ENABLE,OUTPUT);


//	softPwmCreate(SERVO,0,RANGE1);
	softPwmCreate(PWM,0,RANGE2);

	vector<Vec2f> lines;
	vector<Mat> ROI_planes;

	while (1) {
		cam.grab();
		cam.retrieve(image);

		cvtColor(image, image, CV_BGR2GRAY);
		GaussianBlur(image, image, Size(3, 3), 0, 0);

		Rect rect(0, height / 3, width, height * 2 / 3);
		Mat subimg = image(rect);

		Canny(subimg, edgeimg, 320, 350);

		HoughLines(edgeimg, lines, 1, CV_PI / 180, 60, 0, 0);

		t1 = getTickCount();

		for (k = 0; k < num; k++)
		{
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

			theta1 = 1.57 - theta1;
			theta2 = 4.71 - theta2;
			thetaL_array[k] = theta1 * 180/pi;
			thetaR_array[k] = theta2 * 180/pi;
			thetaL += thetaL_array[k];
			thetaR += thetaR_array[k];
			delay(10);
		}

		thetaL = thetaL / num;
		thetaR = thetaR / num;

		if (waitKey(30) == 27) {
			cout << "esc key is pressed by user" << endl;
		}

		else if (pt1.x != 0 && pt3.x != 0) { //forward 
			line(subimg, pt1, pt2, Scalar(255, 0, 0), 4, CV_AA);
			line(subimg, pt3, pt4, Scalar(0, 0, 255), 4, CV_AA);
//			value = floor(abs((thetaR-standard_R)/4));
			value = 0;
			softPwmWrite(PWM, 35);
//			value = floor(abs(300/15)*(45-thetaL));
/*			if (thetaL <= 30)
			{
				value = floor(abs((thetaL-standard_L)/5));
			}
*/			printf("!!!!!both lines : %d\n", value);
		}

		else if (pt1.x != 0 && pt3.x == 0) { //left 
			line(subimg, pt1, pt2, Scalar(255, 0, 0), 4, CV_AA);
//			value = floor(abs((standard_L-thetaL)/5));
			if (value > 250)
				value = 340;
			else
				value = floor(abs((300/15)*(45-thetaL)));
			printf("left lines : %d\n", value);
			softPwmWrite(PWM, 50);

		}

		else if (pt1.x == 0 && pt3.x != 0) { //right 
			line(subimg, pt3, pt4, Scalar(0, 0, 255), 4, CV_AA);
//			value = -floor(abs((thetaR-standard_R)/5));
			if (value > -210)
				value = -300;
			else
				value = -floor(abs((340/15)*(135-thetaR)));
			printf("right lines : %d\n", value);
			softPwmWrite(PWM, 50);
		}

/*
		if ((theta1 > 0.66 && theta1 < 0.83) || (theta2 > 2.2 && theta2 < 2.3)) {
			softPwmWrite(SERVO, 15);
			printf("15, theta1 : %f theta2 : %f\n",theta1, theta2);
			delay(100);
		}
		else if (theta1 <= 0.66) {
			softPwmWrite(SERVO, 16);
			printf("16, theta1 : %f theta2 : %f\n",theta1, theta2);
			delay(100);
		}
		else if (theta1 >= 0.83 || theta2 >= 2.3) {
			softPwmWrite(SERVO, 14);
			printf("14, theta1 : %f theta2 : %f\n",theta1, theta2);
			delay(100);
		}
*/
		printf("theta1 : %f, theta2 : %f\n",theta1, theta2);
		printf("thetaL : %f, thetaR : %f\n",thetaL, thetaR);

		tag = 1;

		if (value > 340) {
			value = 340;
		}
		else if (value < -300) {
			value = -300;
		}

		if (b_value != (standard + value)) {
			printf("value : %d\n",(standard+ value));
			softServoWrite(SERVO, (standard+value));
		}
		digitalWrite(DIR, HIGH);

		thetaL = 0;
		thetaR = 0;
		tag = 1;
		b_value = standard + value;


		t2 = getTickCount();
		cout << "It took " << (t2 - t1) * 1000 / getTickFrequency() << " ms." << endl;

		imshow("CAM", subimg);
		imshow("Camera1", image);
		imshow("Camera2", edgeimg);

		int k = waitKey(1);
		if (k == 27)
			break;
		else if (k == 'f' || k == 'F')
			do_flip = !do_flip;

	} // While End 

	softPwmWrite(PWM, 0);
//	digitalWrite(ENABLE, HIGH);
	cam.release();
	destroyAllWindows();

	return 0;
}
