//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

// Implements a helper class to convert the values of a Matrix into other 
// values (e.g. color conversions, luminance) using a Loop Up Table

#pragma once
#include <vector>

namespace cv
{
	class Mat;
}

namespace EA::EACC::Utils
{
	struct FrameConverterParams
	{
		FrameConverterParams(std::vector<float> values) : values(values) {};
		std::vector<float> values; //The input array containing the values for the convertion
	};

	class FrameConverter
	{
	public:
		/// <summary>
		/// Create an instance of the Rgb converter
		/// </summary>
		/// <param name="sRgbValues">The array of decimal values for Bgr to sRgb convertion</param>
		FrameConverter(std::vector<float>& c);

		/// <summary>
		/// Create an instance of the Rgb converter
		/// </summary>
		/// <param name="params">A FrameRgbConverterParams struct containing the vector of decimal values for Bgr to sRgb convertion</param>
		FrameConverter(FrameConverterParams* params);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="mat"></param>
		/// <returns></returns>
		cv::Mat* Convert(cv::Mat& mat);

		/// <summary>
		/// Returns current values to convert to
		/// </summary>
		/// <returns></returns>
		cv::Mat* GetTable()
		{
			return m_pMatValues;
		};

		/// <summary>
		/// Destructor for frame converter
		/// </summary>
		~FrameConverter();

	private:

		/// <summary>
		/// The input array containing the values for the mat values (e.g. color space) convertion
		/// </summary>
		cv::Mat* m_pMatValues = nullptr;

		FrameConverterParams* m_params = nullptr; //config params
	};
}