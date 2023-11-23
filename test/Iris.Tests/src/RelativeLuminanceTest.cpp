//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include <gtest/gtest.h>
#include "utils/FrameConverter.h"
#include "IrisLibTest.h"
#include <opencv2/opencv.hpp>
#include "RelativeLuminance.h"
#include "IrisFrame.h"
#include <limits>

namespace iris::Tests
{
	class RelativeLuminanceTest : public IrisLibTest {
	protected:
		EA::EACC::Utils::FrameConverter* frameRgbConverter = nullptr;
		void SetUp() override {
			configuration.SetLuminanceType(Configuration::LuminanceType::RELATIVE);
			IrisLibTest::SetUp();
			frameRgbConverter = new EA::EACC::Utils::FrameConverter(configuration.GetFrameSrgbConverterParams());
		}
		~RelativeLuminanceTest() override {
			delete frameRgbConverter;
		}
	};

	TEST_F(RelativeLuminanceTest, Luminance_When_WhiteFrame_Test)
	{
		cv::Size size(1280, 720);
		RelativeLuminance relativeLuminance(3, size, configuration.GetLuminanceFlashParams());
		cv::Mat imageBgr(size, CV_8UC3, white);
		
		IrisFrame imagesRgb;
		imagesRgb.sRgbFrame = frameRgbConverter->Convert(imageBgr);

		relativeLuminance.SetCurrentFrame(imagesRgb);
		cv::Mat* luminance = relativeLuminance.getCurrentFrame();
		float testLum = luminance->at<float>(0, 0);
		EXPECT_EQ(1, testLum);

		imagesRgb.Release();
	}

	TEST_F(RelativeLuminanceTest, Luminance_WhenBlackFrame_Test)
	{
		cv::Size size(1280, 720);
		RelativeLuminance relativeLuminance(3, size, configuration.GetLuminanceFlashParams());
		cv::Mat imageBgr(size, CV_8UC3, black);
		
		IrisFrame imagesRgb;
		imagesRgb.sRgbFrame = frameRgbConverter->Convert(imageBgr);

		relativeLuminance.SetCurrentFrame(imagesRgb);
		cv::Mat* luminance = relativeLuminance.getCurrentFrame();
		float testLum = luminance->at<float>(0, 0);
		EXPECT_EQ(0, testLum);

		imagesRgb.Release();
	}

	TEST_F(RelativeLuminanceTest, Luminance_When_GrayFrame_Test)
	{
		cv::Size size(1280, 720);
		RelativeLuminance relativeLuminance(3, size, configuration.GetLuminanceFlashParams());
		cv::Mat imageBgr(size, CV_8UC3, gray);

		IrisFrame imagesRgb;
		imagesRgb.sRgbFrame = frameRgbConverter->Convert(imageBgr);

		relativeLuminance.SetCurrentFrame(imagesRgb);
		cv::Mat* luminance = relativeLuminance.getCurrentFrame();
		float testLum = luminance->at<float>(0, 0);
		testLum = RelativeLuminance::roundoff(testLum, 3);
		EXPECT_EQ(0.216f, testLum);
		
		imagesRgb.Release();
	}

	TEST_F(RelativeLuminanceTest, Luminance_When_BlueFrame_Test)
	{
		cv::Size size(1280, 720);
		RelativeLuminance relativeLuminance(3, size, configuration.GetLuminanceFlashParams());
		cv::Mat imageBgr(size, CV_8UC3, blue);
		
		IrisFrame imagesRgb;
		imagesRgb.sRgbFrame = frameRgbConverter->Convert(imageBgr);

		relativeLuminance.SetCurrentFrame(imagesRgb);
		cv::Mat* luminance = relativeLuminance.getCurrentFrame();
		float testLum = luminance->at<float>(0, 0);
		testLum = RelativeLuminance::roundoff(testLum, 3);
		EXPECT_EQ(0.072f, testLum);

		imagesRgb.Release();
	}

