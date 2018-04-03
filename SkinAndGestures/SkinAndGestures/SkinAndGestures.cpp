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

using namespace DollarRecognizer;

/*
The ActiveCanvas is a struct that keeps track of the button status and gestures.

INPUT: active_templates, a list of template names which are to be recognized
 */
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
};

float getDistance(cv::Point a, cv::Point b) {
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

float getAngle(cv::Point s, cv::Point f, cv::Point e) {
	float l1 = getDistance(f, s);
	float l2 = getDistance(f, e);
	float rad = acos((s - f).dot(e - f) / (l1*l2));
	return rad * 180 / CV_PI;
}

/*
The mouse call back routine handles the mouse actions.
On left mouse press it records [x,y] locations into the positions list (a vector).
On left mouse release the recorded [x,y] positions are fed the GeometricRecognizer,
the GeometricRecognizer returns a result which is placed in last_result, finally
positions (the vector) is cleared.

INPUT
  event   : a recorded mouse event type that activated the call back
  x, y    : mouse position in the window
  flags   : mouse callback flags (button status)
  userdata: pointer to the ActiveCanvas
*/
void mouseCallBack(int event, int x, int y, int flags, void* userdata)
{
	ActiveCanvas* frame_canvas = (ActiveCanvas*)userdata;

	// Find out if the x,y position is unique, we don't want to record double locations
	Point2D np(x, y);
	const auto po_it = std::find_if(frame_canvas->positions.begin(), frame_canvas->positions.end(), [np](const Point2D &op) {
		return np.x == op.x && np.y == op.y;
	});

	// If the left button is pressed and the [x,y]-position is unique, record the location
	if (frame_canvas->is_lbutton_down && po_it == frame_canvas->positions.end())
	{
		frame_canvas->positions.emplace_back(Point2D(x, y));
	}

	if (event == cv::EVENT_LBUTTONDOWN) // if the left mouse button is pressed
	{
		frame_canvas->is_lbutton_down = true;
	}
	else if (event == cv::EVENT_LBUTTONUP) // if the left mouse button is released
	{
		// unset is_lbutton_down and "arm" is_result_shown to show the recognition result once
		frame_canvas->is_lbutton_down = false;
		frame_canvas->is_result_shown = false;

		// run the recognizer on the current list of [x,y]-positions (if there are any)
		if (!frame_canvas->positions.empty())
		{
			RecognitionResult result = frame_canvas->recognizer.recognize(frame_canvas->positions);
			frame_canvas->last_result = std::unique_ptr<RecognitionResult>(new RecognitionResult(result));

			// clear the [x,y]-positions, ready to record a new gesture
			frame_canvas->positions.clear();
		}
	}
}

cv::Mat getp_skin_rgb(int bin_size) {
	const std::string path_images = "C:/Face_Dataset/Photos";
	std::vector<std::string> files_images;
	Utilities::getDirectory(path_images, "jpg", files_images);

	const std::string path_masks = "C:/Face_Dataset/masks";
	std::vector<std::string> files_masks;
	Utilities::getDirectory(path_masks, "png", files_masks);

	CVLog(INFO) << files_images;
	CVLog(INFO) << files_masks;

	// 2 histograms: one for counting skin pixel colors, the other for counting non-skin pixel colors
	cv::Mat sum_hist_skin, sum_hist_nonskin;

	int hist_size[3] = { bin_size, bin_size, bin_size };      // Construct a 3D array of bin_size bin sizes
	sum_hist_skin = cv::Mat::zeros(2, hist_size, CV_32F);     // Construct the skin histogram
	sum_hist_nonskin = cv::Mat::zeros(2, hist_size, CV_32F);  // Construct the non-skin histogram

	const float color_range[2] = { 0,  256 };                            // Color range
	const float *ranges[3] = { color_range, color_range, color_range }; // Pointer to 3D array, range for every color channel
	const int channels[3] = { 1, 2 };                                // Channels to work with

																	 // Iterate over all training image filenames
	for (size_t i = 0; i < files_images.size(); ++i)
	{
		const auto image_file = files_images[i];

		// We need to find the correct mask for this image
		const auto ma_it = std::find_if(files_masks.begin(), files_masks.end(), [image_file](const std::string &m)
		{
			return Utilities::getBasename(m) == Utilities::getBasename(image_file);
		});

		// If we find the right mask
		if (ma_it != files_masks.end())
		{
			cv::Mat image = cv::imread(image_file);       // load training image
			cv::Mat image_YCrCb;
			cv::cvtColor(image, image_YCrCb, cv::ColorConversionCodes::COLOR_RGB2YCrCb);

			cv::Mat mask = cv::imread(*ma_it, CV_LOAD_IMAGE_GRAYSCALE);             // load training mask
			CV_Assert(image.rows == mask.rows && image.cols == mask.cols);          // They should be equal in 

			cv::calcHist(&image_YCrCb, 1, channels, mask, sum_hist_skin, 2, hist_size, ranges, true, true);

			cv::Mat inverseMask;
			cv::bitwise_not(mask, inverseMask);
			cv::calcHist(&image_YCrCb, 1, channels, inverseMask, sum_hist_nonskin, 2, hist_size, ranges, true, true);
		}
	}

	float totalSkinPixels = cv::sum(sum_hist_skin)[0];
	float totalNonSkinPixels = cv::sum(sum_hist_nonskin)[0];
	float totalPixels = totalSkinPixels + totalNonSkinPixels;

	float p_skin = totalSkinPixels / totalPixels;
	float p_nonskin = totalNonSkinPixels / totalPixels;

	cv::Mat p_rgb_skin = sum_hist_skin / totalSkinPixels;
	cv::Mat p_rgb_nonskin = sum_hist_nonskin / totalNonSkinPixels;

	return (p_rgb_skin * p_skin) /
		(p_rgb_skin * p_skin + p_rgb_nonskin * p_nonskin);

}

int main()
{
	int bin_size = 32;    // TODO find suitable size
	int threshold = 20;

	cv::Mat Pskin_rgb = getp_skin_rgb(bin_size);
	// Factor to scale a color number to a histogram bin
	const double factor = bin_size / 256.0;

	// Open a window
	cv::namedWindow("detection result", CV_WINDOW_FREERATIO);

	// Open the webcam
	cv::VideoCapture video_capture(0);

	// Set list of gestures to be recognized
	// see http://depts.washington.edu/aimgroup/proj/dollar/
	std::vector<std::string> active_templates = { "Triangle", "X", "Rectangle", "Circle" };
	ActiveCanvas gesture_recorder(active_templates);

	// Create the mouse call back so we can enter gestures with the mouse
	// TODO replace with a routine that records the gesture without the mouse!
	// You should, 1: recognize a hand preparation
	//             2: record [x,y]-positions of the stroke once the preparation was recognized
	//             3: stop recording and process the stroke once the retraction is recognized
	// It is not necessary to do this in a call back routine! If you find it difficult to comprehend this 
	// type of event handling, you can implement the position recording as a function inside the video 
	// loop below. 

	int blue = 100, red = 100;

	cv::setMouseCallback("detection result", mouseCallBack, (void*)&gesture_recorder);
	cv::createTrackbar("threshold", "detection result", &threshold, 100);
	cv::createTrackbar("Blue weight", "detection result", &blue, 200);
	cv::createTrackbar("Red weight", "detection result", &red, 200);

	int key = -1;
	while (key != 27)
	{
		// Grab the webcam image
		cv::Mat capture_rgb;
		video_capture >> capture_rgb;
		cv::cvtColor(capture_rgb, gesture_recorder.canvas, cv::ColorConversionCodes::COLOR_RGB2YCrCb);

		// Initialize an empty mask
		cv::Mat test_mask = cv::Mat::zeros(gesture_recorder.canvas.size(), CV_8U);
		

		if (!Pskin_rgb.empty())
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

					// encode the each color channel as a position in the probabilistic look-up histogram
					const int loc[2] = { cvFloor(value[1] * factor * (red/100.0f)), cvFloor(value[2] * factor * (blue/100.0f)) };
					
					// read the probability a given color is a skin color
					const float prob = Pskin_rgb.at<float>(loc);

					float theta = threshold / 100.0f;   // TODO find suitable 
					t_scanline[x] = prob > theta ? 255 : 0;
				}
			}
		}

		cv::Mat close_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5), cv::Point(1, 1));
		cv::morphologyEx(test_mask, test_mask, cv::MORPH_CLOSE, close_element);

		// Convert the 1-channel mask into a 3-channel color image
		cv::Mat tm_color;
		cv::cvtColor(test_mask, tm_color, CV_GRAY2BGR);

		cv::Mat background, binary, diff;
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

			convexHull(contours[largestIdx], hullI[0]);
			convexHull(contours[largestIdx], hullPoints[0]);
			drawContours(tm_color, hullPoints, 0, cv::Scalar(0, 255, 255));

			if (hullI[0].size() > 2) {
				convexityDefects(contours[largestIdx], hullI[0], defects);

				for (cv::Vec4i defect : defects) {
					cv::line(tm_color, contours[largestIdx][defect[0]], contours[largestIdx][defect[2]], cv::Scalar(0, 0, 255));
					cv::line(tm_color, contours[largestIdx][defect[1]], contours[largestIdx][defect[2]], cv::Scalar(0, 0, 255));

					float angle = getAngle(contours[largestIdx][defect[0]], contours[largestIdx][defect[2]], contours[largestIdx][defect[1]]);

					if (angle > 10 && angle < 90) {
						cv::circle(tm_color, contours[largestIdx][defect[2]], 4, cv::Scalar(0, 255, 0), 2);
						nFingers++;
					}
				}
			}
		}

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