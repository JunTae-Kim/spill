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
#define PWM 21
#define DIR 22
#define ENABLE 23

using namespace cv;
using namespace std;

int main()
{
	int Width = 320;
	int Height = 240;

	raspicam::RaspiCam_Cv cam;

	cam.set(CV_CAP_PROP_FORMAT, CV_8UC3);
	cam.set(CV_CAP_PROP_FRAME_WIDTH, Width);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, Height);

	if (!cam.open()) 
	{
		cerr << "Camera open failed!" << endl;
		return -1;
	}

	if (wiringPiSetup() == -1)
	{
		return 1; //wiringPi error
	}

	Mat image, edgeimg, houghimg;
	int64 t1, t2;

	bool do_flip = false;
	int tag = 0;

	Point pt1, pt2, pt3, pt4;

	softPwmCreate(SERVO, 0, 200);
	softPwmCreate(PWM, 0, 200);


	pinMode(SERVO, PWM_OUTPUT);
	pinMode(PWM, PWM_OUTPUT);
	pinMode(DIR, OUTPUT);
	pinMode(ENABLE, OUTPUT);

	vector<Vec2f> lines;

	while (1) 
	{
		cam.grab();
		cam.retrieve(image);
		image.copyTo(houghimg);

		imshow("image", image);

		if (do_flip)
		{
			flip(image, image, -1);
		}

		Canny(image, edgeimg, 350, 400);

		imshow("edgeimg", edgeimg);

		HoughLines(edgeimg, lines, 1, CV_PI / 180, 100, 0, 0);

		t1 = getTickCount();

		for (size_t i = 0; i < lines.size(); i++) 
		{
			float rho = lines[i][0], theta = lines[i][1];
			float theta1, theta2;
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

				angle = (atan2(pt1.y - pt2.y, pt1.x - pt2.x))*(180 / CV_PI);
				printf("floats : %f\n", angle); //printing angle 

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

				angle2 = (atan2(pt3.y - pt4.y, pt3.x - pt4.x))*(180 / CV_PI);
				printf("floats2 : %f\n", angle2); //printing angle °¢µµÈ®ÀÎÀ» À§ÇÑ ÇÁž°Æ®ÄÚµå 

				tag = 0;
			}
		}

		if (waitKey(30) == 27) 
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}

		else if (pt1.x != 0 && pt3.x != 0)
		{
			line(houghimg, pt1, pt2, Scalar(255, 0, 0), 2, CV_AA);
			line(houghimg, pt3, pt4, Scalar(0, 0, 255), 2, CV_AA);

			printf("forward\n");
			softPwmWrite(SERVO, 14);
			softPwmWrite(PWM, 80);
			digitalWrite(DIR, LOW);
			digitalWrite(ENABLE, LOW);
			delay(500);
			tag = 1;

		}

		else if (pt1.x != 0 && pt3.x == 0) 
		{
			line(houghimg, pt1, pt2, Scalar(255, 0, 0), 2, CV_AA);

			printf("right turn\n");

			softPwmWrite(SERVO, 18);
			softPwmWrite(PWM, 100);
			digitalWrite(DIR, LOW);
			digitalWrite(ENABLE, LOW);
			delay(500);
			tag = 2;

		}

		else if (pt1.x == 0 && pt3.x != 0)
		{
			line(houghimg, pt3, pt4, Scalar(0, 0, 255), 2, CV_AA);
		}

		imshow("houghimg", houghimg);
	}

	cam.release();
	destroyAllWindows();

}

