//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#pragma once

#include "Flash.h"
#include "opencv2/core/types.hpp"

namespace cv
{
	class Mat;
}

namespace iris
{
	struct FlashParams;

	class RedSaturation : public Flash
	{
	public:

		RedSaturation(short videoFPS, const cv::Size& frameSize, FlashParams* params);
		~RedSaturation();

		void SetCurrentFrame(cv::Mat* sRgbFrame) override;

	private:

		struct CalculateRedSaturation
		{
			CalculateRedSaturation(cv::Mat* redSaturationMat) { redSat = redSaturationMat; };
			cv::Mat* redSat = nullptr;

			void operator()(cv::Vec3f& pixel, const int* position) const
			{
				//if R / (R + G + B) >= 0.8 = > pixel is saturated red
				if (pixel[2] / (pixel[2] + pixel[1] + pixel[0]) >= 0.8f)
				{
					//if (R - G - B) * 320 > 20 => value to check for new transitions with frame diff
					float red = (pixel[2] - pixel[1] - pixel[0]) * 320;
					
					//if negative set to 0 (default value is already 0)
					if (red > 0)
					{
						redSat->ptr<float>(position[0])[position[1]] = red;
					}

				} //else pixel is not red saturated (default value is already 0)
			}
		};

		/// <summary>
		/// Calculates the red saturation in all the pixels of the frame
		/// if R / (R + G + B) >= 0.8 => pixel is saturated red
		/// </summary>
		/// <param name="channels">sRGB frame channel mats</param>
		//cv::Mat* RedSaturationValue(cv::Mat channels[]);

		/// <summary>
		/// Calculates the red saturation coefficient (flash values) of the video frame in sRGB
		/// </summary>
		/// <param name="channels">sRGB frame channel mats</param>
		//void RedSatCoefficient(cv::Mat* frame, cv::Mat channels[], cv::Mat* redMask);

		FlashParams* m_params = nullptr; //struct with config params
	};
}
