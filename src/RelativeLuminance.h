//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "Flash.h"
#include <opencv2/core/types.hpp>

namespace cv
{
	class Mat;
}

namespace iris
{
	struct FlashParams;
	struct IrisFrame;
	
	class RelativeLuminance : public Flash
	{
	public:
		/// <summary>
		/// 
		/// </summary>
		/// <param name="fps"></param>
		/// <param name="flashThreshold"></param>
		RelativeLuminance(short fps, const cv::Size& frameSize, FlashParams* params);


		void SetCurrentFrame(const IrisFrame& irisFrame) override;
		void SetCurrentFrame(cv::Mat* bgrFrame);
		
		~RelativeLuminance();
	protected:
		static cv::Scalar rgbValues;

		struct ConvertToRelativeLuminance 
		{
			ConvertToRelativeLuminance(cv::Mat* luminanceMat) { luminance = luminanceMat; };
			cv::Mat* luminance = nullptr;

			void operator()(cv::Vec3f& pixel, const int* position) const
			{
				//Y = 0.0722 * B + 0.7152 * G + 0.2126 * R where B, G and R
				luminance->ptr<float>(position[0])[position[1]] = 0.0722f * pixel[0] + 0.7152f * pixel[1] + 0.2126f * pixel[2];
			}
		};

	private:
	};
}