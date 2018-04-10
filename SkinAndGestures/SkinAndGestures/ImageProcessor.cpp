#include "stdafx.h"
#include "ImageProcessor.h"

namespace DollarRecognizer
{
	ImageProcessor::ImageProcessor(cv::Mat* _canvas) {
		canvas = _canvas;
	}

	void ImageProcessor::getSkinProbabilities(cv::Mat* test_mask, PSkinLoader* pSkinLoader, colorEncodingVariables colorEncodingVars, bool useYCrCb) {
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

	void ImageProcessor::makeMask(cv::Mat* test_mask) {
		cv::Mat close_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5), cv::Point(1, 1));
		cv::morphologyEx(*test_mask, *test_mask, cv::MORPH_CLOSE, close_element);

		// Convert the 1-channel mask into a 3-channel color image
		cv::cvtColor(*test_mask, mask_color, CV_GRAY2BGR);
	}

	void ImageProcessor::makeBinaryImage(cv::Mat* test_mask) {
		blur(*test_mask, *test_mask, cv::Size(3, 3));
		cv::threshold(*test_mask, mask_binary, 150, 255, CV_THRESH_BINARY);
	}

	int ImageProcessor::process() {
		findContours(mask_binary, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

		//find the largest contour and store its index
		int largestIdx = getLargestContourIndex();

		int nFingers = 0;

		// Draw convex hull and convexity defects, calculate number of fingers held up.
		if (largestIdx != -1) {
			drawContours(mask_color, contours, largestIdx, cv::Scalar(255, 255, 0));

			// get bounding rectangle
			cv::Rect boundingBox = cv::boundingRect(cv::Mat(contours[largestIdx]));
			float maxFingerLength = boundingBox.height / 1.5f;
			float minFingerLength = boundingBox.height / 8;

			convexHull(contours[largestIdx], hullI[0]);
			convexHull(contours[largestIdx], hullPoints[0]);
			drawContours(mask_color, hullPoints, 0, cv::Scalar(0, 255, 255));
			cv::rectangle(mask_color, boundingBox, cv::Scalar(100, 100, 0), 1);

			if (hullI[0].size() > 2) {
				convexityDefects(contours[largestIdx], hullI[0], defects);

				for (cv::Vec4i defect : defects) {

					cv::Point farPt = contours[largestIdx][defect[2]];
					cv::Point start = contours[largestIdx][defect[0]];
					cv::Point end = contours[largestIdx][defect[1]];
					
					// Draw defects
					cv::line(mask_color, start, farPt, cv::Scalar(0, 0, 255));
					cv::line(mask_color, end, farPt, cv::Scalar(0, 0, 255));

					float angle = getAngle(start, farPt, end);
					float distance1 = getDistance(start, farPt);
					float distance2 = getDistance(end, farPt);

					cv::Point x1 = cv::Point(start.x, farPt.y);
					float angleWithX1 = getAngle(start, farPt, x1);

					cv::Point x2 = cv::Point(end.x, farPt.y);
					float angleWithX2 = getAngle(end, farPt, x2);
					
					float anglewithx = (angleWithX1 + angleWithX2) / 2;

					// Check if this defect is within range to be a defect between two fingers.
					if (angle < 90
						&& angleWithX1 > 30 && anglewithx < 150
						&& distance1 < maxFingerLength && distance2 < maxFingerLength
						&& distance1 > minFingerLength && distance2 > minFingerLength
						&& farPt.y > boundingBox.height/3 + boundingBox.y - boundingBox.height ) 
					{
						cv::circle(mask_color, farPt, 4, cv::Scalar(180, 180, 0), 3);
						nFingers++;
					}
				}
			}
		}
		// 1 angle between fingers means 2 fingers held up
		if (nFingers > 0) {
			nFingers++;
		}
		return nFingers;
	}

	float ImageProcessor::getDistance(cv::Point a, cv::Point b) {
		return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
	}

	float ImageProcessor::getAngle(cv::Point s, cv::Point f, cv::Point e) {
		float l1 = getDistance(f, s);
		float l2 = getDistance(f, e);
		float rad = acos((s - f).dot(e - f) / (l1*l2));
		return rad * 180 / CV_PI;
	}

	int ImageProcessor::getLargestContourIndex() {
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
		return largestIdx;
	}
}