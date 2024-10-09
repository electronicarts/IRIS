//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include <gtest/gtest.h>
#include "utils/FrameConverter.h"
#include "RedSaturation.h"
#include "IrisLibTest.h"
#include <opencv2/opencv.hpp>

namespace iris::Tests
{
	class RedSaturationTests : public IrisLibTest {
	protected:
		EA::EACC::Utils::FrameConverter* frameRgbConverter = nullptr;

		void SetUp() override {
			IrisLibTest::SetUp();
			frameRgbConverter = new EA::EACC::Utils::FrameConverter(configuration.GetFrameSrgbConverterParams());
		}
		~RedSaturationTests() override {
			delete frameRgbConverter;
		}
	};

    TEST_F(RedSaturationTests, Positive_FrameDifference)
    {
		cv::Size size(100, 100);
		RedSaturation redSaturation(0, size, configuration.GetRedSaturationFlashParams());
		cv::Mat redImageBgr(size, CV_8UC3, red);
		cv::Mat* pRedSrgb = frameRgbConverter->Convert(redImageBgr);

		cv::Mat blackImageBgr(size, CV_8UC3, black);
		cv::Mat* pBlackSrgb = frameRgbConverter->Convert(blackImageBgr);

		redSaturation.SetCurrentFrame(pBlackSrgb);
		redSaturation.SetCurrentFrame(pRedSrgb);

		cv::Mat* matDiff = redSaturation.FrameDifference();

		double testChangeValues = matDiff->at<double>(0, 0);

		EXPECT_EQ(320, testChangeValues);

		if (matDiff != nullptr) {
		matDiff->release();
		delete matDiff;
		}

		delete pRedSrgb;
		delete pBlackSrgb;
    }

    TEST_F(RedSaturationTests, Negative_FrameDifference)
    {
		cv::Size size(2000, 2000);
		RedSaturation redSaturation(0, size, configuration.GetRedSaturationFlashParams());
		cv::Mat redImageBgr(size, CV_8UC3, red);
		cv::Mat* pRedSrgb = frameRgbConverter->Convert(redImageBgr);

		cv::Mat blackImageBgr(size, CV_8UC3, black);
		cv::Mat* pBlackSrgb = frameRgbConverter->Convert(blackImageBgr);

		redSaturation.SetCurrentFrame(pRedSrgb);
		redSaturation.SetCurrentFrame(pBlackSrgb);

		cv::Mat* matDiff = redSaturation.FrameDifference();

		double testChangeValues = matDiff->at<double>(0, 0);

		EXPECT_EQ(-320, testChangeValues);

		if (matDiff != nullptr) {
		matDiff->release();
		delete matDiff;
		}

		delete pRedSrgb;
		delete pBlackSrgb;
    }

    TEST_F(RedSaturationTests, Positive_FrameDifference_PartialRed)
    {
		cv::Size size(100, 100);
		RedSaturation redSaturation(0, size, configuration.GetRedSaturationFlashParams());

		cv::Mat blackImageBgr(size, CV_8UC3, black);
		cv::Mat* pBlackSrgb = frameRgbConverter->Convert(blackImageBgr);

		cv::Mat redImageBgr(size, CV_8UC3, black);
		cv::rectangle(redImageBgr, cv::Rect(20, 20, 30, 30), red, -1);
		cv::Mat* pRedSrgb = frameRgbConverter->Convert(redImageBgr);

		redSaturation.SetCurrentFrame(pBlackSrgb);
		redSaturation.SetCurrentFrame(pRedSrgb);

		cv::Mat* matDiff = redSaturation.FrameDifference();

		double testChangeValues = matDiff->at<double>(21, 21);
		double testNullChangeValues = matDiff->at<double>(0, 0);

		EXPECT_EQ(320, testChangeValues);
		EXPECT_EQ(0, testNullChangeValues);

		if (matDiff != nullptr) {
		matDiff->release();
		delete matDiff;
		}

		delete pRedSrgb;
		delete pBlackSrgb;
    }

	TEST_F(RedSaturationTests, Negative_FrameDifference_PartialRed)
	{
		cv::Size size(100, 100);
		RedSaturation redSaturation(0, size, configuration.GetRedSaturationFlashParams());
		
		cv::Mat blackImageBgr(size, CV_8UC3, black);
		cv::Mat* pBlackSrgb = frameRgbConverter->Convert(blackImageBgr);

		cv::Mat redImageBgr(size, CV_8UC3, black);
		cv::rectangle(redImageBgr, cv::Rect(20, 20, 30, 30), red, -1);
		cv::Mat* pRedSrgb = frameRgbConverter->Convert(redImageBgr);


		redSaturation.SetCurrentFrame(pRedSrgb);
		redSaturation.SetCurrentFrame(pBlackSrgb);

		cv::Mat* matDiff = redSaturation.FrameDifference();

		double testChangeValues = matDiff->at<double>(21, 21);
		double testNullChangeValues = matDiff->at<double>(0, 0);

		EXPECT_EQ(-320, testChangeValues);
		EXPECT_EQ(0, testNullChangeValues);

		if (matDiff != nullptr) {
			matDiff->release();
			delete matDiff;
		}

		delete pRedSrgb;
		delete pBlackSrgb;
	}