	TEST_F(RelativeLuminanceTest, TransitionBlackWhite_FrameDifference_Test)
	{
		cv::Size size(1280, 720);
		RelativeLuminance relativeLuminance(3, size, configuration.GetLuminanceFlashParams());
		cv::Mat imageWhiteBgr(size, CV_8UC3, white);
		IrisFrame pImageWhitesRgb;
		pImageWhitesRgb.sRgbFrame = frameRgbConverter->Convert(imageWhiteBgr);
		cv::Mat imageBlackBgr(size, CV_8UC3, black);
		IrisFrame pImagesBlackRgb;
		pImagesBlackRgb.sRgbFrame = frameRgbConverter->Convert(imageBlackBgr);

		relativeLuminance.SetCurrentFrame(pImagesBlackRgb);
		relativeLuminance.SetCurrentFrame(pImageWhitesRgb);
		cv::Mat* diff = relativeLuminance.FrameDifference();
		float testLum = diff->at<float>(0, 0);
		EXPECT_EQ(1, testLum);

		delete diff;
		pImagesBlackRgb.Release();
		pImageWhitesRgb.Release();
	}

	TEST_F(RelativeLuminanceTest, TransitionWhiteBlack_FrameDifference_Test)
	{
		cv::Size size(1280, 720);
		RelativeLuminance relativeLuminance(3, size, configuration.GetLuminanceFlashParams());
		cv::Mat imageWhiteBgr(size, CV_8UC3, white);
		IrisFrame pImageWhitesRgb;
		pImageWhitesRgb.sRgbFrame = frameRgbConverter->Convert(imageWhiteBgr);
		cv::Mat imageBlackBgr(size, CV_8UC3, black);
		IrisFrame pImagesBlackRgb;
		pImagesBlackRgb.sRgbFrame = frameRgbConverter->Convert(imageBlackBgr);

		relativeLuminance.SetCurrentFrame(pImageWhitesRgb);
		relativeLuminance.SetCurrentFrame(pImagesBlackRgb);
		cv::Mat* diff = relativeLuminance.FrameDifference();
		float testLum = diff->at<float>(0, 0);
		EXPECT_EQ(-1, testLum);

		delete diff;
		pImagesBlackRgb.Release();
		pImageWhitesRgb.Release();
	}

	TEST_F(RelativeLuminanceTest, TransitionWhiteBlackBlack_FrameDifference_Test)
	{
		cv::Size size(1280, 720);
		RelativeLuminance relativeLuminance(3, size, configuration.GetLuminanceFlashParams());
		cv::Mat imageWhiteBgr(size, CV_8UC3, white);
		IrisFrame pImageWhitesRgb;
		pImageWhitesRgb.sRgbFrame = frameRgbConverter->Convert(imageWhiteBgr);
		cv::Mat imageBlackBgr(size, CV_8UC3, black);
		IrisFrame pImagesBlackRgb;
		pImagesBlackRgb.sRgbFrame = frameRgbConverter->Convert(imageBlackBgr);

		relativeLuminance.SetCurrentFrame(pImageWhitesRgb);
		relativeLuminance.SetCurrentFrame(pImagesBlackRgb);
		relativeLuminance.SetCurrentFrame(pImagesBlackRgb);
		cv::Mat* diff = relativeLuminance.FrameDifference();
		float testLum = diff->at<float>(0, 0);
		EXPECT_EQ(0, testLum);

		delete diff;
		pImagesBlackRgb.Release();
		pImageWhitesRgb.Release();
	}

	TEST_F(RelativeLuminanceTest,CheckTransition_WhenFalse_From_value_to_zero_Test)
	{
		RelativeLuminance relativeLuminance(5, cv::Size(), configuration.GetLuminanceFlashParams());
		float testLastAvg = 0.02f;

		//SameSign (positive) 0 case && !newTransition
		Flash::CheckTransitionResult transitionResult = relativeLuminance.CheckTransition(0, testLastAvg);

		EXPECT_FALSE(transitionResult.checkResult);
		EXPECT_EQ(0.02f, transitionResult.lastAvgDiffAcc);
	}

