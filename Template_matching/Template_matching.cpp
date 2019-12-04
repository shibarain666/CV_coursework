#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

int main(int argc, char const *argv[]) {
	Mat sample = imread("letter_e.jpg", CV_LOAD_IMAGE_COLOR);    //For any case, color, gray scale...
	Mat test = imread("test_l1.jpg", CV_LOAD_IMAGE_COLOR);
	Mat sample_gray, test_gray;
	Mat sample_bin, test_bin;
	int correlation = 0;
	int x = 0, y = 0;
	int x1 = 0, y1 = 0;

	double t = getCPUTickCount();

	cvtColor(test, test_gray, CV_BGR2GRAY);    //convert to grayscale
	cvtColor(sample, sample_gray, CV_BGR2GRAY);
	threshold(sample_gray, sample_bin, 150, 255, THRESH_BINARY);    //convert to binary
	threshold(test_gray, test_bin, 150, 255, THRESH_BINARY);

	int h = sample_bin.rows;
	int w = sample_bin.cols;

	for (y = 0; y < test_bin.rows - sample_bin.rows; y++) {
		for (x = 0; x < test_bin.cols - sample_bin.cols; x++) {
			correlation = 0;    //clear correlation 
			for (y1 = 0; y1 < sample_bin.rows; y1++) {
				uchar* ptr_test = test_bin.ptr<uchar>(y+y1);
				uchar* ptr_sample = sample_bin.ptr<uchar>(y1);
				for (x1 = 0; x1 < sample_bin.cols; x1++) {
					if (ptr_test[x + x1] == ptr_sample[x1]) {    //compare
						correlation++;
					}
				}
			}
			if (correlation > 2945) {    //threshold for matching, tuned
				rectangle(test, Point(x, y), Point(x + w, y + h), Scalar(0, 0, 255), 1);    //(B, G, R)
			}
		}
	}
	t = (getCPUTickCount() - t) / getTickFrequency(); 

	cout << t << "s" << endl;
	imshow("Result", test);
	waitKey(0);
	return 0;
}