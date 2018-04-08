#include "stdafx.h"
#include "PSkinLoader.h"

namespace DollarRecognizer
{
	PSkinLoader::PSkinLoader(int _bin_size)
	{
		Utilities::getDirectory(path_images, "jpg", files_images);
		Utilities::getDirectory(path_masks, "png", files_masks);

		CVLog(INFO) << files_images;
		CVLog(INFO) << files_masks;

		bin_size = _bin_size;
		factor = bin_size / 256.0;
		for (int i = 0; i < 3; ++i) {
			hist_size[i] = _bin_size;
		}
		init();
	}

	void PSkinLoader::init() {
		p_skin_YCrCb = set_P_skin(true);
		p_skin_HSV = set_P_skin(false);
	}

	const float PSkinLoader::getp_skin_YCrCb(const uchar* value, int redModifier, int blueModifier, int minY, int maxY) {
		if (value[0] < minY || value[0] > maxY) {
			return 0;
		}
		// encode the each color channel as a position in the probabilistic look-up histogram
		const int loc[2] = { cvFloor(value[1] * factor * (redModifier / 100.0f)), cvFloor(value[2] * factor * (blueModifier / 100.0f)) };

		// read the probability a given color is a skin color
		return p_skin_YCrCb.at<float>(loc);
	}

	const float PSkinLoader::getp_skin_HSV(const uchar* value, int hueMin, int hueMax) {
		// encode the each color channel as a position in the probabilistic look-up histogram
		const int loc[2] = { cvFloor(value[0] * factor), cvFloor(value[1] * factor) };// , cvFloor(value[1] * factor), cvFloor(value[2] * factor)};

		// read the probability a given color is a skin color
		float prob = p_skin_HSV.at<float>(loc);
		int hue = value[0];
		if (hue < hueMin || hue > hueMax) {
			prob = 0;
		}
		return prob;
	}

	cv::Mat PSkinLoader::set_P_skin(bool isYCrCb) {
		sum_hist_skin = cv::Mat::zeros(3, hist_size, CV_32F);     // Construct the skin histogram
		sum_hist_nonskin = cv::Mat::zeros(3, hist_size, CV_32F);  // Construct the non-skin histogram

		const float color_range[2] = { 0,  256 };                            // Color range
		const float *ranges[3] = { color_range, color_range, color_range }; // Pointer to 3D array, range for every color channel
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
				cv::Mat image_converted;

				cv::Mat mask = cv::imread(*ma_it, CV_LOAD_IMAGE_GRAYSCALE);             // load training mask
				CV_Assert(image.rows == mask.rows && image.cols == mask.cols);          // They should be equal in 

				cv::Mat inverseMask;
				cv::bitwise_not(mask, inverseMask);

				if (isYCrCb) {
					// Use only Cr and Cb channels for YCrCb
					int channels[3] = { 1, 2 };                                // Channels to work with
					cv::cvtColor(image, image_converted, cv::ColorConversionCodes::COLOR_RGB2YCrCb);
					cv::calcHist(&image_converted, 1, channels, mask, sum_hist_skin, 2, hist_size, ranges, true, true);
					cv::calcHist(&image_converted, 1, channels, inverseMask, sum_hist_nonskin, 2, hist_size, ranges, true, true);
				}
				else {
					// Use first 2 channels of for HSV
					int channels[2] = { 0, 1 };                                // Channels to work with
					cv::cvtColor(image, image_converted, cv::ColorConversionCodes::COLOR_RGB2HSV);
					cv::calcHist(&image_converted, 1, channels, mask, sum_hist_skin, 2, hist_size, ranges, true, true);
					cv::calcHist(&image_converted, 1, channels, inverseMask, sum_hist_nonskin, 2, hist_size, ranges, true, true);
				}
					
			}
		}
		return convertToPSkin(sum_hist_skin, sum_hist_nonskin);
	}

	cv::Mat PSkinLoader::convertToPSkin(cv::Mat sum_hist_skin, cv::Mat sum_hist_nonskin)
	{
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
}