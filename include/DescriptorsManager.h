//Matcher.h

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include "opencv2/xphoto.hpp"
#include "opencv2/calib3d.hpp"
#include <opencv2/imgproc.hpp>

class DescriptorsManager;

class DescriptorsManagerDestroyer
{
private:
    DescriptorsManager* instance;
public:
    ~DescriptorsManagerDestroyer();
    void initialize(DescriptorsManager * p);
};


class DescriptorsManager {
private:
    static DescriptorsManager *instance;
    static DescriptorsManagerDestroyer destroyer;
    std::vector<cv::KeyPoint> firstImageKeypoints, secondImageKeypoints;
    cv::Ptr<cv::xfeatures2d::SIFT> sift = cv::xfeatures2d::SIFT::create();
    cv::Mat firstImageDescriptors, secondImageDescriptors;
    std::vector<cv::Point2f> firstImageMatchedKeypoints, secondImageMatchedKeypoints;

    cv::Ptr<cv::DescriptorMatcher> matcher = cv::FlannBasedMatcher::create("FlannBased");
    std::vector<cv::DMatch> matches;
    std::vector<cv::DMatch> bestMatches;

    friend class DescriptorsManagerDestroyer;

public:
    static DescriptorsManager &getInstance();

    void computeDescriptors(cv::Mat a, cv::Mat b);

    void matchDescriptors();

    std::vector<cv::DMatch> getBestMatches();

    //Getters below:
    std::vector<cv::KeyPoint> &getFirstImageKeypoints() {
        return firstImageKeypoints;
    }

    std::vector<cv::KeyPoint> &getSecondImageKeypoints() {
        return secondImageKeypoints;
    }

    cv::Mat &getFirstImageDescriptors() {
        return firstImageDescriptors;
    }

    cv::Mat &getSecondImageDescriptors() {
        return secondImageDescriptors;
    }

    std::vector<cv::Point2f> &getFirstImageMatchedKeypoints() {
        return firstImageMatchedKeypoints;
    }

    std::vector<cv::Point2f> &getSecondImageMatchedKeypoints() {
        return secondImageMatchedKeypoints;
    }
    //End of getters
};