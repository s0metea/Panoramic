// Matcher.cpp
#include "../include/DescriptorsManager.h"

DescriptorsManager *DescriptorsManager::instance = 0;
DescriptorsManagerDestroyer DescriptorsManager::destroyer;

DescriptorsManagerDestroyer::~DescriptorsManagerDestroyer() {
    delete instance;
}
void DescriptorsManagerDestroyer::initialize(DescriptorsManager* p) {
    instance = p;
}

DescriptorsManager &DescriptorsManager::getInstance() {
    if (!instance) {
        instance = new DescriptorsManager();
        destroyer.initialize(instance);
    }
    return *instance;
}

void DescriptorsManager::computeDescriptors(cv::Mat a, cv::Mat b) {
    sift->clear();
    firstImageKeypoints.clear();
    secondImageKeypoints.clear();
    sift->detectAndCompute(a, cv::noArray(), firstImageKeypoints, firstImageDescriptors, false);
    sift->detectAndCompute(b, cv::noArray(), secondImageKeypoints, secondImageDescriptors, false);
}

void DescriptorsManager::matchDescriptors() {
    sift->clear();
    matches.clear();
    bestMatches.clear();
    firstImageMatchedKeypoints.clear();
    secondImageMatchedKeypoints.clear();

    matcher->match(firstImageDescriptors, secondImageDescriptors, matches);

    //TODO: is it a better way?
    double  minDist = 100, maxDist = 0;
    for( int i = 0; i < matches.size(); i++ ) {
        double dist = matches[i].distance;
        if(dist < minDist) minDist = dist;
        if(dist > maxDist) maxDist = dist;
    }

    for( int i = 0; i < matches.size(); i++ ) {
        if(matches[i].distance <= 3 * minDist) {
            bestMatches.push_back(matches[i]);
        }
    }

    for(int i = 0; i < bestMatches.size(); i++) {
        firstImageMatchedKeypoints.push_back(firstImageKeypoints[bestMatches[i].queryIdx].pt);
        secondImageMatchedKeypoints.push_back(secondImageKeypoints[bestMatches[i].trainIdx].pt);
    }
}

std::vector<cv::DMatch> DescriptorsManager::getBestMatches() {
    return bestMatches;
}

