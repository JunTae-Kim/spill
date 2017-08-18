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

	if (!cam.open()) {
		cerr << "Camera open failed!" << endl;
		return -1;
	}

	if (wiringPiSetup() == -1) return 1; //wiringPi error

	printf("1point\n");

	Mat image, edgeimg, to_hsv1, lower_red_hue_range, upper_red_hud_range, red_hue_image, hsv, ROIframe;
//	int64 t1, t2;
	bool do_flip = false;
<<<<<<< HEAD:test1.cpp
	int tag;
=======
	int tag = 0;
>>>>>>> 968c0d700ace723fce75dc0c579ddd14a9d79891:test2.cpp
	Point pt1, pt2, pt3, pt4;
//	float angle, angle2;
	softPwmCreate(SERVO, 0, 200);
	softPwmCreate(PWM, 0, 200);

	printf("2point\n");

	pinMode(SERVO, PWM_OUTPUT);
	pinMode(PWM, PWM_OUTPUT);
	pinMode(DIR, OUTPUT);
	pinMode(ENABLE, OUTPUT);

	vector<Vec2f> lines;
	vector<Mat> ROI_planes;

	printf("3point\n");

	while (1) {
		cam.grab();
		cam.retrieve(image);
		image.copyTo(ROIframe);

printf("h1\n");
		if (do_flip)
			flip(image, image, -1);

<<<<<<< HEAD:test1.cpp
		GaussianBlur(ROIframe, ROIframe, Size(3, 3), 0, 0);
		Canny(image, edgeimg, 350, 400);

=======
printf("h2\n");
		GaussianBlur(ROIframe, ROIframe, Size(3, 3), 0, 0);
printf("h3\n");
	Canny(image, edgeimg, 350, 400);
printf("h4\n");
printf("h4\n");
printf("h4\n");
>>>>>>> 968c0d700ace723fce75dc0c579ddd14a9d79891:test2.cpp
		cvtColor(ROIframe, hsv, CV_BGR2HSV);
		cvtColor(ROIframe, to_hsv1, CV_BGR2HSV);

	printf("4point\n");

		inRange(hsv, Scalar(0, 10, 10), Scalar(15, 255, 255), red_hue_image);
		inRange(to_hsv1, Scalar(5, 70, 230), Scalar(70, 255, 255), ROIframe);

		int element_shape = MORPH_RECT;
		Mat element = getStructuringElement(element_shape, Size(3,3));
		dilate(red_hue_image, red_hue_image, element);

		HoughLines(edgeimg, lines, 1, CV_PI / 180, 100, 0, 0);

//		t1 = getTickCount();

	printf("5point\n");
		for (size_t i = 0; i < lines.size(); i++) // °ËÃâµÈ Æ÷ÀÎÆ®žŠ Â÷Œ±Àž·Î ¿¬°á. 
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

//				angle = (atan2(pt1.y - pt2.y, pt1.x - pt2.x))*(180 / CV_PI);
//				printf("floats : %f\n", angle); //printing angle 

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

//				angle2 = (atan2(pt3.y - pt4.y, pt3.x - pt4.x))*(180 / CV_PI);
//				printf("floats2 : %f\n", angle2); //printing angle °¢µµÈ®ÀÎÀ» À§ÇÑ ÇÁž°Æ®ÄÚµå 

				tag = 0;
			}
		}

	printf("6point\n");

		if (waitKey(30) == 27) {
			cout << "esc key is pressed by user" << endl;
			break;
		}

		else if (pt1.x != 0 && pt3.x != 0) {
			line(image, pt1, pt2, Scalar(255, 0, 0), 2, CV_AA);
			line(image, pt3, pt4, Scalar(0, 0, 255), 2, CV_AA);

			printf("forward\n");
			softPwmWrite(SERVO, 14);
			softPwmWrite(PWM, 80);
			digitalWrite(DIR, LOW);
			digitalWrite(ENABLE, LOW);
			delay(500);
			tag = 1;


	printf("7point\n");
		}

		else if (pt1.x != 0 && pt3.x == 0) {
			line(image, pt1, pt2, Scalar(255, 0, 0), 2, CV_AA);

			printf("right turn\n");

			softPwmWrite(SERVO, 18);
			softPwmWrite(PWM, 100);
			digitalWrite(DIR, LOW);
			digitalWrite(ENABLE, LOW);
			delay(500);
			tag = 2;

	printf("8point\n");
		}

		else if (pt1.x == 0 && pt3.x != 0) {
			line(image, pt3, pt4, Scalar(0, 0, 255), 2, CV_AA);
}
}
}

