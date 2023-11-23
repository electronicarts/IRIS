//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

/*
**** FramesRgbConverter ****
This class converts a frame - an openCV's Mat object - from 8 bits RGB (0 - 255) to sRGB (decimal values from 0 to 1) 
*/

#include "FrameRgbConverter.h"
#include <opencv2/core.hpp>
#include "ConfigurationParams.h"

namespace iris
{
	FrameRgbConverter::FrameRgbConverter(std::vector<float> &sRgbValues)
	{
		m_pMatSrgbValues = new cv::Mat(sRgbValues, true);
	}

	FrameRgbConverter::FrameRgbConverter(FrameRgbConverterParams* params) : params(params)
	{
		m_pMatSrgbValues = new cv::Mat(params->sRgbValues, true);
	}

	FrameRgbConverter::~FrameRgbConverter()
	{
		if (m_pMatSrgbValues != nullptr) {
			m_pMatSrgbValues->release();
			delete m_pMatSrgbValues;
		}
	}

	cv::Mat* FrameRgbConverter::ConvertBgrFrameToSrgb(cv::Mat& bgrMat)
	{
		cv::Mat* sRgbMat = new cv::Mat();
		cv::LUT(bgrMat, *m_pMatSrgbValues, *sRgbMat);
		return sRgbMat;
	}
}