	TEST_F(RelativeLuminanceTest, CheckTransition_WhenFalse_From_value_to_value_Test)
	{
		RelativeLuminance relativeLuminance(5, cv::Size(), configuration.GetLuminanceFlashParams());
		float testLastAvg = 0.02f;

		//SameSign (positive) 0 case && !newTransition
		Flash::CheckTransitionResult transitionResult = relativeLuminance.CheckTransition(0.05f, testLastAvg);

		EXPECT_FALSE(transitionResult.checkResult);
		EXPECT_EQ(0.07f, transitionResult.lastAvgDiffAcc);
	}

	TEST_F(RelativeLuminanceTest, CheckTransition_WhenTrue_From_value_to_value_Test)
	{
		RelativeLuminance relativeLuminance(5, cv::Size(), configuration.GetLuminanceFlashParams());
		float testLastAvg = 0.07f;

		//SameSign (positive) 0 case && !newTransition
		Flash::CheckTransitionResult transitionResult = relativeLuminance.CheckTransition(0.03f, testLastAvg);

		EXPECT_TRUE(transitionResult.checkResult);
		EXPECT_EQ(0.1f, transitionResult.lastAvgDiffAcc);
	}

	TEST_F(RelativeLuminanceTest, CheckTransition_WhenFalse_From_threshold_value_to_value_Test)
	{
		RelativeLuminance relativeLuminance(5, cv::Size(), configuration.GetLuminanceFlashParams());
		float testLastAvg = 0.1f;

		//SameSign (positive) 0 case && !newTransition
		Flash::CheckTransitionResult transitionResult = relativeLuminance.CheckTransition(0.1f, testLastAvg);

		EXPECT_FALSE(transitionResult.checkResult);
		EXPECT_EQ(0.2f, transitionResult.lastAvgDiffAcc);
	}

	TEST_F(RelativeLuminanceTest, CheckTransition_WhenTrue_From_value_to_negative_value_Test)
	{
		RelativeLuminance relativeLuminance(5, cv::Size(), configuration.GetLuminanceFlashParams());
		float testLastAvg = 0.2f;

		//SameSign (positive) 0 case && !newTransition
		Flash::CheckTransitionResult transitionResult = relativeLuminance.CheckTransition(-0.1f, testLastAvg);

		EXPECT_TRUE(transitionResult.checkResult);
		EXPECT_EQ(-0.1f, transitionResult.lastAvgDiffAcc);
	}

	TEST_F(RelativeLuminanceTest, CheckTransition_WhenTrue_From_negative_value_to_value_Test)
	{
		RelativeLuminance relativeLuminance(5, cv::Size(), configuration.GetLuminanceFlashParams());
		float testLastAvg = -0.1f;

		//SameSign (positive) 0 case && !newTransition
		Flash::CheckTransitionResult transitionResult = relativeLuminance.CheckTransition(0.1f, testLastAvg);

		EXPECT_TRUE(transitionResult.checkResult);
		EXPECT_EQ(0.1f, transitionResult.lastAvgDiffAcc);
	}

	TEST_F(RelativeLuminanceTest, CheckTransition_WhenFalse_From_value_to_negative_value_Test)
	{
		RelativeLuminance relativeLuminance(5, cv::Size(), configuration.GetLuminanceFlashParams());
		float testLastAvg = 0.1f;

		//SameSign (positive) 0 case && !newTransition
		Flash::CheckTransitionResult transitionResult = relativeLuminance.CheckTransition(-0.05f, testLastAvg);

		EXPECT_FALSE(transitionResult.checkResult);
		EXPECT_EQ(-0.05f, transitionResult.lastAvgDiffAcc);
	}

