#include <math.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <cmath> 
#include <iostream> 
#include <ctype.h> 
#include <opencv/cv.h> 
#include <opencv/highgui.h> 
#include <raspicam/raspicam_cv.h> 
#include "opencv2/opencv.hpp" 
//#include <unistd.h> 
#include <fcntl.h> 
//#include <termios.h> 
#include <sstream> 
#include <string> 
#include "opencv2/imgproc.hpp" 


using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
	Mat frame; //실제 카메라 사용시에는 이 부분만 바꿔주면됨. 
	raspicam::RaspiCam_Cv capture; //카메라 사용시 주석해제. 

	int width = 320;
	int height = 240;
	int fps = 15;
	Size framesize(width, height);
	// VideoWriter oVideoWriter("/home/pi/Desktop/testlinse.mp4", CV_FOURCC('M', 'J', 'P', 'G'), fps, framesize, true); //촬영되는 동영상을 저장해줌. 
	//set camera params 

	capture.set(CV_CAP_PROP_FORMAT, CV_8UC3);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, width);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, height);
	//Open camera 

	cout << "Opening Camera..." << endl;
	if (!capture.isOpened()) { cerr << "Error opening the camera" << endl; return -1; }

	while (1) {
		int input;
		double t = (double)cv::getTickCount(); //fps 표시해줌 
		Mat frame, to_hsv1, lower_red_hue_range, upper_red_hue_range, red_hue_image, hsv, image, ROIframe, REDframe;
		vector<Mat> ROI_planes;

		capture.grab();
		capture.retrieve(image); // 캡쳐된 영상을 이미지로 ROIframe에 넣어줌. 
		image.copyTo(ROIframe);

			if (image.empty()) break; //image 가 없다면 break 
		GaussianBlur(ROIframe, ROIframe, Size(3, 3), 0, 0); //가우시안블러 
		cvtColor(ROIframe, hsv, CV_BGR2HSV); // Obstacle // RGB 에서 HSV로 색변환. 
		cvtColor(ROIframe, to_hsv1, CV_BGR2HSV); // Lane 

		inRange(hsv, Scalar(0, 10, 10), Scalar(15, 255, 255), red_hue_image);
		inRange(to_hsv1, Scalar(5, 70, 230), Scalar(70, 255, 255), ROIframe); // => fisrt matching 

		int n = 3;
		int quit = 0;
		int element_shape = MORPH_RECT;
		Mat element = getStructuringElement(element_shape, Size(n, n));
		dilate(red_hue_image, red_hue_image, element);

		Point pt1, pt2, pt3, pt4;
		float angle, angle2;
		vector<Vec2f> lines;
		HoughLines(ROIframe, lines, 1, CV_PI / 180, 150, 0, 0);

		for (size_t i = 0; i < lines.size(); i++) // 검출된 포인트를 차선으로 연결. 
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
				//printf("floats2: \n %f", angle2); //printing angle 각도확인을 위한 프린트코드 
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
			break;
		}

		imshow("Camera", image);
		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		std::cout << "fps:" << 1 / t << std::endl; // fps표시 
		waitKey(1);

	} // While End 

	return 0;

} // int main End 

