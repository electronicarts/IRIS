//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include "utils/FrameConverter.h"
#include <opencv2/core.hpp>

namespace EA::EACC::Utils
{
	FrameConverter::FrameConverter(std::vector<float>& sRgbValues)
	{
		m_pMatValues = new cv::Mat(sRgbValues, true);
	}

	FrameConverter::FrameConverter(FrameConverterParams* params) : m_params(params)
	{
		m_pMatValues = new cv::Mat(params->values, true);
	}

	FrameConverter::~FrameConverter()
	{
		if (m_pMatValues != nullptr) {
			m_pMatValues->release();
			delete m_pMatValues;
		}
	}

	cv::Mat* FrameConverter::Convert(cv::Mat& bgrMat)
	{
		cv::Mat* sRgbMat = new cv::Mat();
		cv::LUT(bgrMat, *m_pMatValues, *sRgbMat);
		return sRgbMat;
	}
}