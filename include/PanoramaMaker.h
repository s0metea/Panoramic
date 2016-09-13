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
	void getCameraParam(int id);
	void releaseCameras();
	void initialize();
	void stop();
	void getFrames();
	void displayCurrentFrames();
	void rebuildHomography();


    Mat getWarped();
private:
    DescriptorsManager descriptorsManager = DescriptorsManager().getInstance();

    vector<VideoCapture> cameras;
    vector<Mat> framesFromCameras;
    vector<Mat> homography;
    vector<int> camerasID;

    Mat warped;

    int camerasAmount;
    int frameWidth;
    int frameHeight;

    void redrawMatches();
    void camInitialize();

};

