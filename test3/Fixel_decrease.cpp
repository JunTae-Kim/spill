/*
#include <opencv/cv.hpp>				// 헤더파일 불러오기
#include <opencv2/highgui.hpp>


int main()
{
	IplImage * srcImage;

	uchar *data;
	int i, j;
	int height, width, size;

	srcImage = cvLoadImage("siba1.jpg", 0);

	height = srcImage->height;
	width = srcImage->width;
	data = (uchar *)srcImage->imageData;

	printf("기존픽셀 : height = %3d width = %3d \n", height, width );

	IplImage * amendedImage = cvCreateImage(cvSize(150, 150), IPL_DEPTH_8U, 1);

	cvResize(srcImage, amendedImage, CV_INTER_LINEAR); // 입력영상 srcImage를 128X128 크기의 출력영상 dstImage128로 CV_INTER_LINEAR 보간하여 크기를 변경

	height = amendedImage->height;
	width = amendedImage->width;
	data = (uchar *)amendedImage->imageData;

	printf("수정픽셀 : height = %3d width = %3d\n", height, width);

	cvNamedWindow("기존사진", CV_WINDOW_AUTOSIZE);
	cvShowImage("기존사진", srcImage);

	cvNamedWindow("수정사진", CV_WINDOW_AUTOSIZE);
	cvShowImage("수정사진", amendedImage);

	cvWaitKey(0);
	cvDestroyAllWindows();

	cvReleaseImage(&srcImage);
	cvReleaseImage(&amendedImage);

	return 0;
}
*/

