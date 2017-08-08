
#include <opencv/cv.hpp>				// 헤더파일 불러오기
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

int main()
{
	IplImage * srcImage;

	uchar *data;
	int i, j;
	int height, width;
	
	srcImage = cvLoadImage("Line2.jpg", 0);

	height = srcImage->height;
	width = srcImage->width;
	data = (uchar *)srcImage->imageData;

	printf("Origianl fixel : height = %3d width = %3d \n", height, width);

	cvNamedWindow("Original pic", CV_WINDOW_AUTOSIZE);
	cvShowImage("Original pic", srcImage);

	// Fixel decrease Code //

	IplImage * decreasedImage = cvCreateImage(cvSize(300, 200), IPL_DEPTH_8U, 1); 

	cvResize(srcImage, decreasedImage, CV_INTER_LINEAR); // 입력영상 srcImage를 128X128 크기의 출력영상 dstImage128로 CV_INTER_LINEAR 보간하여 크기를 변경

	printf("Revised fixel : height = %3d width = %3d\n", height, width);
	
	cvNamedWindow("Revised pic", CV_WINDOW_AUTOSIZE);
	cvShowImage("Revised pic", decreasedImage);

	// Line inspection Code //

	for (i = 0;i < height;i++)
	{
		for (j = 0;j < width;j++)
		{
			if (data[i*width + j] > 60)
			{
				data[i*width + j] = 255;
			}
			
			else
			{
				data[i*width + j] = 0;
			}
		}
	}

	// Edge inspection Code //

	IplImage * edgeImage = cvCreateImage(cvGetSize(decreasedImage), IPL_DEPTH_8U, 1);
	
	cvCanny(decreasedImage, edgeImage, 600, 750, 3);

	height = edgeImage->height;
	width = edgeImage->width;
	data = (uchar *)edgeImage->imageData;

	int firstdot, lastdot, middledot;

	for (i = 0;i < height;i++)
	{
		for (j = 0;j < width;j++)
		{
			if (data[i*width + j] == 255)
			{
				firstdot = i*width + j ;
				break;
			}
		}

		for (j = width;j > 0;j--)
		{
			if (data[i*width + j] == 255)
			{
				lastdot = i*width + j;
				break;
			}
		}

		middledot = (firstdot + lastdot) / 2;
		data[middledot] = 255;
		printf("%8d %8d %8d\n", firstdot, lastdot, middledot);
	}
	
	cvNamedWindow("Edge pic", CV_WINDOW_AUTOSIZE);
	cvShowImage("Edge pic", edgeImage);

	cvWaitKey(0);
	cvDestroyAllWindows();

	cvReleaseImage(&srcImage);
	cvReleaseImage(&decreasedImage);
	cvReleaseImage(&edgeImage);

	return 0;
}
