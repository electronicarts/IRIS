//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include "IrisLibTest.h"
#include <gtest/gtest.h>
#include "TimeFrameManager.h"

namespace iris::Tests
{
	class FrameManagerTest : public IrisLibTest 
	{
		void SetUp() override
		{
		}
	};

	TEST_F(FrameManagerTest, Half_Second_Removal)
	{
		//          (5 frames)
		//	0    125   250   375   500 = frame entry time stamps
		//	|-----|-----|-----|-----|  = time line
		//	0     1     2     3     4  = frame index
		// 
		// As no more frames can be added to the window, the first one must be removed.
		// 
		//	Final result (1 frame removed = number of elements to be removed from the relevant vectors):
		// 125   250   375   500 = frame entry time stamps
		//	|-----|-----|-----|  = time line
		//	1     2     3     4  = frame index

		TimeFrameManager frameManager{};

		FrameData data;
		float timeBarrier = 0.5; //half a second window
		int framesToAdd = 4;

		int index = frameManager.RegisterManager(0, timeBarrier);
		float timeBetweenFrames = (timeBarrier * 1000.0) / framesToAdd; // 500 / 4 = 125 ms

		//frame 0
		frameManager.AddFrame(data);

		//fill out with 4 new frames
		for (int i = 0; i < framesToAdd; i++)
		{
			data.TimeStampVal += timeBetweenFrames;
			frameManager.AddFrame(data);
		}

		EXPECT_EQ(framesToAdd, frameManager.GetCurrentFrameNum(index));
		EXPECT_EQ(1, frameManager.GetFramesToRemove(index));

	}

	TEST_F(FrameManagerTest, Frame_Drop)
	{
		// A.        (5 Frames)
		//	0    250   500   750            1750 = frame entry time stamps
		//	|-----|-----|-----|---------------|  = time line
		//	0     1     2     3               4  = frame index
		// 
		// As the last frames is as big as the window, all frames have to be removed, except for the last one.
		// 
		// Frame drop result (4 frames removed = number of elements to be removed from the relevant vectors):
		// 1750    = frame entry time stamps
		//	|---   = time line
		//	4      = frame index
		// 
		// B. Then 4 frames are added to fill the window and test the correct functionality after a frame drop
		// 
		// Final result (1 frame removed = number of elements to be removed from the relevant vectors):
		// 2000  2250  2500  2750 = frame entry time stamps
		//	|-----|-----|-----|   = time line
		//	5     6     7     8   = frame index

		TimeFrameManager frameManager{};

		FrameData data;
		float timeBarrier = 1.0;  //one second window

		int index = frameManager.RegisterManager(0, timeBarrier);
		float timeBetweenFrames = 250.0;

		//A.
		//frame 0
		frameManager.AddFrame(data);

		//3 new frames
		for (int i = 0; i < 3; i++)
		{
			data.TimeStampVal += timeBetweenFrames;
			frameManager.AddFrame(data);
		}

		//1sec frame drop
		data.TimeStampVal += 1000;
		frameManager.AddFrame(data);

		EXPECT_EQ(1, frameManager.GetCurrentFrameNum(index));
		EXPECT_EQ(4, frameManager.GetFramesToRemove(index));

		//B. fill out with 4 new frames
		for (int i = 0; i < 4; i++)
		{
			data.TimeStampVal += timeBetweenFrames;
			frameManager.AddFrame(data);
		}

		EXPECT_EQ(4, frameManager.GetCurrentFrameNum(index));
		EXPECT_EQ(1, frameManager.GetFramesToRemove(index));
	}

	TEST_F(FrameManagerTest, Reset_Window)
	{
		//  A.    (4 frames)
		//	0    125   250   375 = frame entry time stamps
		//	|-----|-----|-----|  = time line
		//	0     1     2     3  = frame index
		// 
		// B. Window reset
		// 
		// Result after reset:
		// 375    = frame entry time stamps
		//	|---  = time line
		//	4     = frame index

		TimeFrameManager frameManager{};

		FrameData data;
		float timeBarrier = 0.5; //half a second window
		int framesToAdd = 3;

		int index = frameManager.RegisterManager(0, timeBarrier);
		float timeBetweenFrames = 250;

		//A.
		//frame 0
		frameManager.AddFrame(data);

		//fill out with 4 new frames
		for (int i = 0; i < framesToAdd; i++)
		{
			data.TimeStampVal += timeBetweenFrames;
			frameManager.AddFrame(data);
		}

		//B. Window reset
		frameManager.ResetManager(index);
		
		EXPECT_EQ(1, frameManager.GetCurrentFrameNum(index));
		EXPECT_EQ(0, frameManager.GetFramesToRemove(index));

	}

}