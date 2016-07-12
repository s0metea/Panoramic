#pragma once
#include <iostream>
#include <opencv2/videoio.hpp>
#include "DescriptorsManager.h"
#include "mongoose.h"


using namespace std;
using namespace cv;

class PanoramaMaker
{
public:
	PanoramaMaker(vector<int> camerasID, int frameWidth, int frameHeight);
	void setCamerasResolution(int frameWidth, int frameHeight);
	void getCameraInfo(int id);
	void releaseCameras();
	void start(mg_server *server, int *action);
	void stop();
	void getFrames();
	void displayCurrentFrames();
	void rebuildHomography();
	int c;
private:
	vector<VideoCapture> cameras;
	vector<Mat> framesFromCameras;
	vector<Mat> homography;
	DescriptorsManager descriptorsManager;
	int camerasAmount;
	int frameWidth;
	int frameHeight;
	vector<int> camerasID;
	bool try_use_gpu; //Currently unavailable!
	volatile int keyPressed;
	void redrawMatches();

	void camInitialize();
};

