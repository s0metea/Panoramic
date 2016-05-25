#include <mongoose.h>
#include "../include/PanoramaMaker.h"

PanoramaMaker::PanoramaMaker(vector<int> camerasID, int frameWidth, int frameHeight)
{
	this->frameWidth = frameWidth;
	this->frameHeight = frameHeight;
	this->camerasAmount = (int)camerasID.size();
	this->try_use_gpu = false;
	this->homography.reserve(camerasAmount - 1);
	for (int i = 0; i < camerasAmount; i++) {
		cameras.push_back(VideoCapture(camerasID[i]));
		cameras[i].set(CV_CAP_PROP_FOURCC ,CV_FOURCC('M', 'J', 'P', 'G') );
		framesFromCameras.push_back(Mat());
	}
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
}

void PanoramaMaker::stop() {
	//destroyAllWindows();
	this->releaseCameras();
}


void PanoramaMaker::getCameraInfo(int id) {
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
		string path("img/");
		imwrite(path.append(to_string(j).append(".jpg")), framesFromCameras[j]);
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
	imwrite("img/matches.jpg", img_matches);
}


void PanoramaMaker::start(mg_server *server, int *action) {

	int framesCount = 0;
	int64 start, end;

	this->camInitialize();

	this->getFrames();
	this->rebuildHomography();
	this->redrawMatches();
	Mat warped;
	*action = 0;
	start = getTickCount();
	int i = 0;
	while (true) {
        switch (*action) {
            case 50:
                end = getTickCount();
                stop();
                cout << "FPS: " << framesCount / ((end - start) / getTickFrequency()) << endl;
                while(*action != 1) {
                    mg_poll_server(server, 100);
                }
                *action = 0;
                break;
            case 51:
                this->rebuildHomography();
                this->redrawMatches();
                cout << "Rebuilded!" << endl;
                *action = 0;
                break;
            case 52:
                stop();
                mg_destroy_server(&server);
                return;
            default:break;
        }
		this->getFrames();
		this->displayCurrentFrames();

//Creating panorama:

		//TODO: Normal cropping.
		warpPerspective(framesFromCameras[1], warped, homography[0], cv::Size(framesFromCameras[1].cols * 2, framesFromCameras[1].rows));
		cv::Mat half(warped,cv::Rect(0, 0, framesFromCameras[0].cols, framesFromCameras[0].rows));
		framesFromCameras[0].copyTo(half);
		string imgNum("img/result");
		imgNum.append(to_string(i)).append(".jpg");
		imwrite(imgNum, warped);
		imgNum.clear();
		framesCount++;
		i++;
		mg_poll_server(server, 100);
	}

}