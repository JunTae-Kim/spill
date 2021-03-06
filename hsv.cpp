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

using namespace cv;
using namespace std;

void pwm(int milliseconds, int pin);

#define SERVO_CAM 28
#define PIR 25
#define SERVO 26
#define PWM 1
#define DIR 22
#define RANGE 200
#define standard 8
#define pi 3.141592

Mat img_hsv, img_binary1, img_binary2, img_binary3, CUTimage;

void pwm(int milliseconds, int pin);
void Traffic_light(Mat img_input);	

int width = 320;
int height = 240;

int Count_R = 0;
int Count_Y = 0;
int Count_GR = 0;
int Count_GL = 0;
int signal = 0;
int dir;


int main()
{
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

	pinMode(PIR, INPUT);
	pinMode(PWM, OUTPUT);


	int64 t1, t2;
	int tag = 0;
	int Pcount = 0;

	//cvCreateTrackbar("LowH", "setting", &LowH, 179);
	//cvCreateTrackbar("HighH", "setting", &HighH, 179);

	//cvCreateTrackbar("LowS", "setting", &LowS, 255);
	//cvCreateTrackbar("HighS", "setting", &HighS, 255);

	//cvCreateTrackbar("LowV", "setting", &LowV, 255);
	//cvCreateTrackbar("HighV", "setting", &HighV, 255);


	pwm(1850, SERVO_CAM);

	Mat img_input;
	bool do_flip = false;

	vector<Vec2f> lines;

	while (1) {
		t1 = getTickCount();

		cam.grab();
		cam.retrieve(img_input);

		if (do_flip)
			flip(img_input, img_input, -1);

		if(digitalRead(PIR) == 1)
		{
			Pcount++;
			if (Pcount == 5){
				printf("Detection\n");
				tag = 2;
			}
		}
		else
		{
			Pcount=0;
		}

		CUTimage = img_input(Range(60,180), Range(80,240));
		resize(CUTimage, CUTimage, Size(0,0), 2, 2);

		cvtColor(CUTimage, img_hsv, COLOR_BGR2HSV);

		inRange(img_hsv, Scalar(LowH1, LowS, LowV), Scalar(HighH1, HighS, HighV), img_binary1);
		inRange(img_hsv, Scalar(LowH2, LowS, LowV), Scalar(HighH2, HighS, HighV), img_binary2);
		inRange(img_hsv, Scalar(LowH3, LowS, LowV), Scalar(HighH3, HighS, HighV), img_binary3);

		erode(img_binary1, img_binary1, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
		dilate(img_binary1, img_binary1, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));

		dilate(img_binary1, img_binary1, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
		erode(img_binary1, img_binary1, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));

		erode(img_binary2, img_binary2, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
		dilate(img_binary2, img_binary2, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));

		dilate(img_binary2, img_binary2, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
		erode(img_binary2, img_binary2, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));


		erode(img_binary3, img_binary3, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
		dilate(img_binary3, img_binary3, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));

		dilate(img_binary3, img_binary3, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
		erode(img_binary3, img_binary3, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));

		Mat img_labels, stats, centroids;
		int numOfLables = connectedComponentsWithStats(img_binary1, img_labels, stats, centroids, 8, CV_32S);

		int max1 = 500, idx1=0;
		for (int j = 1; j < numOfLables; j++){
			int area = stats.at<int>(j, CC_STAT_AREA);
			if(max1 < area)
			{
				max1 = area;
				idx1 = j;
			}
		}
		if (max1 > 500) {
			printf("Red signal - stop!!\t");
			putText(CUTimage,"Red Signal",Point(0,220), 2, 1, Scalar(0,0,255));
		}

		int left = stats.at<int>(idx1, CC_STAT_LEFT);
		int top = stats.at<int>(idx1, CC_STAT_TOP);
		int width = stats.at<int>(idx1, CC_STAT_WIDTH);
		int height = stats.at<int>(idx1, CC_STAT_HEIGHT);

		rectangle(CUTimage, Point(left,top), Point(left+width,top+height), Scalar(0,0,255),2);

		numOfLables = connectedComponentsWithStats(img_binary2, img_labels, stats, centroids, 8, CV_32S);

		int max2 = 500, idx2=0;
		for (int j = 1; j < numOfLables; j++){
			int area = stats.at<int>(j, CC_STAT_AREA);
			if(max2 < area)
			{
				max2 = area;
				idx2 = j;
			}
		}
		if (max2 > 500){
			printf("Yellow signal - wait!!\t");
			putText(CUTimage,"Yellow Signal",Point(0,220), 2, 1, Scalar(0,255,255));
		}

		left = stats.at<int>(idx2, CC_STAT_LEFT);
		top = stats.at<int>(idx2, CC_STAT_TOP);
		width = stats.at<int>(idx2, CC_STAT_WIDTH);
		height = stats.at<int>(idx2, CC_STAT_HEIGHT);

		rectangle(CUTimage, Point(left,top), Point(left+width,top+height), Scalar(0,255,255),2);

		numOfLables = connectedComponentsWithStats(img_binary3, img_labels, stats, centroids, 8, CV_32S);

		int max3 = 100, idx3=0;
		for (int j = 1; j < numOfLables; j++){
			int area = stats.at<int>(j, CC_STAT_AREA);
			if(max3 < area)
			{
				max3 = area;
				idx3 = j;
			}
		}
		if (max3 > 150 && max3 < 500){
			printf("green signal - go left");
			putText(CUTimage,"Green Signal - left",Point(0,220), 2, 1, Scalar(0,255,0));
		}
		else if (max3 >= 500){
			printf("green signal - go right");
			putText(CUTimage,"Green Signal - right",Point(0,220), 2, 1, Scalar(0,255,0));
		}

		left = stats.at<int>(idx3, CC_STAT_LEFT);
		top = stats.at<int>(idx3, CC_STAT_TOP);
		width = stats.at<int>(idx3, CC_STAT_WIDTH);
		height = stats.at<int>(idx3, CC_STAT_HEIGHT);



		rectangle(CUTimage, Point(left,top), Point(left+width,top+height), Scalar(0,255,0),2);

		imshow("red", img_binary1);
		imshow("yellow", img_binary2);
		imshow("green", img_binary3);
		imshow("input image", img_input);
		imshow("multi", CUTimage);


		t2 = getTickCount();
		cout << "It took " << (t2 - t1) * 1000 / getTickFrequency() << " ms." << endl;

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


void pwm(int milliseconds, int pin)
{
	float range = 20000;
	float range_signal = milliseconds;
	for (int i= 0; i<10;i++)
	{
		digitalWrite(pin, HIGH);
		delayMicroseconds(range_signal);
		digitalWrite(pin, LOW);
		delayMicroseconds(range-range_signal);
	}
}