	TEST_F(RedSaturationTests, CheckTransition_Positive_Value_To_Zero)
	{
		RedSaturation redSaturation (5, cv::Size(), configuration.GetRedSaturationFlashParams());

		double testLastAvg = 5.2;
		
		//SameSign (positive) 0 case && !newTransition
		Flash::CheckTransitionResult transitionResult = redSaturation.CheckTransition(0, testLastAvg);

		EXPECT_EQ(5.2, transitionResult.lastAvgDiffAcc);
		EXPECT_FALSE(transitionResult.checkResult);
	}

	TEST_F(RedSaturationTests, CheckTransition_Positive_Value_Increment_NoTransition)
	{
		RedSaturation redSaturation(5, cv::Size(), configuration.GetRedSaturationFlashParams());

		double testLastAvg = 5.2;

		//SameSign (positive) && !newTransition
		Flash::CheckTransitionResult transitionResult = redSaturation.CheckTransition(10.0, testLastAvg);

		EXPECT_EQ(15.2, transitionResult.lastAvgDiffAcc);
		EXPECT_FALSE(transitionResult.checkResult);
	}

	TEST_F(RedSaturationTests, CheckTransition_Positive_Value_Increment_Transition)
	{
		RedSaturation redSaturation(5, cv::Size(), configuration.GetRedSaturationFlashParams());

		double testLastAvg = 15.2;

		//SameSign (positive) && newTransition
		Flash::CheckTransitionResult transitionResult = redSaturation.CheckTransition(5.0, testLastAvg);

		EXPECT_EQ(20.2, transitionResult.lastAvgDiffAcc);
		EXPECT_TRUE(transitionResult.checkResult);
	}

	TEST_F(RedSaturationTests, CheckTransition_Positive_Value_Decrement_NoTransition)
	{
		RedSaturation redSaturation(5, cv::Size(), configuration.GetRedSaturationFlashParams());

		double testLastAvg = 20.2;

		//SameSign (positive) && !newTransition (last was a transition)
		Flash::CheckTransitionResult transitionResult = redSaturation.CheckTransition(10.3, testLastAvg);

		EXPECT_EQ(30.5, transitionResult.lastAvgDiffAcc);
		EXPECT_FALSE(transitionResult.checkResult);
	}
	
	TEST_F(RedSaturationTests, CheckTransition_ToNegative_Transition)
	{
		RedSaturation redSaturation(5, cv::Size(), configuration.GetRedSaturationFlashParams());

		double testLastAvg = 30.5;

		//!SameSign (negative) && newTransition 
		Flash::CheckTransitionResult transitionResult = redSaturation.CheckTransition(-20.0, testLastAvg);

		EXPECT_EQ(-20.0, transitionResult.lastAvgDiffAcc);
		EXPECT_TRUE(transitionResult.checkResult);
	}

	TEST_F(RedSaturationTests, CheckTransition_ToPositive_NoTransition)
	{
		RedSaturation redSaturation(5, cv::Size(), configuration.GetRedSaturationFlashParams());

		double testLastAvg = -20.0;

		//SameSign (positive) && !newTransition (last was a transition)
		Flash::CheckTransitionResult transitionResult = redSaturation.CheckTransition(25.0, testLastAvg);

		EXPECT_EQ(25.0, transitionResult.lastAvgDiffAcc);
		EXPECT_TRUE(transitionResult.checkResult);
	}

	TEST_F(RedSaturationTests, CheckTransition_ToNegative_NoTransition)
	{
		RedSaturation redSaturation(5, cv::Size(), configuration.GetRedSaturationFlashParams());

		double testLastAvg = 25.0;

		//!SameSign (negative) && !newTransition
		Flash::CheckTransitionResult transitionResult = redSaturation.CheckTransition(-10.0, testLastAvg);

		EXPECT_EQ(-10.0, transitionResult.lastAvgDiffAcc);
		EXPECT_FALSE(transitionResult.checkResult);
	}
	
	TEST_F(RedSaturationTests, CheckTransition_Negative_Value_To_Zero)
	{
		RedSaturation redSaturation(5, cv::Size(), configuration.GetRedSaturationFlashParams());
		
		double testLastAvg = -10.0;

		//SameSign (negative) 0 case && !newTransition
		Flash::CheckTransitionResult transitionResult = redSaturation.CheckTransition(0.0, testLastAvg);

		EXPECT_EQ(-10.0, transitionResult.lastAvgDiffAcc);
		EXPECT_FALSE(transitionResult.checkResult);
	}

