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

	public:
		ImageProcessor(cv::Mat* canvas);
		void getSkinProbabilities(cv::Mat * test_mask, PSkinLoader* pSkinLoader, colorEncodingVariables colorEncodingVars, int useYCrCb);
	};
}
#pragma once
