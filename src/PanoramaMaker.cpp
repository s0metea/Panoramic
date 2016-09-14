#include "../include/PanoramaMaker.h"

PanoramaMaker::PanoramaMaker(vector<int> camerasID, int frameWidth, int frameHeight)
{
	this->frameWidth = frameWidth;
	this->frameHeight = frameHeight;
	this->camerasAmount = (int)camerasID.size();
	this->homography.reserve(camerasAmount - 1);
    for (int i = 0; i < camerasAmount; i++) {
        cameras.push_back(VideoCapture(camerasID[i]));
        framesFromCameras.push_back(Mat());
    }
    homography.push_back(Mat());
    setCamerasResolution(this->frameWidth, this->frameHeight);
}

void PanoramaMaker::camInitialize() {
	for(int i = 0; i < this->camerasAmount; i++) {
		for(int j = 0; j < 5; j++) {
			cameras[i].read(framesFromCameras[i]);
		}
	}
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
    cameras.clear();
}

void PanoramaMaker::stop() {
	this->releaseCameras();
}


void PanoramaMaker::getCameraParam(int id) {
	cout << "Camera with id: " << id << endl;
	cout << this->cameras[id].get(3) << "X" << this->cameras[id].get(4) << endl;
}

void PanoramaMaker::getFrames() {
	for (int i = 0; i < this->camerasAmount; i++) {
		if(!cameras[i].grab())
		{
			cout << "Can not grab images from camera " << i << "!" << endl;
		}
		cameras[i].retrieve(framesFromCameras[i], 0);
	}

}

void PanoramaMaker::displayCurrentFrames() {

	for(int j = 0; j < framesFromCameras.size(); j++) {
		string path("raw/");
		imwrite(path.append(to_string(j).append(".jpg")), framesFromCameras[j]);
	}
}

void PanoramaMaker::rebuildHomography() {
    if(framesFromCameras[0].cols != framesFromCameras[1].cols && framesFromCameras[0].type() != framesFromCameras[1].type())
        return;
    Mat grayFrame0, grayFrame1;
    this->getFrames();
    descriptorsManager.computeDescriptors(framesFromCameras[0], framesFromCameras[1]);
    descriptorsManager.matchDescriptors();
    homography[0] = findHomography(descriptorsManager.getSecondImageMatchedKeypoints(), descriptorsManager.getFirstImageMatchedKeypoints(), CV_RANSAC, 3, noArray(), 2000, 0.995);
    while(homography[0].rows != 3) {
        cout << "Homography error. Trying to rebuild...";
        getFrames();
        descriptorsManager.computeDescriptors(framesFromCameras[0], framesFromCameras[1]);
        descriptorsManager.matchDescriptors();
        homography[0] = findHomography(descriptorsManager.getSecondImageMatchedKeypoints(), descriptorsManager.getFirstImageMatchedKeypoints(), CV_RANSAC, 3, noArray(), 2000, 0.995);
    }
    return;
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
	imwrite("raw/matches.jpg", img_matches);
}

Mat PanoramaMaker::getWarped() {
    getFrames();
    /////
    //TODO: Normal cropping.
    warpPerspective(framesFromCameras[1], warped, homography[0],
                    cv::Size(framesFromCameras[1].cols * 2, framesFromCameras[1].rows));
    cv::Mat half(warped, cv::Rect(0, 0, framesFromCameras[0].cols, framesFromCameras[0].rows));
    framesFromCameras[0].copyTo(half);
    return warped.clone();
}

void PanoramaMaker::initialize() {
    this->camInitialize();
    this->rebuildHomography();
}

int PanoramaMaker::getHomographyCoefficient(int x, int y) {
    if(x > 2 || y > 2 || x < 0 || y < 0)
        return -1;
    return (int) homography.data()->data[x * 3 + y];
}

void PanoramaMaker::setHomographyCoefficient(int x, int y, int value) {
    if(x > 2 || y > 2 || x < 0 || y < 0)
        return;
    homography.data()->data[x * 3 + y] = (unsigned char)value;
}
