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
//#define ENABLE 23
#define RANGE 100
#define pi 3.141592

using namespace cv;
using namespace std;

int main()
{
	int stri;
	if (wiringPiSetup() == -1) 
	{
		return 1; //wiringPi error
	}

	pinMode(PWM,PWM_OUTPUT);
	pinMode(DIR,OUTPUT);
//	pinMode(ENABLE,OUTPUT);

	softPwmCreate(PWM,0,RANGE);
	digitalWrite(DIR, HIGH);

	while (1) 
	{

		if (waitKey(30) == 27) 
		{
			cout << "esc key is pressed by user" << endl;
		}

		fputs("input int  : ", stdout);
		scanf("%d", &stri);

		if (stri != 0) {
		softPwmWrite(PWM, stri);
		delay (2000);
		softPwmWrite(PWM, 0);
		}

		else if (stri == 0) return 0;


		int k = waitKey(1);
		if (k == 27)
			break;

	} // While End 

	return 0;
}
