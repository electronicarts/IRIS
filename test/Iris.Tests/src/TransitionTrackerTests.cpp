//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include "IrisLibTest.h"
#include <gtest/gtest.h>
#include "TransitionTracker.h"
#include "iris/FrameData.h"
#include "FpsFrameManager.h"
#include "TimeFrameManager.h"

namespace iris::Tests
{

class TransitionTrackerTest : public IrisLibTest 
{

protected:
	IFrameManager* frameManager;

	void SetUp() override 
	{
		IrisLibTest::SetUp();
	}

	TransitionTracker GetTransitionTracker(short fps, bool timeAnalys = false)
	{
		if (timeAnalys) frameManager = new TimeFrameManager();
		else frameManager = new FpsFrameManager();

		return TransitionTracker(fps, configuration.GetTransitionTrackerParams(), frameManager);
	}

	~TransitionTrackerTest() override
	{
		delete frameManager;
	}
};

TEST_F(TransitionTrackerTest, Transition_WhenOnlyLuminance_Test)
{
	//Only luminance transitions
	FrameData testData = FrameData();
	TransitionTracker transitionTracker = GetTransitionTracker(8);

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

TEST_F(TransitionTrackerTest, Transition_WhenOnlyRed_Test)
{
	//Only luminance transitions
	FrameData testData = FrameData();
	TransitionTracker transitionTracker = GetTransitionTracker(8);

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

TEST_F(TransitionTrackerTest, Transition_WhenLuminanceAndRed_Test)
{
	//Only luminance transitions
	FrameData testData = FrameData();
	TransitionTracker transitionTracker = GetTransitionTracker(8);

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

TEST_F(TransitionTrackerTest, EvaluateFrameMoment_WhenLuminanceFlashFail_Test)
{
	//Only luminance transitions
	FrameData testData = FrameData();
	TransitionTracker transitionTracker = GetTransitionTracker(8);

	for (int i = 0; i < 8; i++)
	{
		transitionTracker.SetTransitions(true, false, testData, i);
	}
	transitionTracker.EvaluateFrameMoment(testData);
	EXPECT_TRUE(transitionTracker.getFlashFail());
	EXPECT_TRUE(transitionTracker.getLumFlashFail());
	EXPECT_EQ(FlashResult::FlashFail, testData.luminanceFrameResult);
	EXPECT_EQ(1, transitionTracker.getLuminanceIncidents().flashFailFrames);
}

TEST_F(TransitionTrackerTest, EvaluateFrameMoment_WhenRedFlashFail_Test)
{
	//Only luminance transitions
	FrameData testData = FrameData();
	TransitionTracker transitionTracker = GetTransitionTracker(8);

	for (int i = 0; i < 8; i++)
	{
		transitionTracker.SetTransitions(false, true, testData, i);
	}
	transitionTracker.EvaluateFrameMoment(testData);
	EXPECT_TRUE(transitionTracker.getFlashFail());
	EXPECT_TRUE(transitionTracker.getRedFlashFail());
	EXPECT_EQ(FlashResult::FlashFail, testData.redFrameResult);
	EXPECT_EQ(1, transitionTracker.getRedIncidents().flashFailFrames);

}

TEST_F(TransitionTrackerTest, EvaluateFrameMoment_WhenLuminanceAndRedFlashFail_Test)
{
	//Only luminance transitions
	FrameData testData = FrameData();
	TransitionTracker transitionTracker = GetTransitionTracker(10);
	int frameNum = -1;

	for (int i = 0; i < 2; i++) //add 2 luminance and red transitions
	{
		transitionTracker.SetTransitions(true, true, testData, frameNum++);
	}
	for (int i = 0; i < 3; i++) //add 3 red transitions
	{
		transitionTracker.SetTransitions(false, true, testData, frameNum++);
	}
	for (int i = 0; i < 5; i++) //add 5 luminance transitions
	{
		transitionTracker.SetTransitions(true, false, testData, frameNum++);
	}
	transitionTracker.EvaluateFrameMoment(testData);
	EXPECT_TRUE(transitionTracker.getFlashFail());
	EXPECT_TRUE(transitionTracker.getLumFlashFail());
	EXPECT_FALSE(transitionTracker.getRedFlashFail());

	EXPECT_EQ(FlashResult::FlashFail, testData.luminanceFrameResult);
	EXPECT_EQ(FlashResult::PassWithWarning, testData.redFrameResult);
	EXPECT_EQ(1, transitionTracker.getLuminanceIncidents().flashFailFrames);
	EXPECT_EQ(1, transitionTracker.getRedIncidents().passWithWarningFrames);

}

TEST_F(TransitionTrackerTest, EvaluateFrameMoment_WhenLuminanceFlashPass_Test)
{
	//Only luminance transitions
	FrameData testData = FrameData();
	TransitionTracker transitionTracker = GetTransitionTracker(8);
	int frameNum = -1;

	for (int i = 0; i < 3; i++) //add 3 luminance transitions
	{
		transitionTracker.SetTransitions(true, false, testData, frameNum++);
	}

	for (int i = 0; i < 5; i++) //add 5 frames, no transitions
	{
		transitionTracker.SetTransitions(false, false, testData, frameNum++);
	}
	transitionTracker.EvaluateFrameMoment(testData);
	EXPECT_FALSE(transitionTracker.getFlashFail());
	EXPECT_EQ(FlashResult::Pass, testData.luminanceFrameResult);
	EXPECT_EQ(FlashResult::Pass, testData.redFrameResult);
}

TEST_F(TransitionTrackerTest, EvaluateFrameMoment_WhenRedFlashPass_Test)
{
	//Only luminance transitions
	FrameData testData = FrameData();
	TransitionTracker transitionTracker = GetTransitionTracker(8);
	int frameNum = -1;

	for (int i = 0; i < 3; i++) //add 3 red transitions
	{
		transitionTracker.SetTransitions(false, true, testData, frameNum++);
	}

	for (int i = 0; i < 5; i++) //add 5 frames, no transitions
	{
		transitionTracker.SetTransitions(false, false, testData, frameNum++);
	}
	transitionTracker.EvaluateFrameMoment(testData);
	EXPECT_FALSE(transitionTracker.getFlashFail());
	EXPECT_EQ(FlashResult::Pass, testData.luminanceFrameResult);
	EXPECT_EQ(FlashResult::Pass, testData.redFrameResult);
}

TEST_F(TransitionTrackerTest, EvaluateFrameMoment_WhenLuminanceAndRedFlashPass_Test)
{
	//Only luminance transitions
	FrameData testData = FrameData();
	TransitionTracker transitionTracker = GetTransitionTracker(8);
	int frameNum = 0;

	for (int i = 0; i < 3; i++) //add 3 luminance and red transitions
	{
		transitionTracker.SetTransitions(true, true, testData, frameNum++);
	}

	for (int i = 0; i < 5; i++) //add 5 frames, no transitions
	{
		transitionTracker.SetTransitions(false, false, testData, frameNum++);
	}
	transitionTracker.EvaluateFrameMoment(testData);
	EXPECT_FALSE(transitionTracker.getFlashFail());
	EXPECT_EQ(FlashResult::Pass, testData.luminanceFrameResult);
	EXPECT_EQ(FlashResult::Pass, testData.redFrameResult);
}

TEST_F(TransitionTrackerTest, EvaluateFrameMoment_WhenExtendedFailure_Test)
{
	//Only luminance transitions
	FrameData testData;
	TransitionTracker transitionTracker = GetTransitionTracker(5);
	
	for (int i = 0; i < 23; i++) //add luminance and red transitions
	{
		frameManager->AddFrame(testData);
		transitionTracker.SetTransitions(true, true, testData, i);
		transitionTracker.EvaluateFrameMoment(testData);
	}
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
}

TEST_F(TransitionTrackerTest, TotalTransictionCount_WhenLuminanceAndRed_Test)
{
	//Only luminance transitions
	FrameData testData = FrameData();
	TransitionTracker transitionTracker = GetTransitionTracker(4);

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

TEST_F(TransitionTrackerTest, RealTime_EvaluateFrameMoment_WhenLuminanceAndRedFlashFail)
{
	//Only luminance transitions
	FrameData testData = FrameData();
	TransitionTracker transitionTracker = GetTransitionTracker(10, true);
	int frameNum = -1;

	for (int i = 0; i < 2; i++) //add 2 luminance and red transitions
	{
		frameManager->AddFrame(testData);
		transitionTracker.SetTransitions(true, true, testData, frameNum++);
	}
	for (int i = 0; i < 3; i++) //add 3 red transitions
	{
		frameManager->AddFrame(testData);
		transitionTracker.SetTransitions(false, true, testData, frameNum++);
	}
	for (int i = 0; i < 5; i++) //add 5 luminance transitions
	{
		frameManager->AddFrame(testData);
		transitionTracker.SetTransitions(true, false, testData, frameNum++);
	}
	transitionTracker.EvaluateFrameMoment(testData);
	EXPECT_TRUE(transitionTracker.getFlashFail());
	EXPECT_TRUE(transitionTracker.getLumFlashFail());
	EXPECT_FALSE(transitionTracker.getRedFlashFail());

	EXPECT_EQ(FlashResult::FlashFail, testData.luminanceFrameResult);
	EXPECT_EQ(FlashResult::PassWithWarning, testData.redFrameResult);
	EXPECT_EQ(1, transitionTracker.getLuminanceIncidents().flashFailFrames);
	EXPECT_EQ(1, transitionTracker.getRedIncidents().passWithWarningFrames);

}

TEST_F(TransitionTrackerTest, RealTime_EvaluateSecond_ExtendedFailWhenTimesBeteewnFramesNotEqual)
{
	//Only luminance transitions
	FrameData testData;
	TransitionTracker transitionTracker = GetTransitionTracker(5, true);

	int frameNum = 0;
	testData.TimeStampVal = 0;
	frameManager->AddFrame(testData);

	transitionTracker.SetTransitions(true, true, testData, frameNum);
	transitionTracker.EvaluateFrameMoment(testData);

	for (int i = 0; i < 4; i++)
	{
		testData.TimeStampVal += 200;
		frameManager->AddFrame(testData);
		transitionTracker.SetTransitions(true, true, testData, frameNum++);
		transitionTracker.EvaluateFrameMoment(testData);

		testData.TimeStampVal += 300;
		frameManager->AddFrame(testData);
		transitionTracker.SetTransitions(true, true, testData, frameNum++);
		transitionTracker.EvaluateFrameMoment(testData);

		testData.TimeStampVal += 100;
		frameManager->AddFrame(testData);
		transitionTracker.SetTransitions(true, true, testData, frameNum++);
		transitionTracker.EvaluateFrameMoment(testData);

		testData.TimeStampVal += 250;
		frameManager->AddFrame(testData);
		transitionTracker.SetTransitions(true, true, testData, frameNum++);
		transitionTracker.EvaluateFrameMoment(testData);

		testData.TimeStampVal += 150;
		frameManager->AddFrame(testData);
		transitionTracker.SetTransitions(true, true, testData, frameNum++);
		transitionTracker.EvaluateFrameMoment(testData);
	}

	testData.TimeStampVal += 200;
	frameManager->AddFrame(testData);
	transitionTracker.SetTransitions(true, true, testData, frameNum++);
	transitionTracker.EvaluateFrameMoment(testData);

	testData.TimeStampVal += 300;
	frameManager->AddFrame(testData);
	transitionTracker.SetTransitions(true, true, testData, frameNum++);
	transitionTracker.EvaluateFrameMoment(testData);


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
}

TEST_F(TransitionTrackerTest, RealTime_EvaluateSecond_ExtendedFailFramerateFluctuations)
{
	//Only luminance transitions
	FrameData testData;
	TransitionTracker transitionTracker = GetTransitionTracker(5, true);
	int frameNum = 0;

	//1st 1s window (5 transitions - 4 FPS)
	//First frame should not set any transition/evaluate moment (analysis by time)
	testData.TimeStampVal = 0;
	frameManager->AddFrame(testData);

	for (int i = 0; i < 4; i++)
	{
		testData.TimeStampVal += 250;
		frameManager->AddFrame(testData);
		transitionTracker.SetTransitions(true, true, testData, frameNum++);
		transitionTracker.EvaluateFrameMoment(testData);
	}

	//2nd 1s window (1 transition - 1FPS)
	testData.TimeStampVal += 1000;
	frameManager->AddFrame(testData);
	transitionTracker.SetTransitions(true, true, testData, frameNum++);
	transitionTracker.EvaluateFrameMoment(testData);


	//3th 1s window (5 transitions - 6 FPS)
	for (int i = 0; i < 2; i++)
	{
		testData.TimeStampVal += 200;
		frameManager->AddFrame(testData);
		transitionTracker.SetTransitions(true, true, testData, frameNum++);
		transitionTracker.EvaluateFrameMoment(testData);

		testData.TimeStampVal += 100;
		frameManager->AddFrame(testData);
		transitionTracker.SetTransitions(true, true, testData, frameNum++);
		transitionTracker.EvaluateFrameMoment(testData);
	}

	testData.TimeStampVal += 400;
	frameManager->AddFrame(testData);
	transitionTracker.SetTransitions(true, true, testData, frameNum++);
	transitionTracker.EvaluateFrameMoment(testData);

	//4th 1s window (4 transitions - 5 FPS)
	testData.TimeStampVal += 100;
	frameManager->AddFrame(testData);
	transitionTracker.SetTransitions(true, true, testData, frameNum++);
	transitionTracker.EvaluateFrameMoment(testData);

	testData.TimeStampVal += 150;
	frameManager->AddFrame(testData);
	transitionTracker.SetTransitions(true, true, testData, frameNum++);
	transitionTracker.EvaluateFrameMoment(testData);

	testData.TimeStampVal += 250;
	frameManager->AddFrame(testData);
	transitionTracker.SetTransitions(true, true, testData, frameNum++);
	transitionTracker.EvaluateFrameMoment(testData);

	testData.TimeStampVal += 500;
	frameManager->AddFrame(testData);
	transitionTracker.SetTransitions(true, true, testData, frameNum++);
	transitionTracker.EvaluateFrameMoment(testData);

	//5th 1s window (5 transitions - 6 FPS)

	testData.TimeStampVal += 200;
	frameManager->AddFrame(testData);
	transitionTracker.SetTransitions(true, true, testData, frameNum++);
	transitionTracker.EvaluateFrameMoment(testData);

	testData.TimeStampVal += 100;
	frameManager->AddFrame(testData);
	transitionTracker.SetTransitions(true, true, testData, frameNum++);
	transitionTracker.EvaluateFrameMoment(testData);

	testData.TimeStampVal += 100;
	frameManager->AddFrame(testData);
	transitionTracker.SetTransitions(true, true, testData, frameNum++);
	transitionTracker.EvaluateFrameMoment(testData);

	testData.TimeStampVal += 300;
	frameManager->AddFrame(testData);
	transitionTracker.SetTransitions(true, true, testData, frameNum++);
	transitionTracker.EvaluateFrameMoment(testData);

	testData.TimeStampVal += 300;
	frameManager->AddFrame(testData);
	transitionTracker.SetTransitions(true, true, testData, frameNum++);
	transitionTracker.EvaluateFrameMoment(testData);

	//6th 1s window (4 transitions - 5 FPS)
	testData.TimeStampVal += 200;
	frameManager->AddFrame(testData);
	transitionTracker.SetTransitions(true, true, testData, frameNum++);
	transitionTracker.EvaluateFrameMoment(testData);

	testData.TimeStampVal += 100;
	frameManager->AddFrame(testData);
	transitionTracker.SetTransitions(true, true, testData, frameNum++);
	transitionTracker.EvaluateFrameMoment(testData);

	testData.TimeStampVal += 300;
	frameManager->AddFrame(testData);
	transitionTracker.SetTransitions(true, true, testData, frameNum++);
	transitionTracker.EvaluateFrameMoment(testData);

	testData.TimeStampVal += 400;
	frameManager->AddFrame(testData);
	transitionTracker.SetTransitions(true, true, testData, frameNum++);
	transitionTracker.EvaluateFrameMoment(testData);


	//7th 1s window (3 transitions to trigger the ExtendedFail)

	testData.TimeStampVal += 150;
	frameManager->AddFrame(testData);
	transitionTracker.SetTransitions(true, true, testData, frameNum++);
	transitionTracker.EvaluateFrameMoment(testData);

	testData.TimeStampVal += 300;
	frameManager->AddFrame(testData);
	transitionTracker.SetTransitions(true, true, testData, frameNum++);
	transitionTracker.EvaluateFrameMoment(testData);

	EXPECT_TRUE(transitionTracker.getExtendedFailure());
	EXPECT_TRUE(transitionTracker.getLumExtendedFailure());
	EXPECT_TRUE(transitionTracker.getRedExtendedFailure());
	EXPECT_EQ(FlashResult::ExtendedFail, testData.luminanceFrameResult);
	EXPECT_EQ(FlashResult::ExtendedFail, testData.redFrameResult);

	EXPECT_EQ(0, transitionTracker.getLuminanceIncidents().flashFailFrames);
	EXPECT_EQ(1, transitionTracker.getLuminanceIncidents().extendedFailFrames);
	EXPECT_EQ(18, transitionTracker.getLuminanceIncidents().passWithWarningFrames);
	EXPECT_EQ(0, transitionTracker.getRedIncidents().flashFailFrames);
	EXPECT_EQ(1, transitionTracker.getRedIncidents().extendedFailFrames);
	EXPECT_EQ(18, transitionTracker.getRedIncidents().passWithWarningFrames);
}

}