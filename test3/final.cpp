/*
#include <opencv/cv.hpp>				// ������� �ҷ�����
#include <opencv2/highgui.hpp>


int main()
{
	IplImage* srcImage;

	uchar* data;
	int i, j;
	int height, width, size;

	srcImage = cvLoadImage("siba.JPG", 0);

	height = srcImage->height;
	width = srcImage->width;
	data = (uchar *)srcImage->imageData;

	printf("before : height = %3d width = %3d \n", height, width);

	IplImage * amendedImage = cvCreateImage(cvSize(300, 200), IPL_DEPTH_8U, 1);

	cvResize(srcImage, amendedImage, CV_INTER_LINEAR); // �Է¿��� srcImage�� 128X128 ũ���� ��¿��� dstImage128�� CV_INTER_LINEAR �����Ͽ� ũ�⸦ ����

	height = amendedImage->height;
	width = amendedImage->width;
	data = (uchar *)amendedImage->imageData;

	printf("before : height = %3d width = %3d\n", height, width);
	scanf("%d", &size);

	cvNamedWindow("before", CV_WINDOW_AUTOSIZE);
	cvShowImage("before", srcImage);

	for (i = 0;i < height;i++)
	{
		for (j = 0;j < width;j++)
		{
			if (data[i*width + j] > 110)
			{
				data[i*width + j] = 255;
			}

			else
			{
				data[i*width + j] = 0;
			}
		}
	}

	cvNamedWindow("after", CV_WINDOW_AUTOSIZE);
	cvShowImage("after", amendedImage);

	cvWaitKey(0);
	cvDestroyAllWindows();

	cvReleaseImage(&srcImage);
	cvReleaseImage(&amendedImage);

	return 0;
}
*/