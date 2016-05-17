#include "PanoramaMaker.h"

PanoramaMaker::PanoramaMaker(vector<int> camerasID, int frameWidth, int frameHeight)
{
	this->frameWidth = frameWidth;
	this->frameHeight = frameHeight;
	this->camerasAmount = (int)camerasID.size();
	this->try_use_gpu = false;
	this->homography.reserve(camerasAmount - 1);
	for (int i = 0; i < camerasAmount; i++) {
		cameras.push_back(VideoCapture(camerasID[i]));
		framesFromCameras.push_back(Mat());
	}
	setCamerasResolution(this->frameWidth, this->frameHeight);
}

void PanoramaMaker::setCamerasResolution(int frameWidth, int frameHeight) {
	for (int i = 0; i < this->camerasAmount; i++) {
		cameras[i].set(CV_CAP_PROP_FRAME_WIDTH, frameWidth);
		cameras[i].set(CV_CAP_PROP_FRAME_HEIGHT, frameHeight);
	}
}

void PanoramaMaker::releaseCameras() {
	for (int i = 0; i < this->camerasAmount; i++) {
		cameras[i].release();
	}
}

void PanoramaMaker::stop() {
	destroyAllWindows();
	this->releaseCameras();
}


void PanoramaMaker::getCameraInfo(int id) {
	cout << "Camera with id: " << id << endl;
	cout << this->cameras[id].get(3) << "X" << this->cameras[id].get(4) << endl;
}

void PanoramaMaker::getFrames() {
	for (int i = 0; i < this->camerasAmount; i++) {
		cameras[i] >> framesFromCameras[i];
	}

}

void PanoramaMaker::displayCurrentFrames() {
	for(int j = 0; j < framesFromCameras.size(); j++) {
		imshow(to_string(j), framesFromCameras[j]);
	}
}



void PanoramaMaker::rebuildHomography() {
		if(framesFromCameras[0].cols != framesFromCameras[1].cols && framesFromCameras[0].type() != framesFromCameras[1].type())
			return;
		Mat grayFrame0, grayFrame1;
		cvtColor(framesFromCameras[0], grayFrame0, CV_BGR2GRAY);
		cvtColor(framesFromCameras[1], grayFrame1, CV_BGR2GRAY);
		descriptorsManager.computeDescriptors(grayFrame0, grayFrame1);
		descriptorsManager.matchDescriptors();
		homography[0] = findHomography(descriptorsManager.getSecondImageMatchedKeypoints(), descriptorsManager.getFirstImageMatchedKeypoints(), CV_RANSAC, 3, noArray(), 2000, 0.995);
}

void PanoramaMaker::redrawMatches() {
	if(framesFromCameras[0].cols != framesFromCameras[1].cols && framesFromCameras[0].type() != framesFromCameras[1].type())
		return;
	Mat img_matches;
	Mat grayFrame0, grayFrame1;
	cvtColor(framesFromCameras[0], grayFrame0, CV_BGR2GRAY);
	cvtColor(framesFromCameras[1], grayFrame1, CV_BGR2GRAY);
	drawMatches(framesFromCameras[0], descriptorsManager.getFirstImageKeypoints(), framesFromCameras[1], descriptorsManager.getSecondImageKeypoints(),
				descriptorsManager.getBestMatches(), img_matches, Scalar::all(-1), Scalar::all(-1),
				vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imshow("Matches", img_matches);
}


void PanoramaMaker::start() {

	int keyPressed = 0;
	int framesCount = 0;
	int64 start, end;

	this->getFrames();
	this->rebuildHomography();
	this->redrawMatches();
	keyPressed = 0;

	Mat warped;
	//Esc pressed
	start = getTickCount();
	while (keyPressed % 256 != 27) {
		if(keyPressed % 256 == 32) {
			this->rebuildHomography();
			this->redrawMatches();
		}

		this->getFrames();
		this->displayCurrentFrames();

//Creating panorama:

		//TODO: Normal cropping.
		warpPerspective(framesFromCameras[1], warped, homography[0], cv::Size(framesFromCameras[1].cols * 2, framesFromCameras[1].rows));
		cv::Mat half(warped,cv::Rect(0, 0, framesFromCameras[0].cols, framesFromCameras[0].rows));
		framesFromCameras[0].copyTo(half);
		imshow("Result", warped);

		framesCount++;

		keyPressed = cv::waitKey(5);

	}

	end = getTickCount();
	stop();
	cout << "FPS: " << framesCount / ((end - start) / getTickFrequency()) << endl;
	cout << "Exit!";
}