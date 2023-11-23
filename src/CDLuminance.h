//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "Flash.h"
#include "opencv2/core/types.hpp"

namespace cv
{
	class Mat;
}

namespace EA::EACC::Utils
{
	class FrameConverter;
}


namespace iris
{
	struct FlashParams;
	struct IrisFrame;

	class CDLuminance : public Flash
	{
	public:
		/// <summary>
		/// 
		/// </summary>
		/// <param name="fps"></param>
		/// <param name="flashThreshold"></param>
		CDLuminance(EA::EACC::Utils::FrameConverter* converter, const short& fps, const cv::Size& frameSize, FlashParams* params);
		void SetCurrentFrame(const IrisFrame& irisFrame) override;
		void SetCurrentFrame(cv::Mat* bgrFrame);

		~CDLuminance();
	protected:
	private:
		EA::EACC::Utils::FrameConverter* m_converter;
	};
}