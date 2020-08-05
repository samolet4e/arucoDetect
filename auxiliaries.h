#ifndef AUXILIARIES_H
#define AUXILIARIES_H

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <unordered_map>

//#define APRILTAG

typedef float Real;

typedef struct _fixation {
	Real startTime, duration;
	cv::Point2f X;
} fixation;

//cv::Mat hwnd2mat(HWND hwnd);
int imshowT(cv::Mat input, std::unordered_map<int, cv::Point2f > &umap, std::vector <fixation> &fPoint, Real timeStamp);
void readFixations(std::vector <fixation> &fPoint, int w, int h);

#endif
