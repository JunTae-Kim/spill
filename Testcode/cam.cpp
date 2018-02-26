#include <opencv2/opencv.hpp>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <opencv/cv.hpp>
#include <opencv2/highgui.hpp>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define pi 3.141592

using namespace cv;
using namespace std;

int main()
{
	int width = 320;
	int height = 240;
	int ROI_widthL = floor(width*3/8);
	int ROI_widthR = floor(width*5/8);
	int ROI_heightH = floor(height/3);
	int ROI_heightL = floor(height*5/6);
	int count = 0;

	int value = 0;
	int b_value = 0;
	int tag;
	int64 t1, t2;
	float theta1 = 0, theta2 = 0;
	float thetaL, thetaR;
	Point pt1, pt2;		//left line  : up_point, down_point
	Point pt4, pt3;		//right line : up_point, down_point
	Point leftP1, leftP2;		//left point
	Point rightP1, rightP2;		//right point
	Point banishP;		//banish point;

	FILE *fp = fopen("/home/pi/spill/Testcode/data/value-3.txt", "w");
//	FILE *fp1 = fopen("/home/pi/spill/Testcode/data/theta1.txt", "w");
//	FILE *fp2 = fopen("/home/pi/spill/Testcode/data/theta2.txt", "w");

	Size framesize(width, height);

	raspicam::RaspiCam_Cv cam;

	cam.set(CV_CAP_PROP_FORMAT, CV_8UC3);
	cam.set(CV_CAP_PROP_FRAME_WIDTH, width);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, height);

	if (!cam.open()) {
		cerr << "Camera open failed!" << endl;
		return -1;
	}	

	Mat image, grayimg, edgeimg, blurimg, closeimg, dilimg, erimg;
	Mat ROIimg(height, width, CV_8UC1, Scalar(0));

	/* ROI image */
	for (int y=0; y<height; y++){
		for (int x=0; x<width; x++){
			if (y >= ROI_heightH && y <= ROI_heightL){
				if (x >= (ROI_widthL - (y - ROI_heightH/2+20)) && x <= (ROI_widthR + (y - ROI_heightH/2+20))){
					ROIimg.at<uchar>(y,x) = 255;
				}
			}
		}
	}
	/* ROI image end */

	bool do_flip = false;

	vector<Vec2f> lines;
	
	while (1) {
		t1 = getTickCount();

		cam.grab();
		cam.retrieve(image);

		if (do_flip)
			flip(image, image, -1);

		cvtColor(image, grayimg, CV_BGR2GRAY);
		GaussianBlur(grayimg, blurimg, Size(3, 3), 0, 0);

		Canny(blurimg, edgeimg, 150, 300);

		int element_shape = MORPH_RECT;
		Mat element1 = getStructuringElement(element_shape, Size(5,5));
		Mat element2 = getStructuringElement(element_shape, Size(7,7));


		//dilate(edgeimg, dilimg, element1);
		//dilate(dilimg, dilimg, element1);
		//morphologyEx(dilimg, closeimg, MORPH_CLOSE, element1);
		//erode(closeimg, erimg, element2);
		//erode(erimg, erimg, element2); 

		morphologyEx(edgeimg, closeimg, MORPH_CLOSE, element2);
		erode(closeimg, erimg, element1);

		for (int y=0; y<height; y++){
			for (int x=0; x<width; x++){
				erimg.at<uchar>(y,x) = ROIimg.at<uchar>(y,x) & erimg.at<uchar>(y,x);
			}
		}

		/* Houghline detection */
		HoughLines(erimg, lines, 1, CV_PI / 180, 70, 0, 0);

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
				leftP1.x=0;
				leftP1.y=0;
				leftP2.x=500;
				leftP2.y=0;
				rightP1.x=500;
				rightP1.y=0;
				rightP2.x=0;
				rightP2.y=0;
			}

			if (theta<1.41 && theta>=0)
			{
				theta1 = theta;
				rho1 = rho;
				double a1 = cos(theta1), b1 = sin(theta1);
				double x0 = a1*rho1, y0 = b1*rho1;

				pt1.x = cvRound(x0 - length * (-b1));
				pt1.y = cvRound(y0 - length * (a1));
				pt2.x = cvRound(x0 + length * (-b1));
				pt2.y = cvRound(y0 + length * (a1));

				tag = 0;
			}

			else if (theta<3.14 && theta>=1.77)
			{
				theta2 = theta;
				rho2 = rho;
				double a2 = cos(theta2), b2 = sin(theta2);
				double x02 = a2*rho2, y02 = b2*rho2;

				pt3.x = cvRound(x02 - length * (-b2));
				pt3.y = cvRound(y02 - length * (a2));
				pt4.x = cvRound(x02 + length * (-b2));
				pt4.y = cvRound(y02 + length * (a2));

				tag = 0;
			}
		}
		/* Houghline detection end */

		printf("theta1 : %0.3f, theta2 : %0.3f\n",theta1, theta2);

		//fprintf(fp1,"%0.3f\n", theta1);
		//fprintf(fp2,"%0.3f\n", theta2);

		/* Servo controll */
		if (waitKey(30) == 27) {
			cout << "esc key is pressed by user" << endl;
		}

		// forward 
		else if (pt1.x != 0 && pt3.x != 0) {

			// banish Point detection 

			// leftLine : first linear equation
			float gradientL = (float)(pt2.y - pt1.y) / (float)(pt2.x - pt1.x);		// gradient 
			float interceptL = pt2.y - gradientL * pt2.x;							// y-intercept

			// rightLine : first linear equation
			float gradientR = (float)(pt4.y - pt3.y) / (float)(pt4.x - pt3.x);		// gradient
			float interceptR = pt4.y - gradientR * pt4.x;							// y-intercept

			// banishPoint : nodePoint of two equation
			banishP.x = (int)((interceptR - interceptL) / (gradientL - gradientR));
			banishP.y = (int)(gradientL * banishP.x + interceptL);
//			line(image, pt2, banishP, Scalar(255, 0, 0), 2, CV_AA);
//			line(image, pt3, banishP, Scalar(0, 0, 255), 2, CV_AA);


			line(image, pt2, pt1, Scalar(255, 0, 0), 2, CV_AA);
			line(image, pt3, pt4, Scalar(0, 0, 255), 2, CV_AA);

			float b1 = pt2.y - gradientL * pt2.x;
			leftP2.y = height;
			leftP2.x = (leftP2.y - b1) / gradientL;

			float b2 = pt3.y - gradientR * pt3.x;
			rightP2.y = height;
			rightP2.x = (rightP2.y - b2) / gradientR;

			printf("***********Both Line Detect***********\n");
			printf("banishP.x : %d,\t banishP.y : %d\n", banishP.x, banishP.y);
			printf("leftP2.x : %d,\t rightP2.x : %d\n", leftP2.x, rightP2.x);

			//fprintf(fp,"%d\t%d\n", leftP2.x, rightP2.x);
			tag = 1;
		}

		// left 
		else if (pt1.x != 0 && pt3.x == 0) { 

			// left Point detection 

			// leftLine : first linear equation
			float gradientL = (float)(pt2.y - pt1.y) / (float)(pt2.x - pt1.x);		// gradient
			float interceptL = pt2.y - gradientL * pt2.x;					// y-intercept

			// leftPoint : nodePoint of two equation
			leftP1.x = (int)(interceptL / -gradientL);
			leftP1.y = (int)(gradientL * leftP1.x + interceptL);

			float b1 = leftP1.y - gradientL * leftP1.x;
			leftP2.y = height;
			leftP2.x = (leftP2.y - b1) / gradientL;

			line(image, pt2, leftP1, Scalar(255, 0, 0), 2, CV_AA);

			printf("***********Left Line Detect***********\n");
			printf("leftP1.x : %d,\t leftP1.y : %d\n", leftP1.x, leftP1.y);
			printf("leftP2.x : %d,\t leftP2.y : %d\n", leftP2.x, leftP2.y);

			//fprintf(fp,"%d\t%d\n", leftP2.x, leftP2.y);
			//fprintf(fp,"leftP2.x : %d,\t leftP2.y : %d\n", leftP2.x, leftP2.y);
			tag = 2;
		}

		// right 
		else if (pt1.x == 0 && pt3.x != 0) { 

			// right Point detection 

			// rightLine : first linear equation
			float gradientR = (float)(pt4.y - pt3.y) / (float)(pt4.x - pt3.x);		// gradient
			float interceptR = pt4.y - gradientR * pt4.x;							// y-intercept

			// rightPoint : nodePoint of two equation
			rightP1.x = (int)(interceptR / -gradientR);
			rightP1.y = (int)(gradientR * rightP1.x + interceptR);

			float b2 = rightP1.y - gradientR * rightP1.x;
			rightP2.y = height;
			rightP2.x = (rightP2.y - b2) / gradientR;

			line(image, pt3, rightP1, Scalar(0, 0, 255), 2, CV_AA);

			printf("***********Right Line Detect***********\n");
			printf("rightP1.x : %d,\t rightP1.y : %d\n", rightP1.x, rightP1.y);
			printf("rightP2.x : %d,\t rightP2.y : %d\n", rightP2.x, rightP2.y);

			fprintf(fp,"%d\t%d\n", rightP2.x, rightP2.y);
			//fprintf(fp,"rightP2.x : %d,\t rightP2.y : %d\n", rightP2.x, rightP2.y);
			tag = 3;
		}

		
		// value normalization
		if (leftP2.x <= -26 && rightP2.x >= 317) {
			value = 0;
		}
		else if (leftP1.x >= 200 && leftP1.x <= 300) {
			value = 1;
		}
		else if (leftP1.x > 300 && leftP1.x <= 400) {
			value = 2;
		}
		else if (leftP1.x > 400) {
			value = 3;
		}
		else if (rightP1.x < 80 && rightP1.x >= 0) {
			value = -1;
		}
		else if (rightP1.x < 0 && rightP1.x >= -80) {
			value = -2;
		}
		else if (rightP1.x < -80) {
			value = -3;
		}

		else if (leftP2.x > -70 && leftP2.x <= 6) {
			value = 1;
		}
		else if (leftP2.x > 6 && leftP2.x < 34) {
			value = 2;
		}
		else if (leftP2.x > 34 && leftP2.x <= 200) {
			value = 3;
		}
		else if (rightP2.x > 120 && rightP2.x <= 257) {
			value = -3;
		}
		else if (rightP2.x > 257 && rightP2.x <= 280) {
			value = -2;
		}
		else if (rightP2.x > 280 && rightP2.x < 350) {
			value = -1;
		}

		if (b_value !=  value || count <= 1) {

		}
		if (b_value == value){
			count++;
		}
		else	count = 0;

		printf("\n-----value : %d-----\n\n",value);

		b_value = value;
		/* Servo controll end */

		imshow("image", image);
		imshow("edgeimg",edgeimg);
		imshow("closeimg",closeimg);
		imshow("ROIimg",ROIimg);
		imshow("erimg", erimg);

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
