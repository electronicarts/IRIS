//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved

#include "IrisLibTest.h"
#include <gtest/gtest.h>
#include "TransitionTrackerByTime.h"
#include "FrameData.h"

namespace iris::Tests
{
	class TransitionTrackerByTimeTest : public IrisLibTest {
	protected:
	};

	TEST_F(TransitionTrackerByTimeTest, Transition_WhenOnlyLuminance_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionTrackerByTime transitionTracker(8, configuration.GetTransitionTrackerParams());

		transitionTracker.SetTransitions(true, false, testData);
		transitionTracker.SetTransitions(true, false, testData);
		transitionTracker.SetTransitions(false, false, testData);
		transitionTracker.SetTransitions(true, false, testData);
		transitionTracker.SetTransitions(false, false, testData);
		transitionTracker.SetTransitions(false, false, testData);
		transitionTracker.SetTransitions(true, false, testData);

		EXPECT_EQ(4, testData.LuminanceTransitions);
		EXPECT_EQ(0, testData.RedTransitions);
	}

	TEST_F(TransitionTrackerByTimeTest, Transition_WhenOnlyRed_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionTrackerByTime transitionTracker(8, configuration.GetTransitionTrackerParams());

		transitionTracker.SetTransitions(false, false, testData);
		transitionTracker.SetTransitions(false, true, testData);
		transitionTracker.SetTransitions(false, false, testData);
		transitionTracker.SetTransitions(false, true, testData);
		transitionTracker.SetTransitions(false, false, testData);
		transitionTracker.SetTransitions(false, true, testData);
		transitionTracker.SetTransitions(false, true, testData);

		EXPECT_EQ(0, testData.LuminanceTransitions);
		EXPECT_EQ(4, testData.RedTransitions);
	}

	TEST_F(TransitionTrackerByTimeTest, Transition_WhenLuminanceAndRed_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionTrackerByTime transitionTracker(8, configuration.GetTransitionTrackerParams());

		transitionTracker.SetTransitions(false, false, testData);
		transitionTracker.SetTransitions(true, true, testData);
		transitionTracker.SetTransitions(false, false, testData);
		transitionTracker.SetTransitions(false, true, testData);
		transitionTracker.SetTransitions(true, false, testData);
		transitionTracker.SetTransitions(false, true, testData);
		transitionTracker.SetTransitions(true, true, testData);

		EXPECT_EQ(3, testData.LuminanceTransitions);
		EXPECT_EQ(4, testData.RedTransitions);
	}

	TEST_F(TransitionTrackerByTimeTest, EvaluateFrameMoment_WhenLuminanceFlashFail_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionTrackerByTime transitionTracker(8, configuration.GetTransitionTrackerParams());

		for (int i = 0; i < 8; i++)
		{
			transitionTracker.SetTransitions(true, false, testData);
		}
		transitionTracker.EvaluateFrameMoment(8, 8, testData);
		EXPECT_TRUE(transitionTracker.getFlashFail());
		EXPECT_TRUE(transitionTracker.getLumFlashFail());
		EXPECT_EQ(FlashResult::FlashFail, testData.luminanceFrameResult);
		EXPECT_EQ(1, transitionTracker.getLuminanceIncidents().flashFailFrames);
	}

	TEST_F(TransitionTrackerByTimeTest, EvaluateFrameMoment_WhenRedFlashFail_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionTrackerByTime transitionTracker(8, configuration.GetTransitionTrackerParams());

		for (int i = 0; i < 8; i++)
		{
			transitionTracker.SetTransitions(false, true, testData);
		}
		transitionTracker.EvaluateFrameMoment(8, 8, testData);
		EXPECT_TRUE(transitionTracker.getFlashFail());
		EXPECT_TRUE(transitionTracker.getRedFlashFail());
		EXPECT_EQ(FlashResult::FlashFail, testData.redFrameResult);
		EXPECT_EQ(1, transitionTracker.getRedIncidents().flashFailFrames);

	}

