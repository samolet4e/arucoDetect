#include "auxiliaries.h"
#define UNDISTORT

int main() {
//HWND hDesktopWnd = GetDesktopWindow();
bool playVideo = true, playTransform = false;
std::vector< int > ids;
std::vector< std::vector<cv::Point2f> > corners;

#ifdef APRILTAG
	int dictionaryId = cv::aruco::DICT_APRILTAG_36h11;
#else
	int dictionaryId = cv::aruco::DICT_ARUCO_ORIGINAL;
#endif

	cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));




	// http://aishack.in/tutorials/calibrating-undistorting-opencv-oh-yeah/
	// https://riptutorial.com/opencv/example/9922/efficient-pixel-access-using-cv--mat--ptr-t--pointer
	cv::Mat intrinsic = cv::Mat(3, 3, CV_32FC1);
	cv::Mat distCoeffs;
	std::vector<cv::Mat> rvecs;
	std::vector<cv::Mat> tvecs;

	cv::Mat dist_coeffs;
	cv::FileStorage fs("intrinsicArucoSimLatShortF.yml", cv::FileStorage::READ);
	fs["camera_matrix"] >> intrinsic;
	fs["distortion_coefficients"] >> dist_coeffs;


	cv::VideoCapture cap("output.avi");
	if (!cap.isOpened()) {
		std::cout << "Error opening video stream or file" << std::endl;
		return -1;
	}//if

	int capHeight = (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	int capWidth = (int)cap.get(cv::CAP_PROP_FRAME_WIDTH);

	cv::Mat imageDistorted(capHeight, capWidth, CV_8UC1, cv::Scalar(255, 255, 255));
	cv::Mat image = imageDistorted.clone();

	// Define a vector with initial size of zero elements and default fixation constructor
	std::vector <fixation> fPoint(0, { 0.f, 0.f, cv::Point2f(0.f, 0.f) });
	readFixations(fPoint, capWidth, capHeight);

	while (1) {
#ifdef UNDISTORT
		if (playVideo) { cap >> imageDistorted; cv::undistort(imageDistorted, image, intrinsic, dist_coeffs); }
#else
		if (playVideo) cap >> image;
#endif
		if (image.empty()) { std::cout << "Empty Frame\n"; break; }

		if (playVideo) {

			cv::aruco::detectMarkers(image, dictionary, corners, ids);
			if (ids.size() > 0) cv::aruco::drawDetectedMarkers(image, corners, ids); // If at least one marker detected

			// for (int i = 0; i < ids.size(); i++) std::cout << ids.at(i) << std::endl; std::cout << std::endl;
			// ACHTUNG! Every frame markers are detected in an arbitrary order. Consider using a hasmap instead of an array!
			std::unordered_map< int, cv::Point2f > umap;		 // umap[markerID, markerCentroid]
			// draws a circle at each marker centroid
			for (int i = 0; i < (int)ids.size(); i++) {			 // scans all markers
				std::vector<cv::Point2f> points = corners.at(i); // corners at ith marker
				float p[] = { 0., 0. };						     // current marker centroid WITH INITIALISATION (obligatory due to recursion)
				for (int j = 0; j < (int)points.size(); j++) {	 // scans all corners within current marker
					p[0] += points.at(j).x;
					p[1] += points.at(j).y;
				}//for_j
				p[0] *= 0.25;
				p[1] *= 0.25;
				cv::circle(image, cv::Point2f(p[0], p[1]), 10, cv::Scalar(0, 0, 255), cv::FILLED, cv::LINE_AA, 0);
				umap[ids.at(i)] = cv::Point2f(p[0], p[1]);
				// p[0] = 0.; p[1] = 0.; // redundant in spite of recursion
			}//for_i

			Real currTimeStamp = (Real)cap.get(cv::CAP_PROP_POS_MSEC) / 1000.f;

			// what to play: normal or transformed image
			if (playTransform) imshowT(image, umap, fPoint, currTimeStamp);
			else imshow("Detected markers", image);

			//umap.clear(); // redundant, the map is overwritten at the same indices

			std::cout << currTimeStamp << std::endl;

		}//if

		// flow control
		char key = cv::waitKey(20);
		if (key == 'p') playVideo = !playVideo;
//		else if (key == 's') { cv::Mat shot = hwnd2mat(hDesktopWnd); imshow("Screenshot", shot); } // takes a screenshot
		else if (key == 't') playTransform = !playTransform;
		else if (key == 27) break; // a very complicated behaviour
		else {}

	}//while

	cap.release();

	return 0;
}//main
