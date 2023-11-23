//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include <gtest/gtest.h>
#include "utils/FrameConverter.h"
#include "IrisLibTest.h"
#include <opencv2/opencv.hpp>

namespace iris::Tests
{
	class FrameRgbConverterTest : public IrisLibTest {
	};

	TEST_F(FrameRgbConverterTest, MemoryLeakTest)
	{
		uint8_t testArr[3][3] = {
				{ 0, 1, 2 },
				{ 3, 3, 2 },
				{ 1, 0, 0 }
		};
		cv::Mat bgrMat = cv::Mat(3, 3, CV_8U, &testArr);

		EA::EACC::Utils::FrameConverter* converter = new EA::EACC::Utils::FrameConverter(configuration.GetFrameSrgbConverterParams());
		cv::Mat* sRgbMat = converter->Convert(bgrMat);

		for (int x = 0; x < sRgbMat->cols; x++) {
			for (int y = 0; y < sRgbMat->cols; y++) {
				int vSource = (int)testArr[x][y];
				cv::Mat* m = converter->GetTable();
				float fTarget = m->at<float>(vSource, 0);
				float fCurrent = sRgbMat->at<float>(x, y);
				EXPECT_EQ(fCurrent, fTarget);
			}
		}

		sRgbMat->release();
		delete sRgbMat;
		bgrMat.release();
		delete converter;
	}
}