	TEST_F(RelativeLuminanceTest, CheckTransition_WhenFalse_From_negative_value_to_zero_Test)
	{
		RelativeLuminance relativeLuminance(5, cv::Size(), configuration.GetLuminanceFlashParams());
		float testLastAvg = -0.05f;

		//SameSign (positive) 0 case && !newTransition
		Flash::CheckTransitionResult transitionResult = relativeLuminance.CheckTransition(0, testLastAvg);

		EXPECT_FALSE(transitionResult.checkResult);
		EXPECT_EQ(-0.05f, transitionResult.lastAvgDiffAcc);
	}

	TEST_F(RelativeLuminanceTest, CheckTransition_WhenTrue_From_negative_value_to_negative_value_Test)
	{
		RelativeLuminance relativeLuminance(5, cv::Size(), configuration.GetLuminanceFlashParams());
		float testLastAvg = -0.05f;

		//SameSign (positive) 0 case && !newTransition
		Flash::CheckTransitionResult transitionResult = relativeLuminance.CheckTransition(-0.06f, testLastAvg);

		EXPECT_TRUE(transitionResult.checkResult);
		EXPECT_EQ(-0.11f, transitionResult.lastAvgDiffAcc);
	}

	TEST_F(RelativeLuminanceTest, CheckTransition_WhenFalse_From_negative_threshold_value_to_negative_value_Test)
	{
		RelativeLuminance relativeLuminance(5, cv::Size(), configuration.GetLuminanceFlashParams());
		float testLastAvg = -0.11f;

		//SameSign (positive) 0 case && !newTransition
		Flash::CheckTransitionResult transitionResult = relativeLuminance.CheckTransition(-0.1f, testLastAvg);

		EXPECT_FALSE(transitionResult.checkResult);
		EXPECT_EQ(-0.21f, Flash::roundoff(transitionResult.lastAvgDiffAcc, 2));
	}

	TEST_F(RelativeLuminanceTest, CheckTransition_WhenFalse_From_zero_to_negative_threshold_value_to_negative_threshold_value_Test)
	{
		RelativeLuminance relativeLuminance(2, cv::Size(), configuration.GetLuminanceFlashParams());
		float testLastAvg = 0;

		//SameSign (positive) 0 case && !newTransition
		Flash::CheckTransitionResult transitionResult = relativeLuminance.CheckTransition(-0.11f, testLastAvg);
		testLastAvg = transitionResult.lastAvgDiffAcc;
		transitionResult = relativeLuminance.CheckTransition(-0.1f, testLastAvg);
		testLastAvg = transitionResult.lastAvgDiffAcc;
		transitionResult = relativeLuminance.CheckTransition(-0.1f, testLastAvg);

		EXPECT_FALSE(transitionResult.checkResult);
		EXPECT_EQ(-0.2f, Flash::roundoff(transitionResult.lastAvgDiffAcc, 2));
	}

	TEST_F(RelativeLuminanceTest, AverageLuminanca_WhenWhite_Test)
	{
		RelativeLuminance relativeLuminance(3, cv::Size(), configuration.GetLuminanceFlashParams());
		cv::Mat imageWhiteBgr(1280, 720, CV_8UC3, white);
		IrisFrame pImageWhitesRgb;
		pImageWhitesRgb.sRgbFrame = frameRgbConverter->Convert(imageWhiteBgr);

		relativeLuminance.SetCurrentFrame(pImageWhitesRgb);
		float avgLum = relativeLuminance.FrameMean();
		EXPECT_EQ(1, avgLum);
		
		pImageWhitesRgb.Release();
	}

	TEST_F(RelativeLuminanceTest, AverageLuminanca_WhenGray_Test)
	{
		RelativeLuminance relativeLuminance(3, cv::Size(), configuration.GetLuminanceFlashParams());
		cv::Mat imageWhiteBgr(1280, 720, CV_8UC3, gray);
		IrisFrame pImageWhitesRgb;
		pImageWhitesRgb.sRgbFrame = frameRgbConverter->Convert(imageWhiteBgr);

		relativeLuminance.SetCurrentFrame(pImageWhitesRgb);
		float avgLum = relativeLuminance.roundoff(relativeLuminance.FrameMean(), 2);
		EXPECT_EQ(0.22f, avgLum);

		pImageWhitesRgb.Release();
	}

