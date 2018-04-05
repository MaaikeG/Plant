// SkinAndGestures.cpp : Defines the entry point for the console application.
// Coert van Gemeren (2017) 
// Use this code under the following terms:
// Attribution: appropriate credit + indicate if changes were made
//
// Uses: Unistroke Recognizer by Baylor Wetzel 
#include "stdafx.h"

#include <opencv2/opencv.hpp>
#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include <string>
#include <iostream>
#include "PSkinLoader.h"

using namespace DollarRecognizer;

/*
The ActiveCanvas is a struct that keeps track of the button status and gestures.

INPUT: active_templates, a list of template names which are to be recognized
 */
const int nFrames = 20;

struct ActiveCanvas
{
	// Constructor
	ActiveCanvas(const std::vector<std::string> &active_templates) :
		is_lbutton_down(false),
		is_result_shown(false),
		last_result(nullptr)
	{
		// Load the Dollar shape templates
		recognizer.loadTemplates();

		// Active the given templates
		recognizer.activateTemplates(active_templates);
	}

	// Copy of the video frame we can draw on
	cv::Mat canvas;

	// The Dollar shape recognizer
	GeometricRecognizer recognizer;

	// Unique pointer (not copyable) to the last recognition result
	std::unique_ptr<RecognitionResult> last_result;

	// Is the left mouse button pressed?
	bool is_lbutton_down;

	// Has the last recognition result been shown?
	bool is_result_shown;

	// List of recorded [x,y] coordinates
	Path2D positions;

	// number of fingers recognized in last 10 frames
	int nFingersPointer = 0;
	int nFingersPerFrame[nFrames] = { 0 };
	int nFingers;
};

// Set recognized finger number in the array containing n fingers recognized in last x frames.
// Return number of fingers that was recognized most in last x frames.
void calcNFingers(ActiveCanvas* gesture_recorder, int currentNFingers) {
	int* nFingersPerFrame = gesture_recorder->nFingersPerFrame;
	int* pointer = &gesture_recorder->nFingersPointer;
	*(nFingersPerFrame + *pointer) = currentNFingers;
	*pointer = (*pointer + 1) % nFrames;

	// we have only 5 fingers so don't need more than 6 bins.
	int nfingerBins[6] = { 0 };
	for (int i = 0; i < nFrames; i++) {
		if (*(nFingersPerFrame + i) < 6 && *(nFingersPerFrame + i) >= 0) {
			// get n fingers recognized in frame 1, and increment that bin.
			nfingerBins[*(nFingersPerFrame + i)]++;
		}
	}

	// Get bin index with highest value.
	int max_value = 0;
	int max_index = 0;
	for (int i = 0; i < 6; i++) {
		if (nfingerBins[i] > max_value) {
			max_value = nfingerBins[i];
			max_index = i;
		}
	}
	gesture_recorder->nFingers = max_index;
}


