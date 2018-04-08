#include <string>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include "Utilities.h"
#include "PSkinLoader.h"
#include "stdafx.h"
#include "ColorEncodingVars.h"

namespace DollarRecognizer
{
	class ImageProcessor {
	private:
		cv::Mat* canvas;
		cv::Mat mask_binary;

		vector<vector<cv::Point> > contours;
		vector < vector<int> > hullI = vector<vector<int> >(1);
		vector < vector<cv::Point> > hullPoints = vector<vector<cv::Point> >(1);
		vector<cv::Vec4i> defects;
		float getDistance(cv::Point a, cv::Point b);
		float getAngle(cv::Point s, cv::Point f, cv::Point e);


	public:
		cv::Mat mask_color;
		ImageProcessor(cv::Mat* canvas);
		void getSkinProbabilities(cv::Mat * test_mask, PSkinLoader* pSkinLoader, colorEncodingVariables colorEncodingVars, int useYCrCb);
		void makeMask(cv::Mat * test_mask);
		void makeBinaryImage(cv::Mat * test_mask);
		int process();
	};
}
#pragma once
