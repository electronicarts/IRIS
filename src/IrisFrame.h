//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <opencv2/core.hpp>
#include "iris/FrameData.h"

namespace iris
{
	struct IrisFrame
	{
		IrisFrame() {};
		IrisFrame(cv::Mat* originalFrame, FrameData frameData) : originalFrame(originalFrame), frameData(frameData) {};
		IrisFrame(cv::Mat* originalFrame, cv::Mat* sRgbFrame, FrameData frameData) : originalFrame(originalFrame), sRgbFrame(sRgbFrame), frameData(frameData) {};
		
		void Release() 
		{
			//Release only sRgbFrame as originalFrame is a reference, no malloc is done for it
			if (sRgbFrame != nullptr)
			{
				delete sRgbFrame;  sRgbFrame = nullptr;
			}
		}

		cv::Mat* originalFrame = nullptr; //Video frame in BGR color space
		cv::Mat* sRgbFrame = nullptr; //Converted video frame to sRGB color space
		cv::Mat* luminanceFrame = nullptr; //Converted video frame to luminance 
		FrameData frameData; //Frame info
	};
}