float getDistance(cv::Point a, cv::Point b) {
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

float getAngle(cv::Point s, cv::Point f, cv::Point e) {
	float l1 = getDistance(f, s);
	float l2 = getDistance(f, e);
	float rad = acos((s - f).dot(e - f) / (l1*l2));
	return rad * 180 / CV_PI;
}

struct colorEncodingVariables {
	int threshold = 20;
	int blue = 100, red = 100, hueMin = 0, hueMax = 256;
};

void setWindow(colorEncodingVariables* encoding)
{
	cv::namedWindow("detection result", CV_WINDOW_FREERATIO);

	cv::createTrackbar("threshold", "detection result", &(encoding->threshold), 100);
	cv::createTrackbar("Blue weight", "detection result", &(encoding->blue), 200);
	cv::createTrackbar("Red weight", "detection result", &(encoding->red), 200);
	cv::createTrackbar("Hue Min", "detection result", &(encoding->hueMin), 256);
	cv::createTrackbar("Hue Max", "detection result", &(encoding->hueMax), 256);
}



int main()
{
	int bin_size = 32;    // TODO find suitable size
	int threshold = 20;

	PSkinLoader pSkinLoader = PSkinLoader(bin_size);
	colorEncodingVariables colorEncodingVars;

	int useYCrCb = 1;// is an Int because we can only use a trackbar to set this :(
	setWindow(&colorEncodingVars);
	cv::createTrackbar("HSV / YCrCb", "detection result", &useYCrCb, 1);

	// Open the webcam
	cv::VideoCapture video_capture(0);

	// Set list of gestures to be recognized
	// see http://depts.washington.edu/aimgroup/proj/dollar/
	std::vector<std::string> active_templates = { "Triangle", "X", "Rectangle", "Circle" };
	ActiveCanvas gesture_recorder(active_templates);


	int key = -1;
	while (key != 27)
	{
		// Grab the webcam image
		cv::Mat capture_rgb;
		video_capture >> capture_rgb;
		cv::cvtColor(capture_rgb, gesture_recorder.canvas, cv::ColorConversionCodes::COLOR_RGB2YCrCb);

		// Initialize an empty mask
		cv::Mat test_mask = cv::Mat::zeros(gesture_recorder.canvas.size(), CV_8U);

		if (!pSkinLoader.p_skin_YCrCb.empty())
		{
#pragma omp parallel for
			for (int y = 0; y < gesture_recorder.canvas.rows; ++y)
			{
				const uchar* c_scanline = gesture_recorder.canvas.ptr(y); // skip to row in the canvas
				uchar* t_scanline = test_mask.ptr(y);                     // skip to row in the mask

				for (int x = 0; x < gesture_recorder.canvas.cols; ++x)
				{
					// skip to column (3 colors per cell)
					const uchar* value = c_scanline + x * 3;
					float prob;
					if (useYCrCb) {
						prob = pSkinLoader.getp_skin_YCrCb(value, colorEncodingVars.red, colorEncodingVars.blue);
					}
					else {
						prob = pSkinLoader.getp_skin_HSV(value, colorEncodingVars.hueMin, colorEncodingVars.hueMax);

					}
					float theta = threshold / 100.0f;   // TODO find suitable 
					t_scanline[x] = prob > theta ? 255 : 0;
				}
			}
		}

		cv::Mat close_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5), cv::Point(1, 1));
		cv::morphologyEx(test_mask, test_mask, cv::MORPH_CLOSE, close_element);

		// Convert the 1-channel mask into a 3-channel color image
		cv::Mat tm_color;
		cv::Mat binary;
		cv::cvtColor(test_mask, tm_color, CV_GRAY2BGR);

		// Height of the canvas	
		const int canvas_height = gesture_recorder.canvas.rows;

		// Draw the recorded gesture onto the mask color image	
		for (size_t p = 1; p < gesture_recorder.positions.size(); ++p)
		{
			const auto p1 = gesture_recorder.positions[p - 1];
			const auto p2 = gesture_recorder.positions[p];

			// We subtract the canvas height because the recorded [x,y]-positions are relative to the window, not the canvas	
			cv::line(tm_color, cv::Point2d(p1.x, p1.y - canvas_height), cv::Point2d(p2.x, p2.y - canvas_height), Color_RED, 2, CV_AA);
		}

		vector<vector<cv::Point> > contours;
		vector < vector<int> > hullI = vector<vector<int> >(1);
		vector < vector<cv::Point> > hullPoints = vector<vector<cv::Point> >(1);
		vector<cv::Vec4i> defects;

		blur(test_mask, test_mask, cv::Size(3, 3));
		cv::threshold(test_mask, binary, 150, 255, CV_THRESH_BINARY);
		findContours(binary, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

		//find the largest contour and store its index
		int largestIdx = -1;
		int largestSize = 0;
		for (int i = 0; i < contours.size(); i++)
		{
			double size = contours[i].size();
			if (size > largestSize)
			{
				largestSize = size;
				largestIdx = i;
			}
		}

		int nFingers = 1;

		if (largestIdx != -1) {
			drawContours(tm_color, contours, largestIdx, cv::Scalar(255, 255, 0));

			// get bounding rectangle
			cv::Rect boundingBox = cv::boundingRect(cv::Mat(contours[largestIdx]));
			float maxFingerLength = boundingBox.height / 2;
			float minFingerLength = boundingBox.height / 6;

			convexHull(contours[largestIdx], hullI[0]);
			convexHull(contours[largestIdx], hullPoints[0]);
			drawContours(tm_color, hullPoints, 0, cv::Scalar(0, 255, 255));
			cv::rectangle(tm_color, boundingBox, cv::Scalar(100, 100, 0), 1);

			if (hullI[0].size() > 2) {
				convexityDefects(contours[largestIdx], hullI[0], defects);

				for (cv::Vec4i defect : defects) {
					cv::line(tm_color, contours[largestIdx][defect[0]], contours[largestIdx][defect[2]], cv::Scalar(0, 0, 255));
					cv::line(tm_color, contours[largestIdx][defect[1]], contours[largestIdx][defect[2]], cv::Scalar(0, 0, 255));

					float angle = getAngle(contours[largestIdx][defect[0]], contours[largestIdx][defect[2]], contours[largestIdx][defect[1]]);
					float distance1 = getDistance(contours[largestIdx][defect[0]], contours[largestIdx][defect[2]]);
					float distance2 = getDistance(contours[largestIdx][defect[1]], contours[largestIdx][defect[2]]);

					if (angle < 90
						&& distance1 < maxFingerLength && distance2 < maxFingerLength
						&& distance1 > minFingerLength && distance2 > minFingerLength) {
						cv::circle(tm_color, contours[largestIdx][defect[2]], 4, cv::Scalar(180, 180, 0), 3);
						nFingers++;
					}
				}
			}
		}


		calcNFingers(&gesture_recorder, nFingers);
		nFingers = gesture_recorder.nFingers;
		char buff[20];
		sprintf(buff, "N Fingers: %d", nFingers);
		cv::putText(tm_color, buff, cv::Point(8, 24), CV_FONT_NORMAL, 0.75, Color_WHITE, 1, CV_AA);

		// Place the mask color image under the original webcam image
		cv::Mat canvas;
		cv::vconcat(capture_rgb, tm_color, canvas);

		// Handle showing recognized gestures, we should have a last_result and we should not have shown it
		if (gesture_recorder.last_result != nullptr && gesture_recorder.is_result_shown == false)
		{
			// Threshold showing the result to avoid bad recognitions
			if (gesture_recorder.last_result->score > 0.75)
			{
				CVLog(INFO) << gesture_recorder.last_result->name << "\t\t" << gesture_recorder.last_result->score;
				ofstream myfile;
				myfile.open("commands\\water.txt", ios::app);
				if (myfile.is_open()) {
					myfile << gesture_recorder.last_result->name << "\t\t" << gesture_recorder.last_result->score;
				}
				else { CVLog(INFO) << "Unable to open file"; }
				myfile.close();
			}

			// Disarm is_result_shown so we show the result only once
			gesture_recorder.is_result_shown = true;
		}

		// Show the canvas
		cv::imshow("detection result", canvas);

		// Wait for image refresh
		key = cv::waitKey(5);
	}

	return (EXIT_SUCCESS);
}