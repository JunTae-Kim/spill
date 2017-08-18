#include <opencv2/opencv.hpp>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <opencv/cv.hpp>
#include <opencv2/highgui.hpp>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>


using namespace cv;
using namespace std;

int main()
{

	raspicam::RaspiCam_Cv cam;

	cam.set(CV_CAP_PROP_FORMAT, CV_8UC3);
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

	Mat image;
	bool do_flip;

	if (!cam.open()) 
	{
		cerr << "Camera open failed!" << endl;
		return -1;
	}

	cout << "Opening Camera..." << endl;
	if (!cam.isOpened()) { cerr << "Error opening the camera" << endl; return -1; }



	while (1)
	{
		cam.grab();

		cam.retrieve(image);									// cam영상에서 찍은 영상프레임들을 image라는 Mat구조체로 불러옴
		imshow("image",image);
	
		if (do_flip)
		{
			flip(image, image, -1);
		}

		imshow("image", image);

		int k = waitKey(1);
		if (k == 27)
		{
			break;
		}
		else if (k == 'f' || k == 'F')
		{
			do_flip = !do_flip;
		}

	}  

	cam.release();
	destroyAllWindows();

}