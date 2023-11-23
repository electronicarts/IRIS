//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

/////////////////////////////////////////////////////////////////////////////
// Implements the flash detection flow. The frames of a video are compared
// against the previous one to obtain the luminance or red saturation variation
// in order to check for possible flash transitions. The transitions are
// counted and evaluated at every moment of the video to flag all frames that 
// do not comply with the guidelines and fail.
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "TransitionEvaluator.h"
#include "utils/FrameConverter.h"
#include "opencv2/core/types.hpp"
#include "iris/Configuration.h"
#include "PhotosensitivityDetector.h"

namespace cv
{
	class Mat;
}

namespace iris
{
	class Flash;
	class FrameData;
	struct IrisFrame;
	struct Result;

	class FlashDetection : public PhotosensitivityDetector
	{
	public:
		FlashDetection(Configuration* configuration, const short& fps, const cv::Size& frameSize);
		~FlashDetection();

		/// <summary>
		/// Calculates the current frame's luminance values
		/// </summary>
		/// <param name="irisFrame">struct with video frame, frame in sRgb and current FrameData</param>
		void setLuminance(IrisFrame& irisFrame);

		/// <summary>
		/// Compares the frame against the previous one to detect luminance and red saturation flashes
		/// </summary>
		/// <param name="frame">struct with video frame, frame in sRgb and current FrameData</param>
		/// <param name="framePos">current frame position in video</param>
		/// <param name="data">FrameData to persist</param>
		void checkFrame(const IrisFrame& irisFrame, const int& framePos, FrameData& data) override;
		
		/// <summary>
		/// Returns the flash analysis result
		/// </summary>
		bool isFail() override;

		/// <summary>
		/// Sets the flash detection results for a chunk result object
		/// </summary>
		void setResult(Result& result) override;

		cv::Mat* getLuminanceFrame();

	private:

		/// <summary>
		/// Calculates red and luminance variations and checks for new transitions
		/// </summary>
		/// <param name="framePos">current frame position</param>
		/// <param name="data">FrameData to persist</param>
		void frameDifference(FrameData& data);

		TransitionEvaluator m_transitionEvaluator;
		Flash* m_luminance = nullptr;
		Flash* m_redSaturation = nullptr;
		EA::EACC::Utils::FrameConverter m_sRgbConverter;
		EA::EACC::Utils::FrameConverter* m_cdLuminanceConverter = nullptr;

		float m_lastAvgLumDiffAcc = 0; //first frame has 0 variation
		float m_lastAvgRedDiffAcc = 0; //first frame has 0 variation
		short m_fps = 0;
	};
}