#include "auxiliaries.h"

// http://opencvexamples.blogspot.com/2014/01/perspective-transform.html
int imshowT(cv::Mat input, std::unordered_map< int, cv::Point2f > &umap) {

	// Input Quadrilateral
	cv::Point2f inputQuad[4];
	// Output Quadrilateral
	cv::Point2f outputQuad[4];
	// Output Image;
	cv::Mat output;// = input.clone();
	// Lambda Matrix
	cv::Mat lambda(3, 3, CV_32FC1);

	// The 4 points that select quadilateral on the input, from bottom-left in counterclockwise order
	inputQuad[0] = umap[105];
	inputQuad[1] = umap[106];
	inputQuad[2] = umap[107];
	inputQuad[3] = umap[108];

	// The 4 points where the mapping is to be done, from bottom-left in counterclockwise order
	outputQuad[0] = cv::Point2f(0.f, (Real)input.rows);
	outputQuad[1] = cv::Point2f((Real)input.cols, (Real)input.rows);
	outputQuad[2] = cv::Point2f((Real)input.cols, 0.f);
	outputQuad[3] = cv::Point2f(0., 0.);

	// Get the Perspective Transform Matrix i.e. lambda
	lambda = getPerspectiveTransform(inputQuad, outputQuad);
	// Apply the Perspective Transform just found to the src image
	warpPerspective(input, output, lambda, output.size(), cv::INTER_LINEAR);

	// Information sources
	cv::rectangle(output, cv::Point2f(10.f,  10.f),  cv::Point2f(550.f,  710.f), cv::Scalar(0, 0, 255), 3, cv::LINE_AA, 0);
	cv::rectangle(output, cv::Point2f(560.f, 10.f),  cv::Point2f(1210.f, 460.f), cv::Scalar(0, 0, 255), 3, cv::LINE_AA, 0);
	cv::rectangle(output, cv::Point2f(560.f, 470.f), cv::Point2f(900.f,  710.f), cv::Scalar(0, 0, 255), 3, cv::LINE_AA, 0);
	cv::rectangle(output, cv::Point2f(910.f, 470.f), cv::Point2f(1210.f, 710.f), cv::Scalar(0, 0, 255), 3, cv::LINE_AA, 0);

	// Display input and output
	imshow("Detected markers", output);

	return 0;
}//imshowT

void readFixations(std::vector <fixation> &fPoint, int w, int h) {
	Real startTime0;

	std::ifstream myFile("fixations.txt");
	std::string line;

	bool firstLine = true;
	while (getline(myFile, line)) {
		std::stringstream ss(line);
		Real startTime, duration;
		cv::Point2f X;
		ss >> startTime >> duration >> X.x >> X.y;
		if (firstLine == true) { startTime0 = startTime; firstLine = false; }
		startTime -= startTime0;
		X.x *= w;
		//		X.y = 1.f - X.y; // reverse
		X.y *= h;
		fixation blobT = { startTime, duration / 1000.f, cv::Point2f(X.x, X.y) }; // init the structure here
		fPoint.push_back(blobT);
	}//while

	myFile.close();

	return;
}//readFixations

void drawFixation(cv::Mat input, std::vector <fixation> &fPoint, Real timeStamp) { //, Real lambda) {

	int blobNo = 0;
	bool toDraw = false;
	// Look up fixation the current timestamp falls in
	while (blobNo < fPoint.size()) {
		Real s = fPoint.at(blobNo).startTime, e = fPoint.at(blobNo).startTime + fPoint.at(blobNo).duration;
		if (s < timeStamp && e > timeStamp) { toDraw = true; break; }
		blobNo++;
	}//while_i

	if (toDraw) {
		// https://stackoverflow.com/questions/30909791/how-to-convert-homogeneous-points-to-inhomogeneous-points-using-opencv-in-c
		cv::Point3f fix = cv::Point3f(fPoint.at(blobNo).X.x, fPoint.at(blobNo).X.y, 1.f); // Fixation point homogenous
		cv::circle(input, cv::Point2f(fix.x, fix.y), 25, cv::Scalar(255, 255, 0), 5, cv::LINE_AA, 0);
		cv::putText(input, cv::format("%.3f", fPoint.at(blobNo).duration), cv::Point2f(fix.x + 25.f, fix.y + 35.f), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, cv::Scalar(255, 255, 0), 1, cv::LINE_AA);
	}//if

	return;
}//drawFixation