	TEST_F(RedSaturationTests, CheckTransition_Negative_Value_Decrement_Transition)
	{
		RedSaturation redSaturation(5, cv::Size(), configuration.GetRedSaturationFlashParams());
		
		double testLastAvg = -10.0;

		//SameSign (negative) && newTransition
		Flash::CheckTransitionResult transitionResult = redSaturation.CheckTransition(-15.0, testLastAvg);

		EXPECT_EQ(-25.0, transitionResult.lastAvgDiffAcc);
		EXPECT_TRUE(transitionResult.checkResult);
	}

	TEST_F(RedSaturationTests, CheckTransition_Negative_Value_Decrement_NoTransition)
	{
		RedSaturation redSaturation(5, cv::Size(), configuration.GetRedSaturationFlashParams());

		
		double testLastAvg = -25.0;

		//SameSign (negative) && !newTransition (last was a transition)

		Flash::CheckTransitionResult transitionResult = redSaturation.CheckTransition(-30.6, testLastAvg);

		EXPECT_EQ(-55.6, transitionResult.lastAvgDiffAcc);
		EXPECT_FALSE(transitionResult.checkResult);
	}

	TEST_F(RedSaturationTests, CheckTransition_Negative_Decrements_NoTransition)
	{
		RedSaturation redSaturation(2, cv::Size(), configuration.GetRedSaturationFlashParams());
		
		double testLastAvg = 0.0;

		//SameSign (negative) && !newTransition (last was a transition)
		Flash::CheckTransitionResult transitionResult = redSaturation.CheckTransition(-25.6, testLastAvg);
		transitionResult = redSaturation.CheckTransition(-30.6, transitionResult.lastAvgDiffAcc);
		transitionResult = redSaturation.CheckTransition(-10.6, transitionResult.lastAvgDiffAcc);

		EXPECT_EQ(-41.2, transitionResult.lastAvgDiffAcc);
		EXPECT_FALSE(transitionResult.checkResult);
	}

	TEST_F(RedSaturationTests, SafeArea_No_Change_Threshold)
	{
		cv::Size size(5, 5);
		RedSaturation redSaturation(3, size, configuration.GetRedSaturationFlashParams());

		cv::Mat imageBgr(size, CV_8UC3, red);
		cv::Mat* imageSbgr = frameRgbConverter->Convert(imageBgr);

		redSaturation.SetCurrentFrame(imageSbgr);
		redSaturation.SetCurrentFrame(imageSbgr);

		cv::Mat* frameDiff = redSaturation.FrameDifference();
		double avgDifference = redSaturation.CheckSafeArea(frameDiff);
		EXPECT_EQ(0, avgDifference);

		double flashAreaProportion = redSaturation.GetFlashArea();
		EXPECT_EQ(0, flashAreaProportion);

		delete imageSbgr;
		delete frameDiff;
	}

	TEST_F(RedSaturationTests, SafeArea_20_Percent_Change_Threshold)
	{
		cv::Size size(5, 5);

		double frameDiffArray[5][5] = {
				{ 200, 200, 200, 0, 0 },
				{ 200, 200, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 },
				{ 0, 0, 0, 0, 0 }
		};
		cv::Mat frameDiff = cv::Mat(size, CV_32FC1, &frameDiffArray);

		RedSaturation redSaturation(3, size, configuration.GetRedSaturationFlashParams());

		cv::Mat imageBgr(size, CV_8UC3, blue);
		cv::Mat* imageSbgr = frameRgbConverter->Convert(imageBgr);
		redSaturation.SetCurrentFrame(imageSbgr);

		cv::Mat imageBgr2(size, CV_8UC3, red);
		cv::Mat* imageSbgr2 = frameRgbConverter->Convert(imageBgr2);
		redSaturation.SetCurrentFrame(imageSbgr2);

		double avgDifference = redSaturation.CheckSafeArea(&frameDiff);
		EXPECT_EQ(0, avgDifference);

		double flashAreaProportion = redSaturation.GetFlashArea();
		EXPECT_TRUE(CompareDouble(0.2, flashAreaProportion));

		delete imageSbgr;
		delete imageSbgr2;
	}

	TEST_F(RedSaturationTests, SafeArea_100_Percent_Change_Threshold)
	{
		cv::Size size(5, 5);
		RedSaturation redSaturation(3, size, configuration.GetRedSaturationFlashParams());

		cv::Mat imageBgr(size, CV_8UC3, blue);
		cv::Mat* imageSbgr = frameRgbConverter->Convert(imageBgr);
		redSaturation.SetCurrentFrame(imageSbgr);

		cv::Mat imageBgr2(size, CV_8UC3, red);
		cv::Mat* imageSbgr2 = frameRgbConverter->Convert(imageBgr2);
		redSaturation.SetCurrentFrame(imageSbgr2);

		cv::Mat* frameDiff = redSaturation.FrameDifference();
		double avgDifference = redSaturation.CheckSafeArea(frameDiff);
		EXPECT_EQ(320, avgDifference);

		double flashAreaProportion = redSaturation.GetFlashArea();
		EXPECT_EQ(1, flashAreaProportion);

		delete imageSbgr;
		delete imageSbgr2;
		delete frameDiff;
	}
}