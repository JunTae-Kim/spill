
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

	if (wiringPiSetup() == -1)									// 이해 X
	{
		return 1;
	}

	Mat image, ROIframe, edgeimg;
	Mat hsv, to_hsv1, red_hue_image;

	bool do_flip = true;										// 논리형 변수 Bool

	vector<Vec2f> lines;
	vector<Mat> ROI_planes;

	softPwmCreate(SERVO, 0, RANGE);

	while (1)
	{
		cam.grab();
		cam.retrieve(image);									// cam영상에서 찍은 영상프레임들을 image라는 Mat구조체로 불러옴.
		image.copyTo(ROIframe);									// image에 저장시킨 사진을 ROIframe으로 복사함. 참고) clone() == copyTo()

		if (do_flip)											// 
		{
			flip(image, image, -1);								// 영상반전함수 flip
		}

		GaussianBlur(ROIframe, ROIframe, Size(3, 3), 0, 0);		// 사진의 가우시안 필터를 거침. GaussianBlur(본사진,수정사진,사이즈,?,?)
		Canny(image, edgeimg, 550, 600);						// 사진의 엣지를 검출함. Canny(본사진,수정사진,최소,최대)
																
		cvtColor(ROIframe, hsv, CV_BGR2HSV);					// 사진의 컬러모형을 변환함. cvtColor(본사진,수정사진,바꾸고자하는 컬러모형)
		cvtColor(ROIframe, to_hsv1, CV_BGR2HSV);				// Hsv컬러모형을 선택함. H=색상, S=채도, V=명도

		inRange(hsv, Scalar(0, 10, 10), Scalar(15, 255, 255), red_hue_image);		//임계값을 갖고 hsv사진을 나눈다
		inRange(to_hsv1, Scalar(5, 70, 230), Scalar(70, 255, 255), ROIframe);

		Mat element = getStructuringElement(MORPH_RECT, Size(3,3));					//
		dilate(red_hue_image, red_hue_image, element);
																
		HoughLines(edgeimg, lines, 1, CV_PI / 180, 100, 0, 0);						//허프변환

		float angle, angle2;														// 각도?
		int i, j, tag;
		int64 t1, t2;
		t1 = getTickCount();

		Point pt1, pt2, pt3, pt4;													// 특정픽셀점의 좌표 ptn(x,y) 	

		for (size_t i = 0; i < lines.size(); i++)									//  
		{
			float a1, a2;
			float rho = lines[i][0], theta = lines[i][1];
			float theta1, theta2, theta3;
			float rho1, rho2, rho3;

			int length = 800;

			if (tag == 1)
			{
				pt1.x = 0;
				pt1.y = 0;
				pt2.x = 0;
				pt2.y = 0;
				pt3.x = 0;
				pt3.y = 0;
				pt4.x = 0;
				pt4.y = 0;
			}

			if (theta<1.5 && theta >= 0)
			{
				theta1 = theta;
				rho1 = rho;
				double a = cos(theta1), b = sin(theta1);
				double x0 = a*rho1, y0 = b*rho1;

				pt1.x = cvRound(x0 - length * (-.b));
				pt1.y = cvRound(y0 - length * (a));
				pt2.x = cvRound(x0 + length * (-b));
				pt2.y = cvRound(y0 + length * (a));

				angle = (atan2(pt1.y - pt2.y, pt1.x - pt2.x))*(180 / CV_PI);
				printf("floats : %f\n", angle); //printing angle 

				tag = 0;
			}

			else if (theta<3.14 && theta >= 2.0)
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
				printf("floats2 : %f\n", angle2); //printing angle 

				tag = 0;
			}
		}


		if (waitKey(30) == 27)
		{
			cout << "esc key is pressed by user" << endl;
		}

		else if (pt1.x != 0 && pt3.x != 0)
		{
			line(image, pt1, pt2, Scalar(255, 0, 0), 2, CV_AA);
			line(image, pt3, pt4, Scalar(0, 0, 255), 2, CV_AA);

			printf("forward\n");
			softPwmWrite(SERVO, 15);
			delay(300);
			tag = 1;

		}

		else if (pt1.x != 0 && pt3.x == 0)
		{
			line(image, pt1, pt2, Scalar(255, 0, 0), 2, CV_AA);

			printf("right\n");

			delay(300);
			tag = 1;
		}

		else if (pt1.x == 0 && pt3.x != 0)
		{
			line(image, pt3, pt4, Scalar(0, 0, 255), 2, CV_AA);

			printf("left\n");
			delay(300);
			tag = 1;
		}

		t2 = getTickCount();						//		cout << "It took " << (t2 - t1) * 1000 / getTickFrequency() << " ms." << endl;

		imshow("Camera1", image);
		imshow("Camera2", edgeimg);

		int k = waitKey(1);
		if (k == 27)
			break;

		else if (k == 'f' || k == 'F')
			do_flip = !do_flip;

	} // While End 

	cam.release();
	destroyAllWindows();

	return 0;
}
