//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "TransitionTracker.h"
#include "ConfigurationParams.h"
#include "FrameData.h"
#include <iostream>

namespace iris
{
	class TransitionTrackerByFPS :public TransitionTracker {

	public:
		TransitionTrackerByFPS(unsigned int fps, TransitionTrackerParams* params);

		void SetTransitions(bool lumTransition, bool redTransition, FrameData& data) override;

		/// <summary>
		/// Checks if in the current frame (moment of the video) the video has 
		/// failed the flash criteria
		/// </summary>
		/// <param name="framePos"> current frame index </param>
		/// <param name="fps">video frame rate</param>
		/// <param name="data">data to persist</param>
		void EvaluateFrameMoment(int framePos, int fps, FrameData& data) override;
	};
}