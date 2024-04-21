//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <vector>
#include <functional>
#include "iris/Configuration.h"
#include <opencv2/core.hpp>

namespace cv
{
	class Mat;
}

namespace iris
{
	struct FlashParams;
	struct CheckTransitionResult;
	struct IrisFrame;

	class Flash
	{
	public:

		/// <param name="fps"></param>
		/// <param name="flashParams">struct with config parameters</param>
		/// <param name="method">method to use to check the flashing area</param>
		Flash(short fps, const cv::Size& frameSize, FlashParams* flashParams);

		virtual ~Flash();

		struct CheckTransitionResult
		{
			bool checkResult = false;
			float lastAvgDiffAcc = 0;

			void operator = (const CheckTransitionResult& data)
			{
				checkResult = data.checkResult;
				lastAvgDiffAcc = data.lastAvgDiffAcc;
			}
		};

		/// <summary>
		/// Calculates the difference between two consecutive frames
		/// </summary>
		/// <returns>difference values</returns>
		cv::Mat* FrameDifference();

		/// <summary>
		/// Change the current frame and move the previous one to last frame, to be ready for the next calculation
		/// </summary>
		/// <param name="sRgbFrame">The new frame with the calculated flash values</param>
		void virtual SetCurrentFrame(cv::Mat* flashValuesFrame);
		
		void virtual SetCurrentFrame(const IrisFrame& irisFrame) {};

		/// <summary>
		/// Checks if there has been enough variation from one frame to the next, if there is, the
		/// positive and negative averages are calculated (to ensure only positive/negative values are
		/// used to calculate the average, a threshold operation is used to separate the values)
		/// </summary>
		/// <param name="frameDifference">difference of flash values as frame(n) - frame(n-1)</param>
		/// <returns>average frame difference</returns>
		float CheckSafeArea(cv::Mat* frameDifference);

		/// <summary>
		/// Accumulates the average difference and returns true if a new transition is detected
		/// </summary>
		/// <param name="avgDiff">average difference of last two frames</param>
		/// <param name="lastAvgDiffAcc">accumulated average difference</param>
		/// <returns></returns>
		CheckTransitionResult CheckTransition(float avgDiff, float lastAvgDiffAcc);

		/// <summary>
		/// Calculates the average frame luminance
		/// </summary>
		/// <returns>average frame</returns>
		float FrameMean();
		inline float GetFrameMean() { return m_avgCurrentFrame; }

		float GetFlashArea() { return m_flashArea;  }


		cv::Mat* getCurrentFrame() {
			return currentFrame;
		}
		
		/// <summary>
		/// 
		/// </summary>
		/// <param name="value"></param>
		/// <param name="prec"></param>
		/// <returns></returns>
		static float roundoff(float value, unsigned char prec);

		/// <summary>
		/// Calculates sRGB values in the log file to later
		/// This method is not called as the values are stored in a json file and the conversion is done with a look up table for performance
		/// </summary>
		void CalculateSrgbValues();

		void ReleaseLastFrame();
		
		void ReleaseCurrentFrame();
		
	protected:

		/// <summary>
		/// Determines if a transition has occurred 
		/// Returns true if a new transition occurrs
		/// </summary>
		/// <param name="lastAvgDiffAcc">last accumulated average difference</param>
		/// <param name="avgDiffAcc">new accumulated average difference</param>
		bool IsFlashTransition(const float& lastAvgDiffAcc, const float& avgDiffAcc, const float& threshold);

		cv::Mat* lastFrame = nullptr;
		cv::Mat* currentFrame = nullptr;

	private:
		
		/// Returns true if both numbers are positive or negative
		/// 0 is both positive and negative as it means flash trend has not changed
		inline bool SameSign(float num1, float num2)
		{
			return (num1 <= 0 && num2 <= 0) || (num1 >= 0 && num2 >= 0);
		}

		FlashParams* m_params;
		std::vector<float> m_avgDiffInSecond; //all avg diff values in the current second (by increase or decrease)
		int m_safeArea = 0; //area size in pixels that, if surpassed, indicates a transition may have occurred  
		static short fps;

		float m_avgCurrentFrame = 0;
		float m_avgLastFrame = 0;
		float m_flashArea = 0;
		int m_frameSize = 0; //frame width * frame height
	};

}