
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
	int width = 640;
	int height = 480;

	raspicam::RaspiCam_Cv cam;

	cam.set(CV_CAP_PROP_FORMAT, CV_8UC3);
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

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
	Mat frame, edgeimg, to_hsv1, lower_red_hue_range, upper_red_hud_range, red_hue_image, hsv, image, ROIframe, REDframe;
	int64 t1, t2;
	bool do_flip = false;
	int i, j;
	int n = 3;
	Point pt1, pt2, pt3, pt4;
	float angle, angle2;

	vector<Vec2f> lines;
	vector<Mat> ROI_planes;

	while (1) {
		cam.grab();
		cam.retrieve(image);
		image.copyTo(ROIframe);

		if (do_flip)
			flip(image, image, -1);

		GaussianBlur(ROIframe, ROIframe, Size(3, 3), 0, 0);
		cvtColor(ROIframe, hsv, CV_BGR2HSV);
		cvtColor(ROIframe, to_hsv1, CV_BGR2HSV);

		inRange(hsv, Scalar(0, 10, 10), Scalar(15, 255, 255), red_hue_image);
		inRange(to_hsv1, Scalar(5, 70, 230), Scalar(70, 255, 255), ROIframe);

		int element_shape = MORPH_RECT;
		Mat element = getStructuringElement(element_shape, Size(n,n));
		dilate(red_hue_image, red_hue_image, element);

		HoughLines(ROIframe, lines, 1, CV_PI / 180, 150, 0, 0);
		
		Canny(image, edgeimg, 600, 750);

		t1 = getTickCount();

		for (size_t i = 0; i < lines.size(); i++) // °ËÃâµÈ Æ÷ÀÎÆ®žŠ Â÷Œ±Àž·Î ¿¬°á. 
		{
			float a1, a2;
			float rho = lines[i][0], theta = lines[i][1];
			float theta1, theta2, theta3;
			float rho1, rho2, rho3;
			int length = 800;

			if (theta<1.5 && theta>0)
			{
				theta1 = theta;
				rho1 = rho;
				double a = cos(theta1), b = sin(theta1);
				double x0 = a*rho1, y0 = b*rho1;

				pt1.x = cvRound(x0 - length * (-b));
				pt1.y = cvRound(y0 - length * (a));
				pt2.x = cvRound(x0 + length * (-b));
				pt2.y = cvRound(y0 + length * (a));

				angle = (atan2(pt1.y - pt2.y, pt1.x - pt2.x))*(180 / CV_PI);
				printf("floats: \n %f", angle); //printing angle 
			}

			else if (theta<3.14 && theta>2.0)
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
				//printf("floats2: \n %f", angle2); //printing angle °¢µµÈ®ÀÎÀ» À§ÇÑ ÇÁž°Æ®ÄÚµå 
			}
		} // Car Lane "for" End 

		if (waitKey(30) == 27) {
			cout << "esc key is pressed by user" << endl;
			break;
		}

		else if (pt1.x != 0 && pt3.x != 0) { //forward 
			line(image, pt1, pt2, Scalar(255, 0, 0), 2, CV_AA);
			line(image, pt3, pt4, Scalar(0, 0, 255), 2, CV_AA);
			//break; 
		}

		else if (pt1.x != 0 && pt3.x == 0) { //left 
			line(image, pt1, pt2, Scalar(255, 0, 0), 2, CV_AA);
			//break; 
		}

		else if (pt1.x == 0 && pt3.x != 0) { //right 
			line(image, pt3, pt4, Scalar(0, 0, 255), 2, CV_AA);
			//break; 
		}

		else {
			//break;
		}

		t2 = getTickCount();
		cout << "It took " << (t2 - t1) * 1000 / getTickFrequency() << " ms." << endl;

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

		t1 = getTickCount();


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

	cam.release();
	destroyAllWindows();

	return 0;
}
