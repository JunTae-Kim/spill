#include <opencv2/opencv.hpp>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <opencv/cv.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

int main()
{
	int width = 640;
	int height = 480;

	raspicam::RaspiCam_Cv cam;

	cam.set(CV_CAP_PROP_FORMAT, CV_8UC1);
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

	if (!cam.open()) {
		cerr << "Camera open failed!" << endl;
		return -1;
	}

	CascadeClassifier cascade("haarcascade_frontalface_default.xml");

	if (cascade.empty()) {
		cerr << "Failed to open xml file!" << endl;
		return -1;
	}

	Mat img, edgeimg;
	int64 t1, t2;
	bool do_flip = false;

	while (1) {
		cam.grab();
		cam.retrieve(img);

		if (do_flip)
			flip(img, img, -1);


		unsigned char* data = (unsigned char*)img.data;


		t1 = getTickCount();

		for (int i = 0;i < height;i++)
		{
			for (int j = 0;j < width;j++)
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

		Canny(img, edgeimg, 50, 100);

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

	cam.release();
	destroyAllWindows();

	return 0;
}
