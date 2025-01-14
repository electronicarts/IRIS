//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include "RedSaturation.h"
#include <opencv2/core.hpp>
#include "opencv2/imgproc.hpp"
#include "ConfigurationParams.h"

namespace iris
{
	RedSaturation::RedSaturation(short fps, const cv::Size& frameSize, FlashParams* params, IFrameManager* frameManager)
		: Flash(fps, frameSize, params, frameManager)
	{

	}

	RedSaturation::~RedSaturation()
	{
		ReleaseLastFrame();
		ReleaseCurrentFrame();
	}

	void RedSaturation::SetCurrentFrame(cv::Mat* sRgbFrame)
	{
		cv::Mat* frame = new cv::Mat(sRgbFrame->size(), CV_32FC1, cv::Scalar(0));
		sRgbFrame->forEach<cv::Vec3f>(CalculateRedSaturation(frame));

		Flash::ReleaseLastFrame();
		Flash::SetCurrentFrame(frame);
	}

	//// if R / (R + G + B) >= 0.8 => pixel is saturated red
	//cv::Mat* RedSaturation::RedSaturationValue(cv::Mat channels[])
	//{
	//	cv::Mat* redMask = new cv::Mat();

	//	//cv::transform(*sRgbFrame, *redMask, cv::Matx13f(1, 1, 1));
	//	cv::add(channels[0], channels[1], *redMask);
	//	cv::add(channels[2], *redMask, *redMask);
	//	cv::divide(channels[2], *redMask, *redMask);

	//	//if pixel >= 0.8 => pixel = 255 else pixel = 0
	//	cv::threshold(*redMask, *redMask, 0.8f, 255, cv::THRESH_BINARY);
	//	redMask->convertTo(*redMask, CV_8UC1);

	//	return redMask;
	//}

	//void RedSaturation::RedSatCoefficient(cv::Mat* frame, cv::Mat channels[], cv::Mat* redMask)
	//{
	//	//if (R - G - B) * 320 > 20 => value to check for new transitions with frame diff
	//	cv::subtract(channels[2], channels[1], *frame, *redMask);
	//	cv::subtract(*frame, channels[0], *frame, *redMask);
	//	cv::multiply(*frame, 320, *frame);

	//	//Negative values are set to 0
	//	cv::threshold(*frame, *frame, 0, 320, cv::THRESH_TOZERO);
	//}
}