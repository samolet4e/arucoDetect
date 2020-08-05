#include "auxiliaries.h"

// http://opencvexamples.blogspot.com/2014/01/perspective-transform.html
int imshowT(cv::Mat input, std::unordered_map< int, cv::Point2f > &umap, std::vector <fixation> &fPoint, Real timeStamp) {

	// Input Quadrilateral
	cv::Point2f inputQuad[4];
	// Output Quadrilateral
	cv::Point2f outputQuad[4];
	// Output Image;
	cv::Mat output;// = input.clone();
	// Lambda Matrix
	cv::Mat lambda(3, 3, CV_32FC1);

	// Set the lambda matrix the same type and size as input (but why?)
//	lambda = cv::Mat::zeros(input.rows, input.cols, input.type());

	// The 4 points that select quadilateral on the input, from bottom-left in counterclockwise order
#ifdef APRILTAG
	// AprilTag
	inputQuad[3] = umap[0];
	inputQuad[2] = umap[14];
	inputQuad[1] = umap[22];
	inputQuad[0] = umap[17];
#else
	// Aruco
	inputQuad[0] = umap[105];
	inputQuad[1] = umap[106];
	inputQuad[2] = umap[107];
	inputQuad[3] = umap[108];
#endif
	// The 4 points where the mapping is to be done, from bottom-left in counterclockwise order
	outputQuad[0] = cv::Point2f(0.f, (Real)input.rows);
	outputQuad[1] = cv::Point2f((Real)input.cols, (Real)input.rows);
	outputQuad[2] = cv::Point2f((Real)input.cols, 0.f);
	outputQuad[3] = cv::Point2f(0., 0.);

	// Get the Perspective Transform Matrix i.e. lambda
	lambda = getPerspectiveTransform(inputQuad, outputQuad);
//	std::cout << lambda << std::endl;
	// Apply the Perspective Transform just found to the src image
	warpPerspective(input, output, lambda, output.size(), cv::INTER_LINEAR);

	// Information sources
	cv::rectangle(output, cv::Point2f(10.f,  10.f),  cv::Point2f(590.f,  710.f), cv::Scalar(0, 0, 255), 3, cv::LINE_AA, 0);
	cv::rectangle(output, cv::Point2f(600.f, 10.f),  cv::Point2f(1210.f, 460.f), cv::Scalar(0, 0, 255), 3, cv::LINE_AA, 0);
	cv::rectangle(output, cv::Point2f(600.f, 470.f), cv::Point2f(900.f,  710.f), cv::Scalar(0, 0, 255), 3, cv::LINE_AA, 0);
	cv::rectangle(output, cv::Point2f(910.f, 470.f), cv::Point2f(1210.f, 710.f), cv::Scalar(0, 0, 255), 3, cv::LINE_AA, 0);
/*
	cv::line(output, cv::Point2f(905.f, 10.f), cv::Point2f(905.f, 460.f), cv::Scalar(0, 0, 255), 3, cv::LINE_AA, 0);
	cv::line(output, cv::Point2f(600.f, 235.f), cv::Point2f(1210.f, 235.f), cv::Scalar(0, 0, 255), 3, cv::LINE_AA, 0);
*/
	static int fText[4] = { 0, 0, 0, 0 };
	cv::putText(output, cv::format("%d", fText[0]), cv::Point2f(505.f, 45.f),   cv::FONT_HERSHEY_COMPLEX_SMALL, 2.0, cv::Scalar(255, 255, 0), 1, cv::LINE_AA);
	cv::putText(output, cv::format("%d", fText[1]), cv::Point2f(1120.f, 45.f),  cv::FONT_HERSHEY_COMPLEX_SMALL, 2.0, cv::Scalar(255, 255, 0), 1, cv::LINE_AA);
	cv::putText(output, cv::format("%d", fText[2]), cv::Point2f(1150.f, 505.f), cv::FONT_HERSHEY_COMPLEX_SMALL, 2.0, cv::Scalar(255, 255, 0), 1, cv::LINE_AA);
	cv::putText(output, cv::format("%d", fText[3]), cv::Point2f(840.f, 505.f),  cv::FONT_HERSHEY_COMPLEX_SMALL, 2.0, cv::Scalar(255, 255, 0), 1, cv::LINE_AA);

	int blob = 0;
	static int blob0 = 0;
	bool newBlob = false;
	bool toDraw = false;
	// Look up a fixation the current timestamp falls in
	while (blob < fPoint.size()) {
		Real s = fPoint.at(blob).startTime, e = fPoint.at(blob).startTime + fPoint.at(blob).duration;
		if (s < timeStamp && e > timeStamp) { toDraw = true; break; }
		blob++;
	}//while_i

	if (blob0 != blob) newBlob = true;

	if (toDraw) {
		// https://stackoverflow.com/questions/30909791/how-to-convert-homogeneous-points-to-inhomogeneous-points-using-opencv-in-c
		cv::Point3f fix = cv::Point3f(fPoint.at(blob).X.x, fPoint.at(blob).X.y, 1.f); // Fixation point homogenous
		cv::Point3f fixT = cv::Point3f(0.f, 0.f, 0.f);								  // Fixation point transformed homogenous

		// Work out a solution to matrix by vector multiplication
		Real L00 = (Real)lambda.at<double>(0, 0), L01 = (Real)lambda.at<double>(0, 1), L02 = (Real)lambda.at<double>(0, 2),
			 L10 = (Real)lambda.at<double>(1, 0), L11 = (Real)lambda.at<double>(1, 1), L12 = (Real)lambda.at<double>(1, 2),
			 L20 = (Real)lambda.at<double>(2, 0), L21 = (Real)lambda.at<double>(2, 1), L22 = (Real)lambda.at<double>(2, 2);

		fixT.x = L00 * fix.x + L01 * fix.y + L02 * fix.z;
		fixT.y = L10 * fix.x + L11 * fix.y + L12 * fix.z;
		fixT.z = L20 * fix.x + L21 * fix.y + L22 * fix.z;

		fixT.x /= fixT.z;
		fixT.y /= fixT.z;

		cv::circle(output, cv::Point2f(fixT.x, fixT.y), 25, cv::Scalar(255, 255, 0), 5, cv::LINE_AA, 0);
		cv::putText(output, cv::format("%.3f", fPoint.at(blob).duration), cv::Point2f(fixT.x + 25.f, fixT.y + 35.f), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.5, cv::Scalar(255, 255, 0), 1, cv::LINE_AA);

			 if (fixT.x > 10.f  && fixT.x < 590.f  && fixT.y > 10.f  && fixT.y < 710.f && newBlob) { fText[0] += 1; newBlob = false; }// first information source
		else if (fixT.x > 600.f && fixT.x < 1210.f && fixT.y > 10.f  && fixT.y < 460.f && newBlob) { fText[1] += 1; newBlob = false; }// second information source
		else if (fixT.x > 910.f && fixT.x < 1210.f && fixT.y > 470.f && fixT.y < 710.f && newBlob) { fText[2] += 1; newBlob = false; }// third information source
		else if (fixT.x > 600.f && fixT.x < 900.f  && fixT.y > 470.f && fixT.y < 710.f && newBlob) { fText[3] += 1; newBlob = false; }// fourth information source
		else {}

//		std::cout << cv::Point2f(fixT.x, fixT.y) << std::endl;
	}//if

	blob0 = blob;

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
		X.y = 1.f - X.y; // reverse
		X.y *= h;
		fixation blobT = { startTime, duration / 1000.f, cv::Point2f(X.x, X.y) }; // init the structure here
		fPoint.push_back(blobT);
	}//while

	myFile.close();

//	for (int i = 0; i < fPoint.size(); i++) std::cout << fPoint.at(i).X << std::endl;

	return;
}//readFixations
