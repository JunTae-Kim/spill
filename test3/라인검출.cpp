/*#include <opencv2/highgui.hpp>
#include <opencv/cv.hpp>
using namespace cv;

int main()
{
	IplImage* Image;
	
	int height, width;
	uchar *data;
	int i, j;

	Image = cvLoadImage("siba1.jpg", 0);

	height = Image->height;
	width = Image->width;
	data = (uchar *)Image->imageData;

	printf("%8d" "%8d", height, width);

	for (i = 0;i < height / 2;i++)
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

	/*
	for (i = 0;i < height;i++)
	{
	for (j = 0;j < width;j++)
	{
	printf("%8d", data[i*width + j]);
	}
	}
	

	cvNamedWindow("siba1", CV_WINDOW_AUTOSIZE);
	cvShowImage("siba1", image);

	cvWaitKey(0);
	cvReleaseImage(&image);
	

	return 0;
}

*/

