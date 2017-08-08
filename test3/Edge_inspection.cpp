/*#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	Mat src = imread("siba.jpg", IMREAD_GRAYSCALE);

	if (src.empty())
	{
		cerr << "Image load failed!" << endl;
		return -1;
	}

	Mat dst;
	Canny(src, dst, 50, 150);

	imshow("src", src);
	imshow("dst", dst);

	waitKey(0);
	return 0;
}


#include <opencv2/highgui.hpp>
#include <opencv/cv.hpp>

int main(void)
{

	IplImage* src_Image = cvLoadImage("siba1.jpg", -1);
	IplImage* edge_Image = cvCreateImage(cvGetSize(src_Image), IPL_DEPTH_16S, 1);
	IplImage* dst_Image = cvCreateImage(cvGetSize(src_Image), IPL_DEPTH_8U, 1);

	cvNamedWindow("Original", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Edge", CV_WINDOW_AUTOSIZE);

	cvLaplace(src_Image, edge_Image, 3);
	cvConvertScale(edge_Image, dst_Image, 1, 0);

	cvShowImage("Original", src_Image);
	cvShowImage("Edge", edge_Image);

	cvWaitKey(0);
	cvReleaseImage(&src_Image);
	cvReleaseImage(&edge_Image);

}*/