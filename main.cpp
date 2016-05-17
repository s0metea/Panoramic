#include <opencv2/core.hpp>
#include <iostream>
#include "PanoramaMaker.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    /*
	Mat a = imread("/home/sometea/Dropbox/Work/4 курс/Защита информационных процессов/Panoramic/part1.jpg");
	Mat b = imread("/home/sometea/Dropbox/Work/4 курс/Защита информационных процессов/Panoramic/part2.jpg");

    if (!a.data || !b.data)
    {
        std::cout << "Images not loaded";
        return -1;
    }

	imshow("Left", a);
	imshow("Right", b);

	Mat c;
	DescriptorsManager descriptorsManager = DescriptorsManager::getInstance();
	descriptorsManager.computeDescriptors(a, b);
    descriptorsManager.matchDescriptors();
	drawMatches(a, descriptorsManager.getFirstImageKeypoints(), b, descriptorsManager.getSecondImageKeypoints(),
				descriptorsManager.getBestMatches(), c, Scalar::all(-1), Scalar::all(-1),
				vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imshow("Matches", c);

	Mat H;
	H = findHomography(descriptorsManager.getSecondImageMatchedKeypoints(), descriptorsManager.getFirstImageMatchedKeypoints(), CV_RANSAC, 3, noArray(), 2000, 0.995);
	imshow("Homography", H);
	cout << H << endl;

	Mat warped;
	warpPerspective(b, warped, H, cv::Size(b.cols * 2, b.rows));
	imshow("Warped", warped);

	Mat result = warped.clone();
	cv::Mat half(result, cv::Rect(0, 0, a.cols, b.rows));
	a.copyTo(half);
	imshow("Result", result);

    waitKey();
	*/
	vector<int> camerasID;
	camerasID.push_back(1);
	camerasID.push_back(2);
	PanoramaMaker pm(camerasID, 640, 480);
	pm.start();
}
