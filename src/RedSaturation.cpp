//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include "RedSaturation.h"
#include <opencv2/core.hpp>
#include "opencv2/imgproc.hpp"
#include "ConfigurationParams.h"

namespace iris
{
	RedSaturation::RedSaturation(short fps, const cv::Size& frameSize, FlashParams* params)
		: Flash(fps, frameSize, params)
	{

	}

	RedSaturation::~RedSaturation()
	{
		ReleaseLastFrame();
		ReleaseCurrentFrame();
	}

	void RedSaturation::SetCurrentFrame(cv::Mat* sRgbFrame)
	{
		cv::Mat* frame = new cv::Mat(sRgbFrame->size(), CV_64FC1, cv::Scalar(0));
		sRgbFrame->forEach<cv::Vec3d>(CalculateRedSaturation(frame));

		Flash::ReleaseLastFrame();
		Flash::SetCurrentFrame(frame);
	}
}