//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include <gtest/gtest.h>
#include  "utils/FrameConverter.h"
#include "CDLuminance.h"
#include "IrisLibTest.h"
#include "IrisFrame.h"
#include <opencv2/opencv.hpp>

namespace iris::Tests
{
	class CDLuminanceTest : public IrisLibTest {
	protected:
		EA::EACC::Utils::FrameConverter* frameRgbConverter = nullptr;
		void SetUp() override {
			configuration.SetLuminanceType(Configuration::LuminanceType::CD);
			IrisLibTest::SetUp();
			frameRgbConverter = new EA::EACC::Utils::FrameConverter(configuration.GetFrameCDLuminanceConverterParams());
		}
		~CDLuminanceTest() override {
			delete frameRgbConverter;
		}
	};

	TEST_F(CDLuminanceTest, Luminance_When_WhiteFrame_Test)
	{
		cv::Size size(1280, 720);
		iris::CDLuminance cdLuminance(frameRgbConverter, 3, size, configuration.GetLuminanceFlashParams());
		cv::Mat imageBgr(size, CV_8UC3, white);
		
		cdLuminance.SetCurrentFrame(&imageBgr);
		cv::Mat* luminance = cdLuminance.getCurrentFrame();
		float testLum = luminance->at<float>(0, 0);
		EXPECT_EQ(200, testLum);

		imageBgr.release();
	}

	TEST_F(CDLuminanceTest, Luminance_When_BlackFrame_Test)
	{
		cv::Size size(1280, 720);
		iris::CDLuminance cdLuminance(frameRgbConverter, 3, size, configuration.GetLuminanceFlashParams());
		cv::Mat imageBgr(size, CV_8UC3, black);

		cdLuminance.SetCurrentFrame(&imageBgr);
		cv::Mat* luminance = cdLuminance.getCurrentFrame();
		float testLum = luminance->at<float>(0, 0);
		EXPECT_EQ(0.07f, testLum);
	}

	TEST_F(CDLuminanceTest, Luminance_When_GrayFrame_Test)
	{
		cv::Size size(1280, 720);
		iris::CDLuminance cdLuminance(frameRgbConverter, 3, size, configuration.GetLuminanceFlashParams());
		cv::Mat imageBgr(size, CV_8UC3, gray);

		cdLuminance.SetCurrentFrame(&imageBgr);
		cv::Mat* luminance = cdLuminance.getCurrentFrame();
		float testLum = luminance->at<float>(0, 0);
		EXPECT_EQ(46.48f, testLum);

		imageBgr.release();
	}

	TEST_F(CDLuminanceTest, Luminance_When_BlueFrame_Test)
	{
		cv::Size size(1280, 720);
		CDLuminance cdLuminance(frameRgbConverter, 3, size, configuration.GetLuminanceFlashParams());
		cv::Mat imageBgr(size, CV_8UC3, blue);

		cdLuminance.SetCurrentFrame(&imageBgr);
		cv::Mat* luminance = cdLuminance.getCurrentFrame();
		float testLum = luminance->at<float>(0, 0);
		EXPECT_EQ(2.53f, testLum);

		imageBgr.release();
	}

	TEST_F(CDLuminanceTest, SafeArea_No_Change_Threshold)
	{
		cv::Size size(5, 5);
		CDLuminance cdLuminance(frameRgbConverter, 3, size, configuration.GetLuminanceFlashParams());

		cv::Mat imageBgr(size, CV_8UC3, blue);
		cdLuminance.SetCurrentFrame(&imageBgr);
		cdLuminance.SetCurrentFrame(&imageBgr);

		cv::Mat* frameDiff = cdLuminance.FrameDifference();
		float avgDifference = cdLuminance.CheckSafeArea(frameDiff);
		EXPECT_EQ(0, avgDifference);

		float flashAreaProportion = cdLuminance.GetFlashArea();
		EXPECT_EQ(0, flashAreaProportion);

		delete frameDiff;
	}

	TEST_F(CDLuminanceTest, SafeArea_20_Percent_Change_Threshold)
	{
		cv::Size size(5, 5);

		float frameDiffArray[5][5] = {
				{ 200, 200, 200, 0, 0 },
				{ 200, 200, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 }
		};
		cv::Mat frameDiff = cv::Mat(size, CV_32FC1, &frameDiffArray);

		CDLuminance cdLuminance(frameRgbConverter, 3, size, configuration.GetLuminanceFlashParams());

		cv::Mat imageBgr(size, CV_8UC3, blue);
		cdLuminance.SetCurrentFrame(&imageBgr);

		cv::Mat imageBgr2(size, CV_8UC3, white);
		cdLuminance.SetCurrentFrame(&imageBgr2);

		float avgDifference = cdLuminance.CheckSafeArea(&frameDiff);
		EXPECT_EQ(0, avgDifference);

		float flashAreaProportion = cdLuminance.GetFlashArea();
		EXPECT_TRUE(CompareFloat(0.2, flashAreaProportion));
	}

	TEST_F(CDLuminanceTest, SafeArea_100_Percent_Change_Threshold)
	{
		cv::Size size(5, 5);
		CDLuminance cdLuminance(frameRgbConverter, 3, size, configuration.GetLuminanceFlashParams());
		
		cv::Mat imageBgr(size, CV_8UC3, blue);
		cdLuminance.SetCurrentFrame(&imageBgr);

		cv::Mat imageBgr2(size, CV_8UC3, white);
		cdLuminance.SetCurrentFrame(&imageBgr2);

		cv::Mat* frameDiff = cdLuminance.FrameDifference();
		float avgDifference = cdLuminance.CheckSafeArea(frameDiff);
		EXPECT_TRUE(CompareFloat(197.47, avgDifference));

		float flashAreaProportion = cdLuminance.GetFlashArea();
		EXPECT_EQ(1, flashAreaProportion);

		delete frameDiff;
	}
}
