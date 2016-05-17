#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>


using namespace std;
using namespace cv;


void printMat(Mat mat);


int main() {
    VideoCapture capture = VideoCapture(1);

     //////////////////////////////////
    //int boardWidth, boardHeight: width and height of calibration chessboard.
    //float measure
    //vector<vector<Point2f>> imagePoints
    //vector<vector<Point3f>> objectPoints
    //
    //


    int boardWidth = 6, boardHeight = 9;
    Size imageSize;

    vector<vector<Point2f>> imagePoints;
    vector<vector<Point3f>> objectPoints(1);

    int pressedKeyValue = 0;

    int chessboardCount = 0, maxChessboardCount = 15;
    bool chessboardDetected;
    Mat image, grayImage;
    capture >> image;
    imageSize = Size(image.cols, image.rows);
    image.release();
    vector<Point2f> corners;
    while (chessboardCount < maxChessboardCount && pressedKeyValue != 1048603) {

        capture >> image;
        flip(image, image, 2);
        grayImage = Mat(image.size(), 8, 1);
        cvtColor(image, grayImage, CV_RGB2GRAY);
        imshow("Successed", grayImage);
        chessboardDetected = findChessboardCorners(grayImage, Size(boardWidth, boardHeight), corners);
        if (chessboardDetected) {
            //draw corner
            cornerSubPix(grayImage, corners, Size(11, 11), Size(-1, -1),
                         TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1)); //wtf?
            drawChessboardCorners(grayImage, Size(boardWidth, boardHeight), Mat(corners), chessboardDetected);
            imagePoints.push_back(corners);
            imshow("Successed", grayImage);
            chessboardCount++;
        }
        image.release();
        grayImage.release();
        pressedKeyValue = waitKey(5);
    }

    capture >> image;

    //Calibration part
    vector<vector<Mat>> rvecs, tvecs;
    Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
    Mat distortion = Mat::zeros(8, 1, CV_64F);
    if(CV_CALIB_FIX_ASPECT_RATIO)
        cameraMatrix.at<double>(0,0) = 1.0;

    for( int i = 0; i < boardHeight; ++i )
        for( int j = 0; j < boardWidth; ++j )
            objectPoints[0].push_back(Point3f(float(j*0.2), float(i*0.2), 0));

    objectPoints.resize(imagePoints.size(),objectPoints[0]);

    waitKey();

    calibrateCamera(objectPoints, imagePoints, image.size(), cameraMatrix, distortion, rvecs, tvecs, 0);

    cout << "Done. Watch result?" << endl;

    waitKey();

    Mat mapx, mapy;
    initUndistortRectifyMap(cameraMatrix, distortion, Mat(), getOptimalNewCameraMatrix(cameraMatrix, distortion, imageSize, 1, imageSize, 0),
            imageSize, CV_16SC2, mapx, mapy);

    pressedKeyValue = 0;
    cout << "Ok, lets try!" << endl;
    while(pressedKeyValue != 1048603) {
        capture >> image;
        flip(image, image, 2);
        grayImage = Mat(image.size(), 8, 1);
        cvtColor(image, grayImage, CV_RGB2GRAY);
        Mat grayImageCorrected = grayImage.clone();

        imshow("Original", grayImage);
        remap(grayImage, grayImageCorrected, mapx, mapy, cv::INTER_CUBIC);
        grayImage.release();
        imshow("Corrected", grayImageCorrected);
        grayImageCorrected.release();

        pressedKeyValue = waitKey(5);
    }

    return 0;
}


void printMat(Mat mat) {
    for(int i=0; i< mat.rows; ++i)
    {
        for(int j=0; j< mat.cols; ++j)
        {
            printf("%lf ", mat.at< double>(i,j)); //cvmGet(matrix,i,j));
        }
        printf("\n");
    }
    printf("\n");
}