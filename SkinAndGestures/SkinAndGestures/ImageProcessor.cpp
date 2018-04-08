#include "stdafx.h"
#include "ImageProcessor.h"

namespace DollarRecognizer
{
	ImageProcessor::ImageProcessor(cv::Mat* _canvas) {
		canvas = _canvas;
	}

	void ImageProcessor::getSkinProbabilities(cv::Mat* test_mask, PSkinLoader* pSkinLoader, colorEncodingVariables colorEncodingVars, int useYCrCb) {
		if (!pSkinLoader->p_skin_YCrCb.empty())
		{
#pragma omp parallel for
			for (int y = 0; y < canvas->rows; ++y)
			{
				const uchar* c_scanline = canvas->ptr(y); // skip to row in the canvas
				uchar* t_scanline = test_mask->ptr(y);                     // skip to row in the mask

				for (int x = 0; x < canvas->cols; ++x)
				{
					// skip to column (3 colors per cell)
					const uchar* value = c_scanline + x * 3;
					float prob;
					if (useYCrCb) {
						prob = pSkinLoader->getp_skin_YCrCb(value, colorEncodingVars.red, colorEncodingVars.blue, colorEncodingVars.minY, colorEncodingVars.maxY);
					}
					else {
						prob = pSkinLoader->getp_skin_HSV(value, colorEncodingVars.hueMin, colorEncodingVars.hueMax);
					}
					float theta = colorEncodingVars.threshold / 100.0f;   // TODO find suitable 
					t_scanline[x] = prob > theta ? 255 : 0;
				}
			}
		}
	}

}