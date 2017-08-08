#include <opencv2/opencv.hpp>
#include <iostream>
#include <raspicam/raspicam_cv.h>

using namespace cv;
using namespace std;

int main(int argc, char* argv[]) {
	raspicam::RaspiCam_Cv cam;

	cam.set(CV_CAP_PROP_FORMAT, CV_8UC3);
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

	Mat frame, gray, reduced;
	int64 t1, t2;
	bool do_flip = false;

	while (1) {
		cam.grab();
		cam.retrieve(frame);

		if (do_flip)
			flip(frame, frame, -1);

		t1 = getTickCount();

		cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
		resize(gray, reduced, Size(0, 0), 0.5, 0.5);

		vector<Rect> faces;
		cascade.detectMultiScale(reduced, faces, 1.1, 3, 0, Size(40, 40));

		for (size_t i = 0; i < faces.size(); i++) {
			Rect rc;
			rc.x = faces[i].x << 1;
			rc.y = faces[i].y << 1;
			rc.width = faces[i].width << 1;
			rc.height = faces[i].height << 1;

			rectangle(frame, rc, Scalar(0, 0, 255), 3);
		}

		t2 = getTickCount();
		cout << "It took " << (t2 - t1) * 1000 / getTickFrequency() << " ms." << endl;

		imshow("frame", frame);

		int k = waitKey(1);
		if (k == 27)
			break;
		else if (k == 'f' || k == 'F')
			do_flip = !do_flip;
	}

	cam.release();
	destroyAllWindows();
}

