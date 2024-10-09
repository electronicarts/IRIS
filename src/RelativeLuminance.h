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
		void SetCurrentFrame(cv::Mat* bgrFrame) override;
		
		~RelativeLuminance();
	protected:

		struct ConvertToRelativeLuminance 
		{
			ConvertToRelativeLuminance(cv::Mat* luminanceMat) { luminance = luminanceMat; };
			cv::Mat* luminance = nullptr;

			void operator()(cv::Vec3d& pixel, const int* position) const
			{
				//Y = 0.0722 * B + 0.7152 * G + 0.2126 * R where B, G and R
				luminance->ptr<double>(position[0])[position[1]] = rgbValues[0] * pixel[0] + rgbValues[1] * pixel[1] + rgbValues[2] * pixel[2];
			}
		};

	private:
		static cv::Scalar rgbValues;

	};
}