	TEST_F(TransitionTrackerByTimeTest, EvaluateFrameMoment_WhenLuminanceAndRedFlashFail_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionTrackerByTime transitionTracker(10, configuration.GetTransitionTrackerParams());

		for (int i = 0; i < 2; i++) //add 2 luminance and red transitions
		{
			transitionTracker.SetTransitions(true, true, testData);
		}
		for (int i = 0; i < 3; i++) //add 3 red transitions
		{
			transitionTracker.SetTransitions(false, true, testData);
		}
		for (int i = 0; i < 5; i++) //add 5 luminance transitions
		{
			transitionTracker.SetTransitions(true, false, testData);
		}
		transitionTracker.EvaluateFrameMoment(10, 10, testData);
		EXPECT_TRUE(transitionTracker.getFlashFail());
		EXPECT_TRUE(transitionTracker.getLumFlashFail());
		EXPECT_FALSE(transitionTracker.getRedFlashFail());

		EXPECT_EQ(FlashResult::FlashFail, testData.luminanceFrameResult);
		EXPECT_EQ(FlashResult::PassWithWarning, testData.redFrameResult);
		EXPECT_EQ(1, transitionTracker.getLuminanceIncidents().flashFailFrames);
		EXPECT_EQ(1, transitionTracker.getRedIncidents().passWithWarningFrames);

	}

	TEST_F(TransitionTrackerByTimeTest, EvaluateFrameMoment_WhenLuminanceFlashPass_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionTrackerByTime transitionTracker(8, configuration.GetTransitionTrackerParams());

		for (int i = 0; i < 3; i++) //add 3 luminance transitions
		{
			transitionTracker.SetTransitions(true, false, testData);
		}

		for (int i = 0; i < 5; i++) //add 5 frames, no transitions
		{
			transitionTracker.SetTransitions(false, false, testData);
		}
		transitionTracker.EvaluateFrameMoment(8, 8, testData);
		EXPECT_FALSE(transitionTracker.getFlashFail());
		EXPECT_EQ(FlashResult::Pass, testData.luminanceFrameResult);
		EXPECT_EQ(FlashResult::Pass, testData.redFrameResult);
	}

	TEST_F(TransitionTrackerByTimeTest, EvaluateFrameMoment_WhenRedFlashPass_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionTrackerByTime transitionTracker(8, configuration.GetTransitionTrackerParams());

		for (int i = 0; i < 3; i++) //add 3 red transitions
		{
			transitionTracker.SetTransitions(false, true, testData);
		}

		for (int i = 0; i < 5; i++) //add 5 frames, no transitions
		{
			transitionTracker.SetTransitions(false, false, testData);
		}
		transitionTracker.EvaluateFrameMoment(8, 8, testData);
		EXPECT_FALSE(transitionTracker.getFlashFail());
		EXPECT_EQ(FlashResult::Pass, testData.luminanceFrameResult);
		EXPECT_EQ(FlashResult::Pass, testData.redFrameResult);
	}

	TEST_F(TransitionTrackerByTimeTest, EvaluateFrameMoment_WhenLuminanceAndRedFlashPass_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionTrackerByTime transitionTracker(8, configuration.GetTransitionTrackerParams());

		for (int i = 0; i < 3; i++) //add 3 luminance and red transitions
		{
			transitionTracker.SetTransitions(true, true, testData);
		}

		for (int i = 0; i < 5; i++) //add 5 frames, no transitions
		{
			transitionTracker.SetTransitions(false, false, testData);
		}
		transitionTracker.EvaluateFrameMoment(8, 8, testData);
		EXPECT_FALSE(transitionTracker.getFlashFail());
		EXPECT_EQ(FlashResult::Pass, testData.luminanceFrameResult);
		EXPECT_EQ(FlashResult::Pass, testData.redFrameResult);
	}

	//TEST_F(TransitionTrackerByTimeTest, EvaluateFrameMoment_WhenExtendedFailure_Test)
	//{
	//	//Only luminance transitions
	//	FrameData testData;
	//	TransitionTrackerByTime transitionTracker(5, configuration.GetTransitionTrackerParams());

