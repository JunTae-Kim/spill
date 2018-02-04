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


#define PWM 1
#define DIR 22
#define ENABLE 23
#define pi 3.141592

using namespace cv;
using namespace std;

int main()
{

	int value = 0;

	if (wiringPiSetup() == -1) return 1; //wiringPi error

	int64 t1, t2;
	bool do_flip = false;
	int tag; int tag2 = 0;
	int n = 3;
	Point pt1, pt2, pt3, pt4;
	float theta1 = 0; float theta2 = 0;

	pinMode(PWM,PWM_OUTPUT);
	pinMode(DIR,OUTPUT);
	pinMode(ENABLE,OUTPUT);

	softPwmCreate(PWM,0,RANGE2);


	while (1) 
	{

		if (waitKey(30) == 27) 
		{
			cout << "esc key is pressed by user" << endl;
		}

		softPwmWrite(PWM, 35);
		delay(2000);
		softPwmWrite(PWM, 0);

		digitalWrite(DIR, HIGH);

		int k = waitKey(1);
		if (k == 27)
			break;
		else if (k == 'f' || k == 'F')
			do_flip = !do_flip;

	} // While End 

	digitalWrite(ENABLE, HIGH);

	return 0;
}
