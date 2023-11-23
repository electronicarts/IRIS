//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include "IrisLibTest.h"
#include <gtest/gtest.h>
#include "TransitionEvaluator.h"
#include "FrameData.h"

namespace iris::Tests
{
	class TransitionEvaluatorTest : public IrisLibTest {
	protected:

	};

	TEST_F(TransitionEvaluatorTest, Transition_WhenOnlyLuminance_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionEvaluator transitionEvaluator(8, configuration.GetTransitionEvaluatorParams());

		transitionEvaluator.SetTransitions(true, false, testData);
		transitionEvaluator.SetTransitions(true, false, testData);
		transitionEvaluator.SetTransitions(false, false, testData);
		transitionEvaluator.SetTransitions(true, false, testData);
		transitionEvaluator.SetTransitions(false, false, testData);
		transitionEvaluator.SetTransitions(false, false, testData);
		transitionEvaluator.SetTransitions(true, false, testData);

		EXPECT_EQ(4, testData.LuminanceTransitions);
		EXPECT_EQ(0, testData.RedTransitions);
	}

	TEST_F(TransitionEvaluatorTest, Transition_WhenOnlyRed_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionEvaluator transitionEvaluator(8, configuration.GetTransitionEvaluatorParams());

		transitionEvaluator.SetTransitions(false, false, testData);
		transitionEvaluator.SetTransitions(false, true, testData);
		transitionEvaluator.SetTransitions(false, false, testData);
		transitionEvaluator.SetTransitions(false, true, testData);
		transitionEvaluator.SetTransitions(false, false, testData);
		transitionEvaluator.SetTransitions(false, true, testData);
		transitionEvaluator.SetTransitions(false, true, testData);

		EXPECT_EQ(0, testData.LuminanceTransitions);
		EXPECT_EQ(4, testData.RedTransitions);
	}

	TEST_F(TransitionEvaluatorTest, Transition_WhenLuminanceAndRed_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionEvaluator transitionEvaluator(8, configuration.GetTransitionEvaluatorParams());

		transitionEvaluator.SetTransitions(false, false, testData);
		transitionEvaluator.SetTransitions(true, true, testData);
		transitionEvaluator.SetTransitions(false, false, testData);
		transitionEvaluator.SetTransitions(false, true, testData);
		transitionEvaluator.SetTransitions(true, false, testData);
		transitionEvaluator.SetTransitions(false, true, testData);
		transitionEvaluator.SetTransitions(true, true, testData);

		EXPECT_EQ(3, testData.LuminanceTransitions);
		EXPECT_EQ(4, testData.RedTransitions);
	}

	TEST_F(TransitionEvaluatorTest, EvaluateSecond_WhenLuminanceFlashFail_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionEvaluator transitionEvaluator(8, configuration.GetTransitionEvaluatorParams());

		for (int i = 0; i < 8; i++)
		{
			transitionEvaluator.SetTransitions(true, false, testData);
		}
		transitionEvaluator.EvaluateSecond(8, 8, testData);
		EXPECT_TRUE(transitionEvaluator.getFlashFail());
		EXPECT_TRUE(transitionEvaluator.getLumFlashFail());
		EXPECT_EQ(FlashResult::FlashFail, testData.luminanceFrameResult);
		EXPECT_EQ(1, transitionEvaluator.getLuminanceIncidents().flashFailFrames);
	}

	TEST_F(TransitionEvaluatorTest, EvaluateSecond_WhenRedFlashFail_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionEvaluator transitionEvaluator(8, configuration.GetTransitionEvaluatorParams());

		for (int i = 0; i < 8; i++)
		{
			transitionEvaluator.SetTransitions(false, true, testData);
		}
		transitionEvaluator.EvaluateSecond(8, 8, testData);
		EXPECT_TRUE(transitionEvaluator.getFlashFail());
		EXPECT_TRUE(transitionEvaluator.getRedFlashFail());
		EXPECT_EQ(FlashResult::FlashFail, testData.redFrameResult);
		EXPECT_EQ(1, transitionEvaluator.getRedIncidents().flashFailFrames);

	}

	TEST_F(TransitionEvaluatorTest, EvaluateSecond_WhenLuminanceAndRedFlashFail_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionEvaluator transitionEvaluator(10, configuration.GetTransitionEvaluatorParams());

		for (int i = 0; i < 2; i++) //add 2 luminance and red transitions
		{
			transitionEvaluator.SetTransitions(true, true, testData);
		}
		for (int i = 0; i < 3; i++) //add 3 red transitions
		{
			transitionEvaluator.SetTransitions(false, true, testData);
		}
		for (int i = 0; i < 5; i++) //add 5 luminance transitions
		{
			transitionEvaluator.SetTransitions(true, false, testData);
		}
		transitionEvaluator.EvaluateSecond(10, 10, testData);
		EXPECT_TRUE(transitionEvaluator.getFlashFail());
		EXPECT_TRUE(transitionEvaluator.getLumFlashFail());
		EXPECT_FALSE(transitionEvaluator.getRedFlashFail());

		EXPECT_EQ(FlashResult::FlashFail, testData.luminanceFrameResult);
		EXPECT_EQ(FlashResult::PassWithWarning, testData.redFrameResult);
		EXPECT_EQ(1, transitionEvaluator.getLuminanceIncidents().flashFailFrames);
		EXPECT_EQ(1, transitionEvaluator.getRedIncidents().passWithWarningFrames);

	}