	TEST_F(RelativeLuminanceTest, AverageLuminanca_WhenRealFrame_Test)
	{
		RelativeLuminance relativeLuminance(3, cv::Size(), configuration.GetLuminanceFlashParams());
		cv::Mat frameBgr = cv::imread("data/TestImages/frames/FrameForTest.jpg");
		IrisFrame pFramesRgb;
		pFramesRgb.sRgbFrame = frameRgbConverter->Convert(frameBgr);

		relativeLuminance.SetCurrentFrame(pFramesRgb);
		float avgLum = relativeLuminance.roundoff(relativeLuminance.FrameMean(), 2);
		EXPECT_EQ(0.33f, avgLum);

		pFramesRgb.Release();
	}

	TEST_F(RelativeLuminanceTest, SafeArea_No_Change_Threshold)
	{
		cv::Size size(5, 5);
		RelativeLuminance luminance(3, size, configuration.GetLuminanceFlashParams());

		cv::Mat imageBgr(size, CV_8UC3, blue);
		cv::Mat* imageSbgr = frameRgbConverter->Convert(imageBgr);

		luminance.SetCurrentFrame(imageSbgr);
		luminance.SetCurrentFrame(imageSbgr);

		cv::Mat* frameDiff = luminance.FrameDifference();
		float avgDifference = luminance.CheckSafeArea(frameDiff);
		EXPECT_EQ(0, avgDifference);

		float flashAreaProportion = luminance.GetFlashArea();
		EXPECT_EQ(0, flashAreaProportion);

		delete imageSbgr;
		delete frameDiff;
	}

	TEST_F(RelativeLuminanceTest, SafeArea_20_Percent_Change_Threshold)
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

		RelativeLuminance luminance(3, size, configuration.GetLuminanceFlashParams());

		cv::Mat imageBgr(size, CV_8UC3, blue);
		cv::Mat* imageSbgr = frameRgbConverter->Convert(imageBgr);
		luminance.SetCurrentFrame(imageSbgr);

		cv::Mat imageBgr2(size, CV_8UC3, white);
		cv::Mat* imageSbgr2 = frameRgbConverter->Convert(imageBgr2);
		luminance.SetCurrentFrame(imageSbgr2);

		float avgDifference = luminance.CheckSafeArea(&frameDiff);
		EXPECT_EQ(0, avgDifference);

		float flashAreaProportion = luminance.GetFlashArea();
		EXPECT_TRUE(CompareFloat(0.2, flashAreaProportion));

		delete imageSbgr;
		delete imageSbgr2;
	}

	TEST_F(RelativeLuminanceTest, SafeArea_100_Percent_Change_Threshold)
	{
		cv::Size size(5, 5);
		RelativeLuminance luminance(3, size, configuration.GetLuminanceFlashParams());

		cv::Mat imageBgr(size, CV_8UC3, blue);
		cv::Mat* imageSbgr = frameRgbConverter->Convert(imageBgr);
		luminance.SetCurrentFrame(imageSbgr);

		cv::Mat imageBgr2(size, CV_8UC3, white);
		cv::Mat* imageSbgr2 = frameRgbConverter->Convert(imageBgr2);
		luminance.SetCurrentFrame(imageSbgr2);

		cv::Mat* frameDiff = luminance.FrameDifference();
		float avgDifference = luminance.CheckSafeArea(frameDiff);
		EXPECT_TRUE(CompareFloat(0.9278, avgDifference));

		float flashAreaProportion = luminance.GetFlashArea();
		EXPECT_EQ(1, flashAreaProportion);

		delete imageSbgr;
		delete imageSbgr2;
		delete frameDiff;
	}
}