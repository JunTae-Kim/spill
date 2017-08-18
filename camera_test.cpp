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
#define RANGE 100

using namespace cv;
using namespace std;

int main()
{
	int width = 320;
	int height = 240;

	raspicam::RaspiCam_Cv cam;

	cam.set(CV_CAP_PROP_FORMAT, CV_8UC3);
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 320);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

	if (wiringPiSetup() == -1)									// ���� X
	{
		return 1;
	}

	Mat image;

	bool do_flip = true;										// ���� ���� Bool

	while (1)
	{
		cam.grab();
		cam.retrieve(image);									// cam���󿡼� ���� ���������ӵ��� image��� Mat����ü�� �ҷ���
		imshow("image",image)
	} // While End 

	cam.release();
	destroyAllWindows();

	return 0;
}