	TEST_F(TransitionEvaluatorTest, EvaluateSecond_WhenLuminanceFlashPass_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionEvaluator transitionEvaluator(8, configuration.GetTransitionEvaluatorParams());

		for (int i = 0; i < 3; i++) //add 3 luminance transitions
		{
			transitionEvaluator.SetTransitions(true, false, testData);
		}

		for (int i = 0; i < 5; i++) //add 5 frames, no transitions
		{
			transitionEvaluator.SetTransitions(false, false, testData);
		}
		transitionEvaluator.EvaluateSecond(8, 8, testData);
		EXPECT_FALSE(transitionEvaluator.getFlashFail());
		EXPECT_EQ(FlashResult::Pass, testData.luminanceFrameResult);
		EXPECT_EQ(FlashResult::Pass, testData.redFrameResult);
	}

	TEST_F(TransitionEvaluatorTest, EvaluateSecond_WhenRedFlashPass_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionEvaluator transitionEvaluator(8, configuration.GetTransitionEvaluatorParams());

		for (int i = 0; i < 3; i++) //add 3 red transitions
		{
			transitionEvaluator.SetTransitions(false, true, testData);
		}

		for (int i = 0; i < 5; i++) //add 5 frames, no transitions
		{
			transitionEvaluator.SetTransitions(false, false, testData);
		}
		transitionEvaluator.EvaluateSecond(8, 8, testData);
		EXPECT_FALSE(transitionEvaluator.getFlashFail());
		EXPECT_EQ(FlashResult::Pass, testData.luminanceFrameResult);
		EXPECT_EQ(FlashResult::Pass, testData.redFrameResult);
	}

	TEST_F(TransitionEvaluatorTest, EvaluateSecond_WhenLuminanceAndRedFlashPass_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionEvaluator transitionEvaluator(8, configuration.GetTransitionEvaluatorParams());

		for (int i = 0; i < 3; i++) //add 3 luminance and red transitions
		{
			transitionEvaluator.SetTransitions(true, true, testData);
		}

		for (int i = 0; i < 5; i++) //add 5 frames, no transitions
		{
			transitionEvaluator.SetTransitions(false, false, testData);
		}
		transitionEvaluator.EvaluateSecond(8, 8, testData);
		EXPECT_FALSE(transitionEvaluator.getFlashFail());
		EXPECT_EQ(FlashResult::Pass, testData.luminanceFrameResult);
		EXPECT_EQ(FlashResult::Pass, testData.redFrameResult);
	}

	TEST_F(TransitionEvaluatorTest, EvaluateSecond_WhenExtendedFailure_Test)
	{
		//Only luminance transitions
		FrameData testData;
		TransitionEvaluator transitionEvaluator(5, configuration.GetTransitionEvaluatorParams());

		for (int i = 0; i < 23; i++) //add luminance and red transitions
		{
			transitionEvaluator.SetTransitions(true, true, testData);
			transitionEvaluator.EvaluateSecond(i, 5, testData);
		}
		EXPECT_TRUE(transitionEvaluator.getExtendedFailure());
		EXPECT_TRUE(transitionEvaluator.getLumExtendedFailure());
		EXPECT_TRUE(transitionEvaluator.getRedExtendedFailure());
		EXPECT_EQ(FlashResult::ExtendedFail, testData.luminanceFrameResult);
		EXPECT_EQ(FlashResult::ExtendedFail, testData.redFrameResult);

		EXPECT_EQ(0, transitionEvaluator.getLuminanceIncidents().flashFailFrames);
		EXPECT_EQ(1, transitionEvaluator.getLuminanceIncidents().extendedFailFrames);
		EXPECT_EQ(19, transitionEvaluator.getLuminanceIncidents().passWithWarningFrames);
		EXPECT_EQ(0, transitionEvaluator.getRedIncidents().flashFailFrames);
		EXPECT_EQ(1, transitionEvaluator.getRedIncidents().extendedFailFrames);
		EXPECT_EQ(19, transitionEvaluator.getRedIncidents().passWithWarningFrames);
	}

	TEST_F(TransitionEvaluatorTest, TotalTransictionCount_WhenLuminanceAndRed_Test)
	{
		//Only luminance transitions
		FrameData testData = FrameData();
		TransitionEvaluator transitionEvaluator(4, configuration.GetTransitionEvaluatorParams());

		for (int i = 0; i < 5; i++) //add 5 luminance and red transitions
		{
			transitionEvaluator.SetTransitions(true, true, testData);
		}
		for (int i = 0; i < 3; i++) //add 3 frames, no transitions
		{
			transitionEvaluator.SetTransitions(false, false, testData);
		}
		for (int i = 0; i < 4; i++) //add 4 luminance transitions
		{
			transitionEvaluator.SetTransitions(true, false, testData);
		}
		for (int i = 0; i < 6; i++) //add 6 red transitions
		{
			transitionEvaluator.SetTransitions(false, true, testData);
		}

		EXPECT_EQ(9, testData.LuminanceTransitions);
		EXPECT_EQ(11, testData.RedTransitions);
	}
}