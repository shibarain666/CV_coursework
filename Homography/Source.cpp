#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat H_matrix(Point2d src[4], Point2d dst[4]) {
	double Point_matrix[8][8];
	for (int i = 0; i < 4; i++) {
		double row1[8] = { src[i].x, src[i].y, 1,        0,        0, 0, -dst[i].x * src[i].x, -dst[i].x * src[i].y };
		double row2[8] = { 0       ,        0, 0, src[i].x, src[i].y, 1, -dst[i].y * src[i].x, -dst[i].y * src[i].y };

		for (int j = 0; j < 8; j++) { 
			Point_matrix[2 * i][j] = row1[j];
			Point_matrix[2 * i + 1][j] = row2[j];
		}
	}
	Mat Point_data(8, 8, CV_64FC1, Point_matrix); //8*8 matrix

	double _dst[8];
	for (int i = 0; i < 4; i++) {
		_dst[2 * i] = dst[i].x;
		_dst[2 * i + 1] = dst[i].y;
	}
	Mat Point_target(8, 1, CV_64FC1, _dst);

	Mat h8 = Point_data.inv() * Point_target;

	double H8[9]; 
	for (int i = 0; i < 8; i++) {
		H8[i] = h8.at<double>(i, 0);
	}
	H8[8] = 1;     //(3, 3) = 1
	Mat H(3, 3, CV_64FC1, H8);

	return H.clone();
}

Mat Homography(Mat src, Mat H) {
	Mat img(768, 1024, CV_8UC3, Scalar(0, 0, 0));    //size determined by user
	int x, y;

	for (int i = 0; i < img.rows; i++) { 
		Vec3b *ptr = img.ptr<Vec3b>(i);
		for (int j = 0; j < img.cols; j++) { 

			double x_pixel[3] = { (double)j, (double)i, 1 };
			Mat Xresult(3, 1, CV_64FC1, x_pixel);
			Mat X = H.inv()*Xresult;
			x = cvRound(X.at<double>(0, 0) / X.at<double>(2, 0));	//normalization
			y = cvRound(X.at<double>(1, 0) / X.at<double>(2, 0));
			if (x < src.cols && y < src.rows && x >= 0 && y >= 0) {
				ptr[j] = src.at<Vec3b>(y, x);
			}
		}
	}
	return img;
}

int main(){

	Mat img = imread("../origin_pic.JPG"); 

	double t = static_cast<double>(getCPUTickCount());

	Point2d original_pic[4] = { Point2d(559, 529), Point2d(2041, 349), Point2d(573, 1733), Point2d(2053, 1887) };    //choose by hand
	Point2d homography_pic[4] = { Point2d(0, 0), Point2d(1024, 0), Point2d(0, 768), Point2d(1024, 768) };

	Mat H = H_matrix(original_pic, homography_pic);
	cout << H << endl;

	Mat result = Homography(img, H);
	imshow("Result", result);
	imwrite("Result.jpg", result);

	t = (getCPUTickCount() - t) / getTickFrequency();
	cout << t << "s" << endl;
	
	waitKey(0);
	return 0;
}