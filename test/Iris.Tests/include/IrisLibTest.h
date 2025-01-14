//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#pragma once

#include <gtest/gtest.h>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "iris/Configuration.h"
#include <iris/Log.h>
#include "iris/FrameData.h"

//redefine logging macros
#ifdef LOG_CORE_DEBUG

#undef LOG_CORE_DEBUG
#undef LOG_CORE_TRACE
#undef LOG_CORE_INFO
#undef LOG_CORE_ERROR
#undef LOG_CORE_WARNING
#undef LOG_CORE_CRITICAL
#undef LOG_DATA_INFO

#define LOG_CORE_DEBUG
#define LOG_CORE_TRACE
#define LOG_CORE_INFO
#define LOG_CORE_ERROR
#define LOG_CORE_WARNING
#define LOG_CORE_CRITICAL
#define LOG_DATA_INFO

#endif // !LOG_CORE_DEBUG



namespace iris::Tests
{
	static cv::Scalar white(255, 255, 255);
	static cv::Scalar gray(128, 128, 128);
	static cv::Scalar black(0, 0, 0);
	static cv::Scalar blue(255, 0, 0);
	static cv::Scalar red(0, 0, 255);

	/// <summary>
	/// Auxiliar method to display test images
	/// </summary>
	/// <param name="mat">image to display</param>
	/// <param name="w">window name</param>
	static void Display(cv::Mat& mat, std::string w = "Test")
	{
		cv::imshow(w, mat);
		cv::waitKey();
		cv::destroyAllWindows();
	}

	//auxiliar method to get canny edge detection lines from image
	static cv::Mat* GetCanny(const char* imageFile, int cannyTh1, int cannyTh2)
	{
		cv::Mat image = cv::imread(imageFile);
		cv::cvtColor(image, image, cv::ColorConversionCodes::COLOR_BGR2GRAY);
		cv::medianBlur(image, image, 5);

		cv::Mat* canny = new cv::Mat();
		cv::Canny(image, *canny, cannyTh1, cannyTh2);

		return canny;
	}

	static bool CompareFloat(const float& a, const float& b, const float& errorMargin = 0.0001f)
	{
		return fabs(a - b) <= errorMargin;
	}

	class IrisLibTest : public ::testing::Test {
	protected:
		Configuration configuration; 

		void SetUp() override {
			cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_ERROR);
			
			iris::Log::Init(false, false);

			//Load configuration
			configuration.Init();
		}
	};
}