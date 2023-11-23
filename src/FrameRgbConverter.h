//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

/////////////////////////////////////////////////////////////////////////////
// Implements a small wrapper of a logging library to use logging in C++ projects.
// To use ILog, implement a Log class that inherits from ILog and define the sinks
// needed for the loggers.
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <vector>

namespace cv
{
	class Mat;
}

namespace iris
{
	struct FrameRgbConverterParams;
	struct IrisFrame;

	class FrameRgbConverter
	{
	public:
		/// <summary>
		/// Create an instance of the Rgb converter
		/// </summary>
		/// <param name="sRgbValues">The array of decimal values for Bgr to sRgb convertion</param>
		FrameRgbConverter(std::vector<float>& c);

		/// <summary>
		/// Create an instance of the Rgb converter
		/// </summary>
		/// <param name="params">A FrameRgbConverterParams struct containing the vector of decimal values for Bgr to sRgb convertion</param>
		FrameRgbConverter(FrameRgbConverterParams* params);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="mat"></param>
		/// <returns></returns>
		cv::Mat* ConvertBgrFrameToSrgb(cv::Mat& mat);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		cv::Mat* GetSrgbTable()
		{
			return m_pMatSrgbValues;
		};

		/// <summary>
		/// Destructor for Rgb converter
		/// </summary>
		~FrameRgbConverter();
	private:

		/// <summary>
		/// The input array containing the decimal values for the sRgb convertion
		/// </summary>
		cv::Mat* m_pMatSrgbValues = nullptr;

		FrameRgbConverterParams* params = nullptr; //config params
	};
}