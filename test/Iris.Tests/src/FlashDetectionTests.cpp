//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include <gtest/gtest.h>
#include "IrisLibTest.h"
#include <opencv2/core.hpp>
#include "FlashDetection.h"
#include "FrameData.h"
#include  "utils/FrameConverter.h"
#include "IrisFrame.h"

namespace iris::Tests
{
	class FlashDetectionTests : public IrisLibTest {
	protected:

		void SetUp() override { //call configuration loading per test to load different luminances
            cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_ERROR);
            iris::Log::Init(false, false);
        }
		~FlashDetectionTests() override {
		}
	};

	TEST_F(FlashDetectionTests, RELATIVE_LUMINANCE)
	{
        
        configuration.SetLuminanceType(Configuration::LuminanceType::RELATIVE);
        configuration.Init();


        cv::Size size(100, 100);

        cv::Mat blackFrame(size, CV_8UC3, black); //lum = 0f
        cv::Mat whiteFrame(size, CV_8UC3, white); //lum = 1f
        cv::Mat redFrame(size, CV_8UC3, red); //lum = 0.2126f

        FlashDetection flashDetection(&configuration, 7, size);
        EA::EACC::Utils::FrameConverter sRgbConverter(configuration.GetFrameSrgbConverterParams());

        FrameData data;
        IrisFrame irisBlackFrame(&blackFrame, sRgbConverter.Convert(blackFrame), FrameData());
        IrisFrame irisWhiteFrame(&whiteFrame, sRgbConverter.Convert(whiteFrame), FrameData());
        IrisFrame irisRedFrame(&redFrame, sRgbConverter.Convert(redFrame), FrameData());

        //add transitions
        flashDetection.setLuminance(irisBlackFrame);
        flashDetection.checkFrame(irisBlackFrame, 0, data);

        flashDetection.setLuminance(irisWhiteFrame);
        flashDetection.checkFrame(irisWhiteFrame, 1, data);

        flashDetection.setLuminance(irisRedFrame);
        flashDetection.checkFrame(irisRedFrame, 2, data);

        flashDetection.setLuminance(irisWhiteFrame);
        flashDetection.checkFrame(irisWhiteFrame, 3, data);

        flashDetection.setLuminance(irisRedFrame);
        flashDetection.checkFrame(irisRedFrame, 4, data);

        FrameData expectedData;
        expectedData.Frame = 0;
        expectedData.LuminanceAverage = 0.2126f;
        expectedData.LuminanceFlashArea = "100.00%";
        expectedData.AverageLuminanceDiff = -0.7874f;
        expectedData.AverageLuminanceDiffAcc = -0.7874f;
        expectedData.RedAverage = 320;
        expectedData.RedFlashArea = "100.00%";
        expectedData.AverageRedDiff = 320;
        expectedData.AverageRedDiffAcc = 320;
        expectedData.LuminanceTransitions = 4;
        expectedData.RedTransitions = 3;
        expectedData.LuminanceExtendedFailCount = 1;
        expectedData.RedExtendedFailCount = 0;
        expectedData.luminanceFrameResult = FlashResult::PassWithWarning;
        expectedData.redFrameResult = FlashResult::Pass;

        EXPECT_EQ(expectedData.LuminanceAverage, data.LuminanceAverage);
        EXPECT_EQ(expectedData.LuminanceFlashArea, data.LuminanceFlashArea);
        EXPECT_EQ(expectedData.AverageLuminanceDiff, data.AverageLuminanceDiff);
        EXPECT_EQ(expectedData.AverageLuminanceDiffAcc, data.AverageLuminanceDiffAcc);
        EXPECT_EQ(expectedData.RedAverage, data.RedAverage);
        EXPECT_EQ(expectedData.RedFlashArea, data.RedFlashArea);
        EXPECT_EQ(expectedData.AverageRedDiff, data.AverageRedDiff);
        EXPECT_EQ(expectedData.AverageRedDiffAcc, data.AverageRedDiffAcc);
        EXPECT_EQ(expectedData.LuminanceTransitions, data.LuminanceTransitions);
        EXPECT_EQ(expectedData.RedTransitions, data.RedTransitions);
        EXPECT_EQ(expectedData.LuminanceExtendedFailCount, data.LuminanceExtendedFailCount);
        EXPECT_EQ(expectedData.RedExtendedFailCount, data.RedExtendedFailCount);
        EXPECT_EQ(expectedData.luminanceFrameResult, data.luminanceFrameResult);
        EXPECT_EQ(expectedData.redFrameResult, data.redFrameResult);

        EXPECT_FALSE(flashDetection.isFail());

        irisBlackFrame.Release();
        irisRedFrame.Release();
        irisWhiteFrame.Release();
	}

    TEST_F(FlashDetectionTests, CD_LUMINANCE)
    {
        configuration.SetLuminanceType(Configuration::LuminanceType::CD);
        configuration.Init();

        cv::Size size(100, 100);

        cv::Mat blackFrame(size, CV_8UC3, black); //lum = 0.07f
        cv::Mat whiteFrame(size, CV_8UC3, white); //lum = 200f
        cv::Mat redFrame(size, CV_8UC3, red); //lum = 015.93f

        FlashDetection flashDetection(&configuration, 7, size);
        EA::EACC::Utils::FrameConverter sRgbConverter(configuration.GetFrameSrgbConverterParams());

        FrameData data;
        IrisFrame irisBlackFrame(&blackFrame, sRgbConverter.Convert(blackFrame), FrameData());
        IrisFrame irisWhiteFrame(&whiteFrame, sRgbConverter.Convert(whiteFrame), FrameData());
        IrisFrame irisRedFrame(&redFrame, sRgbConverter.Convert(redFrame), FrameData());

        //add transitions
        flashDetection.setLuminance(irisBlackFrame);
        flashDetection.checkFrame(irisBlackFrame, 0, data);

        flashDetection.setLuminance(irisWhiteFrame);
        flashDetection.checkFrame(irisWhiteFrame, 1, data);

        flashDetection.setLuminance(irisRedFrame);
        flashDetection.checkFrame(irisRedFrame, 2, data);

        flashDetection.setLuminance(irisWhiteFrame);
        flashDetection.checkFrame(irisWhiteFrame, 3, data);

        flashDetection.setLuminance(irisRedFrame);
        flashDetection.checkFrame(irisRedFrame, 4, data);

        FrameData expectedData;
        expectedData.Frame = 0;
        expectedData.LuminanceAverage = 15.93f;
        expectedData.LuminanceFlashArea = "100.00%";
        expectedData.AverageLuminanceDiff = -184.07f;
        expectedData.AverageLuminanceDiffAcc = -184.07f;
        expectedData.RedAverage = 320;
        expectedData.RedFlashArea = "100.00%";
        expectedData.AverageRedDiff = 320;
        expectedData.AverageRedDiffAcc = 320;
        expectedData.LuminanceTransitions = 4;
        expectedData.RedTransitions = 3;
        expectedData.LuminanceExtendedFailCount = 1;
        expectedData.RedExtendedFailCount = 0;
        expectedData.luminanceFrameResult = FlashResult::PassWithWarning;
        expectedData.redFrameResult = FlashResult::Pass;

        EXPECT_EQ(expectedData.LuminanceAverage, data.LuminanceAverage);
        EXPECT_EQ(expectedData.LuminanceFlashArea, data.LuminanceFlashArea);
        EXPECT_EQ(expectedData.AverageLuminanceDiff, data.AverageLuminanceDiff);
        EXPECT_EQ(expectedData.AverageLuminanceDiffAcc, data.AverageLuminanceDiffAcc);
        EXPECT_EQ(expectedData.RedAverage, data.RedAverage);
        EXPECT_EQ(expectedData.RedFlashArea, data.RedFlashArea);
        EXPECT_EQ(expectedData.AverageRedDiff, data.AverageRedDiff);
        EXPECT_EQ(expectedData.AverageRedDiffAcc, data.AverageRedDiffAcc);
        EXPECT_EQ(expectedData.LuminanceTransitions, data.LuminanceTransitions);
        EXPECT_EQ(expectedData.RedTransitions, data.RedTransitions);
        EXPECT_EQ(expectedData.LuminanceExtendedFailCount, data.LuminanceExtendedFailCount);
        EXPECT_EQ(expectedData.RedExtendedFailCount, data.RedExtendedFailCount);
        EXPECT_EQ(expectedData.luminanceFrameResult, data.luminanceFrameResult);
        EXPECT_EQ(expectedData.redFrameResult, data.redFrameResult);

        EXPECT_FALSE(flashDetection.isFail());

        irisBlackFrame.Release();
        irisRedFrame.Release();
        irisWhiteFrame.Release();
    }
}