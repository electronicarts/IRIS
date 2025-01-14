//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include "IrisLibTest.h"
#include <opencv2/core.hpp>
#include "PatternDetection.h"
#include "FlashDetection.h"
#include "IrisFrame.h"
#include "utils/FrameConverter.h"
#include "iris/TotalFlashIncidents.h"
#include "FpsFrameManager.h"
#include "TimeFrameManager.h"

namespace iris::Tests
{
class PatternDetectionTests : public IrisLibTest 
{
protected:
	EA::EACC::Utils::FrameConverter* frameRgbConverter = nullptr;

	void SetUp() override 
	{
		IrisLibTest::SetUp();

		frameRgbConverter = new EA::EACC::Utils::FrameConverter(configuration.GetFrameSrgbConverterParams());
	}

	~PatternDetectionTests()
	{
		delete frameRgbConverter;
	}
};

TEST_F(PatternDetectionTests, NoPattern_Pass)
{
	cv::Mat image = cv::imread("data/TestImages/Patterns/shapes.png");
	IrisFrame irisFrame(&image, frameRgbConverter->Convert(image), FrameData());
	FpsFrameManager frameManager{};

	FlashDetection flashDetection(&configuration, 0, image.size(), &frameManager);
	PatternDetection patternDetection(&configuration, 5, image.size(), &frameManager);
	flashDetection.setLuminance(irisFrame);

	FrameData data;
	for (int i = 0; i < 5; i++)
	{
		frameManager.AddFrame(data);
		patternDetection.checkFrame(irisFrame, i, data);
	}

	EXPECT_EQ(PatternResult::Pass, data.patternFrameResult);

	irisFrame.Release();
}


TEST_F(PatternDetectionTests, Straight_Lines_Fail)
{
	cv::Mat image = cv::imread("data/TestImages/Patterns/20stripes.png");
	IrisFrame irisFrame(&image, frameRgbConverter->Convert(image), FrameData());
	FpsFrameManager frameManager{};
	
	FlashDetection flashDetection(&configuration, 0, image.size(), &frameManager);
	PatternDetection patternDetection(&configuration, 5, image.size(), &frameManager);
	flashDetection.setLuminance(irisFrame);

	FrameData data;
	for (int i = 0; i < 5; i++)
	{
		frameManager.AddFrame(data);
		patternDetection.checkFrame(irisFrame, i, data);
	}

	EXPECT_EQ(PatternResult::Fail, data.patternFrameResult);

	irisFrame.Release();
}

TEST_F(PatternDetectionTests, RealTime_NoPattern_Pass)
{
	cv::Mat image = cv::imread("data/TestImages/Patterns/shapes.png");
	IrisFrame irisFrame(&image, frameRgbConverter->Convert(image), FrameData());
	TimeFrameManager frameManager{};

	FlashDetection flashDetection(&configuration, 0, image.size(), &frameManager);
	PatternDetection patternDetection(&configuration, 5, image.size(), &frameManager);
	flashDetection.setLuminance(irisFrame);

	FrameData data;
	for (int i = 0; i < 5; i++)
	{
		frameManager.AddFrame(data);
		patternDetection.checkFrame(irisFrame, i, data);
		data.TimeStampVal += 250;
	}

	EXPECT_EQ(PatternResult::Pass, data.patternFrameResult);

	irisFrame.Release();
}


TEST_F(PatternDetectionTests, RealTime_Straight_Lines_Fail)
{
	cv::Mat image = cv::imread("data/TestImages/Patterns/20stripes.png");
	IrisFrame irisFrame(&image, frameRgbConverter->Convert(image), FrameData());
	TimeFrameManager frameManager{};

	FlashDetection flashDetection(&configuration, 0, image.size(), &frameManager);
	PatternDetection patternDetection(&configuration, 5, image.size(), &frameManager);
	flashDetection.setLuminance(irisFrame);

	FrameData data;
	for (int i = 0; i < 5; i++)
	{
		frameManager.AddFrame(data);
		patternDetection.checkFrame(irisFrame, i, data);
		data.TimeStampVal += 250;
	}

	EXPECT_EQ(PatternResult::Fail, data.patternFrameResult);

	irisFrame.Release();
}

}