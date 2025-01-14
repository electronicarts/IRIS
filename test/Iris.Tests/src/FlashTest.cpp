//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include "IrisLibTest.h"
#include <gtest/gtest.h>
#include "utils/FrameConverter.h"
#include "Flash.h"
#include <opencv2/opencv.hpp>
#include "FpsFrameManager.h"

namespace iris::Tests
{
	class FlashTest : public IrisLibTest {
	protected:
		EA::EACC::Utils::FrameConverter* frameRgbConverter;
		void SetUp() override {
			IrisLibTest::SetUp();
			frameRgbConverter = new EA::EACC::Utils::FrameConverter(configuration.GetFrameSrgbConverterParams());
		}
		~FlashTest() override {
			delete frameRgbConverter;
		}
	};

	TEST_F(FlashTest, Transition_Over_Dark_Threshold)
	{
		cv::Size size(1, 1);
		FpsFrameManager frameManager{};

		Flash flash(5, size, configuration.GetLuminanceFlashParams(), &frameManager);

		cv::Mat frame(size, CV_32FC1, cv::Scalar(0.8f));
		cv::Mat frame2(size, CV_32FC1, cv::Scalar(1.0f));
		cv::Mat frame3(size, CV_32FC1, cv::Scalar(0.85f));

		flash.SetCurrentFrame(&frame);
		flash.SetCurrentFrame(&frame2);

		Flash::CheckTransitionResult res = flash.CheckTransition(0.2f, 0.0f);
		EXPECT_FALSE(res.checkResult);

		flash.SetCurrentFrame(&frame3);
		flash.CheckTransition(-0.15f, 0.2f);
		EXPECT_FALSE(res.checkResult);
	}

	TEST_F(FlashTest, Transition_Below_Dark_Threshold)
	{
		cv::Size size(1, 1);
		FpsFrameManager frameManager{};
		Flash flash(5, size, configuration.GetLuminanceFlashParams(), &frameManager);

		cv::Mat frame(size, CV_32FC1, cv::Scalar(0.6f));
		cv::Mat frame2(size, CV_32FC1, cv::Scalar(1.0f));
		cv::Mat frame3(size, CV_32FC1, cv::Scalar(0.4f));

		flash.SetCurrentFrame(&frame);
		flash.SetCurrentFrame(&frame2);

		Flash::CheckTransitionResult res = flash.CheckTransition(0.4f, 0.0f);
		EXPECT_TRUE(res.checkResult);

		flash.SetCurrentFrame(&frame3);
		flash.CheckTransition(-0.6f, 0.4f);
		EXPECT_TRUE(res.checkResult);
	}

	//Edge case test: when a flash transition occurrs but the value continues increasing/decreasing, no new transition is occurring
	TEST_F(FlashTest, CheckTransition_Same_Sign_Above_Threshold)
	{
		FpsFrameManager frameManager{};
		cv::Size size(1, 1);
		Flash flash(2, size, configuration.GetLuminanceFlashParams(), &frameManager);

		flash.CheckTransition(0.01f, 0.0f);
		flash.CheckTransition(0.09f, 0.01f);
		Flash::CheckTransitionResult res = flash.CheckTransition(0.02f, 0.1f);

		EXPECT_FALSE(res.checkResult);
	}
}