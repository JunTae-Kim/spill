#include <opencv2/opencv.hpp>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <opencv/cv.hpp>
#include <opencv2/highgui.hpp>
#include <math.h>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <wiringPi.h>
#include <softPwm.h>
#include <softServo.h>

/*
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
*/

#define LEFTLINE  0
#define RIGHTLINE 1
#define RHO   0
#define THETA 1

using namespace cv;
using namespace std;

const CvScalar COLOR_BLUE = CvScalar(255, 0, 0);
const CvScalar COLOR_RED = CvScalar(0, 0, 255);
const float DEGREE1 = CV_PI * 1 / 180;
const float DEGREE90 = CV_PI * 90 / 180;

bool getLeftLines(Mat* originImg, Mat* grayImg, Mat* cannyImg, float* resultLine, double theta)
{
	vector<Vec2f> linesL;

	// 왼쪽 차선 검출
	for (int i = 10; i > 0; i--)
	{
		int threshold = (grayImg->rows + grayImg->cols) * i / 10;
		HoughLines(*cannyImg, linesL, 1, CV_PI / 180, threshold, 0, 0, 0, theta - DEGREE1);
		if (linesL.size() > 0)
		{
			// Theta가 가장 작은 선 1개만 검출
			resultLine[RHO] = linesL[0][0];
			resultLine[THETA] = linesL[0][1];

			for (size_t i = 1; i < linesL.size(); i++)
			{
				if (linesL[i][1] < resultLine[THETA])
				{
					resultLine[RHO] = linesL[i][0];
					resultLine[THETA] = linesL[i][1];
				}
			}
			return true;
		}
	}

	return false;
}

void calLeftLanePoint(Mat* originImg, float* resultLine, Point* leftP)
{
	float rho = resultLine[RHO];
	float theta = resultLine[THETA];
	double a = cos(theta), b = sin(theta);
	double x0 = a * rho, y0 = b * rho;

	leftP[0].x = cvRound(x0 + (originImg->rows + originImg->cols) * (-b));
	leftP[0].y = cvRound(y0 + (originImg->rows + originImg->cols) * (a)) + originImg->rows / 2;

	leftP[1].x = cvRound(x0 - (originImg->rows + originImg->cols) * (-b));
	leftP[1].y = cvRound(y0 - (originImg->rows + originImg->cols) * (a)) + originImg->rows / 2;
}

bool getRightLines(Mat* originImg, Mat* grayImg, Mat* cannyImg, float* resultLine, double theta)
{
	vector<Vec2f> linesR;

	// 오른쪽 차선 검출
	for (int i = 10; i > 0; i--)
	{
		int threshold = (grayImg->rows + grayImg->cols) * i / 10;
		HoughLines(*cannyImg, linesR, 1, CV_PI / 180, threshold, 0, 0, theta + DEGREE1, CV_PI);
		if (linesR.size() > 0)
		{
			// Theta가 가장 큰 선 1개만 검출
			resultLine[RHO] = linesR[0][0];
			resultLine[THETA] = linesR[0][1];

			for (size_t i = 1; i < linesR.size(); i++)
			{
				if (linesR[i][1] > resultLine[THETA])
				{
					resultLine[RHO] = linesR[i][0];
					resultLine[THETA] = linesR[i][1];
				}
			}
			return true;
		}
	}

	return false;
}

void calRightLanePoint(Mat* originImg, float* resultLine, Point* rightP)
{
	float rho = resultLine[RHO];
	float theta = resultLine[THETA];
	double a = cos(theta), b = sin(theta);
	double x0 = a * rho, y0 = b * rho;

	rightP[0].x = cvRound(x0 + (originImg->rows + originImg->cols) * (-b));
	rightP[0].y = cvRound(y0 + (originImg->rows + originImg->cols) * (a)) + originImg->rows / 2;

	rightP[1].x = cvRound(x0 - (originImg->rows + originImg->cols) * (-b));
	rightP[1].y = cvRound(y0 - (originImg->rows + originImg->cols) * (a)) + originImg->rows / 2;
}

int main()
{
	raspicam::RaspiCam_Cv cam;

	cam.set(CV_CAP_PROP_FORMAT, CV_8UC3);
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 320);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

	if (!cam.open()) 
	{
		cerr << "Camera open failed!" << endl;
		return -1;
	}

	if (wiringPiSetup() == -1) return 1; //wiringPi error
	bool do_flip = false;

	cvNamedWindow("Original", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Canny", CV_WINDOW_AUTOSIZE);

	Mat originImg;
	Mat grayImg;
	Mat cannyImg;
	Mat imageROI;
	float resultLine[2][2];
	Point leftP[2], rightP[2], banishP;

	while (1) 
	{
		cam.grab();
		cam.retrieve(originImg);

		cvtColor(originImg, grayImg, CV_BGR2GRAY);
		GaussianBlur(grayImg, grayImg, Size(3, 3), 0, 0);

		Rect rect(0, 100, 320, 120);
		imageROI = grayImg(rect);
		Canny(imageROI, cannyImg, 320, 350);

		getLeftLines(&originImg, &grayImg, &cannyImg, resultLine[LEFTLINE], CV_PI / 2);
		getRightLines(&originImg, &grayImg, &cannyImg, resultLine[RIGHTLINE], CV_PI / 2);

		/*
		// 좌우 차선이 이루는 각이 90도 보다 큰 경우 각도가 완만한 차선을 버리고 다시 검출
		while (abs(resultLine[RIGHTLINE][THETA] - resultLine[LEFTLINE][THETA]) < DEGREE90)
		{
			if (CV_PI / 2 - resultLine[LEFTLINE][THETA] <= resultLine[RIGHTLINE][THETA] - CV_PI / 2)
			{
				// 좌측 차선 다시 검출
				getLeftLines(&originImg, &grayImg, &cannyImg, resultLine[LEFTLINE], resultLine[LEFTLINE][THETA]);
			}
			else
			{
				// 우측 차선 다시 검출
				getRightLines(&originImg, &grayImg, &cannyImg, resultLine[RIGHTLINE], resultLine[RIGHTLINE][THETA]);
			}
		}
		*/

		calLeftLanePoint(&originImg, resultLine[LEFTLINE], leftP);
		calRightLanePoint(&originImg, resultLine[RIGHTLINE], rightP);

		/* 소실점 검출 */
		// 왼쪽 차선의 1차 방정식
		float leftLineA = (float)(leftP[1].y - leftP[0].y) / (float)(leftP[1].x - leftP[0].x);	//기울기
		float leftLineB = leftP[1].y - leftLineA * leftP[1].x;									//

		// 오른쪽 차선의 1차 방정식
		float rightLineA = (float)(rightP[1].y - rightP[0].y) / (float)(rightP[1].x - rightP[0].x);
		float rightLineB = rightP[1].y - rightLineA * rightP[1].x;

		// 1차 소실점: 좌우 1차선의 교점
		banishP.x = (int)((rightLineB - leftLineB) / (leftLineA - rightLineA));
		banishP.y = (int)(leftLineA * banishP.x + leftLineB);

		line(originImg, leftP[0], banishP, COLOR_RED, 2);
		line(originImg, rightP[1], banishP, COLOR_BLUE, 2);

		imshow("Original", originImg);
		imshow("Canny", cannyImg);

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