	//	for (int i = 0; i < 23; i++) //add luminance and red transitions
	//	{
	//		testData.TimeStampVal += 200;
	//		transitionTracker.SetTransitions(true, true, testData);
	//		transitionTracker.EvaluateFrameMoment(i, 5, testData); 
	//	}
	//	EXPECT_TRUE(transitionTracker.getExtendedFailure());
	//	EXPECT_TRUE(transitionTracker.getLumExtendedFailure());
	//	EXPECT_TRUE(transitionTracker.getRedExtendedFailure());
	//	EXPECT_EQ(FlashResult::ExtendedFail, testData.luminanceFrameResult);
	//	EXPECT_EQ(FlashResult::ExtendedFail, testData.redFrameResult);

	//	EXPECT_EQ(0, transitionTracker.getLuminanceIncidents().flashFailFrames);
	//	EXPECT_EQ(1, transitionTracker.getLuminanceIncidents().extendedFailFrames);
	//	EXPECT_EQ(19, transitionTracker.getLuminanceIncidents().passWithWarningFrames);
	//	EXPECT_EQ(0, transitionTracker.getRedIncidents().flashFailFrames);
	//	EXPECT_EQ(1, transitionTracker.getRedIncidents().extendedFailFrames);
	//	EXPECT_EQ(19, transitionTracker.getRedIncidents().passWithWarningFrames);
	//}

	TEST_F(TransitionTrackerByTimeTest, TotalTransictionCount_WhenLuminanceAndRed_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionTrackerByTime transitionTracker(4, configuration.GetTransitionTrackerParams());

		for (int i = 0; i < 5; i++) //add 5 luminance and red transitions
		{
			transitionTracker.SetTransitions(true, true, testData);
		}
		for (int i = 0; i < 3; i++) //add 3 frames, no transitions
		{
			transitionTracker.SetTransitions(false, false, testData);
		}
		for (int i = 0; i < 4; i++) //add 4 luminance transitions
		{
			transitionTracker.SetTransitions(true, false, testData);
		}
		for (int i = 0; i < 6; i++) //add 6 red transitions
		{
			transitionTracker.SetTransitions(false, true, testData);
		}

		EXPECT_EQ(9, testData.LuminanceTransitions);
		EXPECT_EQ(11, testData.RedTransitions);
	}

	/*TEST_F(TransitionTrackerByTimeTest, EvaluateSecond_WhenTimesBeteewnFramesNotEqual_Test)
	{
		//Only luminance transitions
		FrameData testData;
		TransitionTrackerByTime transitionTracker(5, configuration.GetTransitionTrackerParams());

		int frameNum = 0;
		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 0;
		transitionTracker.EvaluateFrameMoment(frameNum, 5, testData);

		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 150;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);

		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 500;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);

		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 750;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);

		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 1000;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);



		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 1150;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);

		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 1350;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);

		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 1600;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);

		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 1950;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);

		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 2000;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);



		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 2200;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);

		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 2450;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);

		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 2800;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);

		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 2900;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);

		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 3000;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);


		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 3450;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);

		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 3800;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);

		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 3850;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);

		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 3950;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);

		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 4000;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);



		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 4250;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);

		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 4600;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);

		transitionTracker.SetTransitions(true, true, testData);
		testData.TimeStampVal = 4850;
		transitionTracker.EvaluateFrameMoment(frameNum++, 5, testData);

		
		EXPECT_TRUE(transitionTracker.getExtendedFailure());
		EXPECT_TRUE(transitionTracker.getLumExtendedFailure());
		EXPECT_TRUE(transitionTracker.getRedExtendedFailure());
		EXPECT_EQ(FlashResult::ExtendedFail, testData.luminanceFrameResult);
		EXPECT_EQ(FlashResult::ExtendedFail, testData.redFrameResult);

		EXPECT_EQ(0, transitionTracker.getLuminanceIncidents().flashFailFrames);
		EXPECT_EQ(1, transitionTracker.getLuminanceIncidents().extendedFailFrames);
		EXPECT_EQ(19, transitionTracker.getLuminanceIncidents().passWithWarningFrames);
		EXPECT_EQ(0, transitionTracker.getRedIncidents().flashFailFrames);
		EXPECT_EQ(1, transitionTracker.getRedIncidents().extendedFailFrames);
		EXPECT_EQ(19, transitionTracker.getRedIncidents().passWithWarningFrames);
	}*/
}