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

	raspicam::RaspiCam_Cv cam;

	cam.set(CV_CAP_PROP_FORMAT, CV_8UC3);
	cam.set(CV_CAP_PROP_FRAME_WIDTH, width);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, height);

	if (!cam.open()) {
		cerr << "Camera open failed!" << endl;
		return -1;
	}	
/*
	CascadeClassifier cascade("haarcascade_frontalface_default.xml");

	if (cascade.empty()) {
		cerr << "Failed to open xml file!" << endl;
		return -1;
	}
*/

	if (wiringPiSetup() == -1) return 1; //wiringPi error

	Mat image, edgeimg, to_hsv1, lower_red_hue_range, upper_red_hud_range, red_hue_image, hsv, ROIframe;
//	int64 t1, t2;
	bool do_flip = false;
	int tag; int tag2 = 0;
	int n = 3;
	Point pt1, pt2, pt3, pt4;

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
		image.copyTo(ROIframe);

		if (do_flip)
			flip(image, image, -1);

		GaussianBlur(ROIframe, ROIframe, Size(3, 3), 0, 0);
		Canny(image, edgeimg, 350, 400);

		cvtColor(ROIframe, hsv, CV_BGR2HSV);
		cvtColor(ROIframe, to_hsv1, CV_BGR2HSV);

		inRange(hsv, Scalar(0, 10, 10), Scalar(15, 255, 255), red_hue_image);
		inRange(to_hsv1, Scalar(5, 70, 230), Scalar(70, 255, 255), ROIframe);

		int element_shape = MORPH_RECT;
		Mat element = getStructuringElement(element_shape, Size(n,n));
		dilate(red_hue_image, red_hue_image, element);


		HoughLines(edgeimg, lines, 1, CV_PI / 180, 100, 0, 0);

//		t1 = getTickCount();

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
			softPwmWrite(SERVO, 15);
			softPwmWrite(PWM, 80);
			digitalWrite(DIR, LOW);
			digitalWrite(ENABLE, LOW);
			delay(500);
			tag = 1;
			tag2 = 1;

		}

		else if (pt1.x != 0 && pt3.x == 0) { //left 
			line(image, pt1, pt2, Scalar(255, 0, 0), 2, CV_AA);

			if (tag2 != 2)
			{
				printf("right turn\n");
			}
			softPwmWrite(SERVO, 18);
			softPwmWrite(PWM, 100);
			digitalWrite(DIR, LOW);
			digitalWrite(ENABLE, LOW);
			delay(500);
			tag = 2;
			tag2 = 2;
		}

		else if (pt1.x == 0 && pt3.x != 0) { //right 
			line(image, pt3, pt4, Scalar(0, 0, 255), 2, CV_AA);

			if (tag2 != 3)
			{
				printf("left turn\n");
			}
			softPwmWrite(SERVO, 12);
			softPwmWrite(PWM, 100);
			digitalWrite(DIR, LOW);
			digitalWrite(ENABLE, LOW);
			delay(500);
			tag = 3;
			tag2 = 3;
		}

		else {
		}

//		t2 = getTickCount();
//		cout << "It took " << (t2 - t1) * 1000 / getTickFrequency() << " ms." << endl;

		imshow("Camera1", image);
		imshow("Camera2", edgeimg);

		int k = waitKey(1);
		if (k == 27)
			break;
		else if (k == 'f' || k == 'F')
			do_flip = !do_flip;

	} // While End 



/*
		unsigned char* data = (unsigned char*)img.data;

//		t1 = getTickCount();


		for (i = 0;i < height;i++)
		{
			for (j = 0;j < width;j++)
			{
				if (data[i*width + j] > 110)
				{
					data[i*width + j] = 255;

					if (j==319 && i>160)
					{
						data[i*width + j] = 0;
					}
				}
	
				else
				{
					data[i*width + j] = 0;
					
					if (j==319 && i>160)
					{
						data[i*width + j] = 255;
					}
				}
			}
		}

		img.data=data;
		Canny(img, edgeimg, 600, 750);

		for (i=0;i<width;i++)
		{
			if (data[200*width + i] == 255)
			{
				break;
			}
		}

		for (j=width;j>0;j--)
		{
			if (data[200*width + j] == 255)
			{
				break;
			}
		}

		middot1 = (int)((i+j) / 2);

		for (i=0;i<width;i++)
		{
			if (data[300*width + i] == 255)
			{
				break;
			}
		}

		for (j=width;j>0;j--)
		{
			if (data[300*width + j] == 255)
			{
				break;
			}
		}

		middot2 = (int)((i+j) / 2);

		dir = middot2 - middot1; 

		if (abs(dir) < 30)
		{
			printf("go on\n");
		}
		else if (30<=dir && dir<=50)
		{
			printf("left turn\n");
		}
		else if (dir>50)
		{
			printf("strong left turn\n");
		}
		else if (-50<=dir && dir<=-30)
		{
			printf("right turn\n");
		}
		else if (dir<-50)
		{
			printf("strong right turn\n");
		}
		else
		{
			printf("error\n");
		}

		t2 = getTickCount();
		cout << "It took " << (t2 - t1) * 1000 / getTickFrequency() << " ms." << endl;

		imshow("cam", img);
		imshow("cam2", edgeimg);


		int k = waitKey(1);
		if (k == 27)
			break;
		else if (k == 'f' || k == 'F')
			do_flip = !do_flip;
	}


*/
	digitalWrite(ENABLE, HIGH);
	cam.release();
	destroyAllWindows();

	return 0;
}
