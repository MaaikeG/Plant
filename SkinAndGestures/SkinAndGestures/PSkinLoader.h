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

	public:
		PSkinLoader(int bin_size);
		cv::Mat getp_skin_rgb();
		cv::Mat convertToPSkin(cv::Mat sum_hist_skin, cv::Mat sum_hist_nonskin);

	};
}