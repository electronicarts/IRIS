//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include "IrisLibTest.h"
#include <opencv2/core.hpp>
#include "PatternDetection.h"
#include "FlashDetection.h"
#include "IrisFrame.h"
#include "utils/FrameConverter.h"
#include "iris/TotalFlashIncidents.h"

namespace iris::Tests
{
	class PatternDetectionTests : public IrisLibTest {
	protected:
		EA::EACC::Utils::FrameConverter* frameRgbConverter = nullptr;
		void SetUp() override {
			configuration.SetLuminanceType(Configuration::LuminanceType::RELATIVE);
			IrisLibTest::SetUp();

			frameRgbConverter = new EA::EACC::Utils::FrameConverter(configuration.GetFrameSrgbConverterParams());
		}

		~PatternDetectionTests()
		{
			if (frameRgbConverter != nullptr)
			{
				delete frameRgbConverter;
			}
		}
	};

	TEST_F(PatternDetectionTests, NoPattern_Pass)
	{
		cv::Mat image = cv::imread("data/TestImages/Patterns/shapes.png");
		IrisFrame irisFrame(&image, frameRgbConverter->Convert(image), FrameData());

		FlashDetection flashDetection(&configuration, 0, image.size());
		PatternDetection patternDetection(&configuration, 5, image.size());
		flashDetection.setLuminance(irisFrame);

		FrameData data;
		for (int i = 0; i < 5; i++)
		{
			patternDetection.checkFrame(irisFrame, i, data);
		}

		EXPECT_EQ(PatternResult::Pass, data.patternFrameResult);

		irisFrame.Release();
	}


	TEST_F(PatternDetectionTests, Straight_Lines_Fail)
	{
		cv::Mat image = cv::imread("data/TestImages/Patterns/20stripes.png");
		IrisFrame irisFrame(&image, frameRgbConverter->Convert(image), FrameData());

		FlashDetection flashDetection(&configuration, 0, image.size());
		PatternDetection patternDetection(&configuration, 5, image.size());
		flashDetection.setLuminance(irisFrame);

		FrameData data;
		for (int i = 0; i < 5; i++)
		{
			patternDetection.checkFrame(irisFrame, i, data);
		}

		EXPECT_EQ(PatternResult::Fail, data.patternFrameResult);

		irisFrame.Release();
	}

	TEST_F(PatternDetectionTests, Straight_Lines_Pass)
	{
		cv::Mat image = cv::imread("data/TestImages/Patterns/ACO_Pattern.png");
		IrisFrame irisFrame(&image, frameRgbConverter->Convert(image), FrameData());

		FlashDetection flashDetection(&configuration, 0, image.size());
		PatternDetection patternDetection(&configuration, 5, image.size());
		flashDetection.setLuminance(irisFrame);

		FrameData data;
		for (int i = 0; i < 5; i++)
		{
			patternDetection.checkFrame(irisFrame, i, data);
		}

		EXPECT_EQ(PatternResult::Pass, data.patternFrameResult);

		irisFrame.Release();
	}
}