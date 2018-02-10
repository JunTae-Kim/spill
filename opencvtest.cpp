#include <opencv2/opencv.hpp>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <opencv/cv.hpp>
#include <opencv2/highgui.hpp>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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


	Mat image, edgeimg, to_hsv1, lower_red_hue_range, upper_red_hud_range, red_hue_image, hsv, ROIframe;
	int64 t1, t2;
	bool do_flip = false;
	int tag;
	int n = 3;
	Point pt1, pt2, pt3, pt4;
	float theta1, theta2;

	vector<Vec2f> lines;

	while (1)
	{
		t1 = getTickCount();

		cam.grab();
		cam.retrieve(image);
		image.copyTo(ROIframe);

		if (do_flip)
			flip(image, image, -1);

		cvtColor(image, image, CV_BGR2GRAY);
		GaussianBlur(image, image, Size(3, 3), 0, 0);
//		Canny(image, edgeimg, 50, 300);

//		cvtColor(ROIframe, hsv, CV_BGR2HSV);
//		cvtColor(ROIframe, to_hsv1, CV_BGR2HSV);

//		inRange(hsv, Scalar(0, 10, 10), Scalar(15, 255, 255), red_hue_image);
//		inRange(to_hsv1, Scalar(5, 70, 230), Scalar(70, 255, 255), ROIframe);

//		int element_shape = MORPH_RECT;
//		Mat element = getStructuringElement(element_shape, Size(n,n));

//		dilate(image, image, 100);

//		image.copyTo(red_hue_image);

		morphologyEx(image,red_hue_image,MORPH_CLOSE, n);

		Canny(red_hue_image, red_hue_image, 300, 350);

		HoughLines(red_hue_image, lines, 1, CV_PI / 180, 100, 0, 0);

		for (size_t i = 0; i < lines.size(); i++)
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

				tag = 0;
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
			}
		}


		if (waitKey(30) == 27) {
			cout << "esc key is pressed by user" << endl;
		}

		else if (pt1.x != 0 && pt3.x != 0) { //forward 
			line(image, pt1, pt2, Scalar(255, 0, 0), 2, CV_AA);
			line(image, pt3, pt4, Scalar(0, 0, 255), 2, CV_AA);
			tag = 1;
		}

		else if (pt1.x != 0 && pt3.x == 0) { //left 
			line(image, pt1, pt2, Scalar(255, 0, 0), 2, CV_AA);
			tag = 2;
		}

		else if (pt1.x == 0 && pt3.x != 0) { //right 
			line(image, pt3, pt4, Scalar(0, 0, 255), 2, CV_AA);
			tag = 3;
		}

		else {
		}


		t2 = getTickCount();
		cout << "It took " << (t2 - t1) * 1000 / getTickFrequency() << " ms." << endl;

		imshow("cam", image);
		imshow("cam3", red_hue_image);

		int k = waitKey(1);
		if (k == 27)
			break;
		else if (k == 'f' || k == 'F')
			do_flip = !do_flip;


	}
	destroyAllWindows();
	cam.release();
	return 0;
}