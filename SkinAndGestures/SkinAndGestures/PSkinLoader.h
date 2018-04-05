#include <string>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include "Utilities.h"
#include "stdafx.h"


using namespace std;

namespace DollarRecognizer
{
	class PSkinLoader
	{
	private:
		const std::string path_images = "C:/Face_Dataset/Photos";
		const std::string path_masks = "C:/Face_Dataset/masks";

		std::vector<std::string> files_images;
		std::vector<std::string> files_masks;
		
		// 2 histograms: one for counting skin pixel colors, the other for counting non-skin pixel colors
		cv::Mat sum_hist_skin, sum_hist_nonskin;
		int bin_size;
		int hist_size[3];
		// Factor to scale a color number to a histogram bin
		double factor;

		
		void init();
		cv::Mat set_P_skin(bool isYCrCb);
		cv::Mat convertToPSkin(cv::Mat sum_hist_skin, cv::Mat sum_hist_nonskin);

	public:
		cv::Mat p_skin_YCrCb;
		cv::Mat p_skin_HSV;

		PSkinLoader(int bin_size);
		const float getp_skin_YCrCb(const uchar* value, int redModifier, int blueModifier);
		const float getp_skin_HSV(const uchar* value, int hueMin, int hueMax);
	};
}