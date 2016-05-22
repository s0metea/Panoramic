#include <iostream>
#include <opencv2/core.hpp>
#include "PanoramaMaker.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	int firstCamera, secondCamera;
	int frameWidth, frameHeight;
	cout << "Hello, enter id of first camera: " << endl;
	cin >> firstCamera;
	cout << "Ok, first camera will be initialize with id " << firstCamera << "." << endl;
	cout << "Now you need to tell me id of the second camera:" << endl;
	cin >> secondCamera;
	cout << "Ok, second camera will be initialize with id " << secondCamera << "." << endl;

	cout << "Please enter width of the frame: " << endl;
	cin >> frameWidth;
	cout << "And height of the frame:" << endl;
	cin >> frameHeight;
	cout << "Trying to apply parameters..." << endl;

	vector<int> camerasID;
	camerasID.push_back(firstCamera);
	camerasID.push_back(secondCamera);
	PanoramaMaker pm(camerasID, frameWidth, frameHeight);
	cout << "Ok, we can start just now. Enjoy your realtime panorama stitching!" << endl;
	pm.start();
}
