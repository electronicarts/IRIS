//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "TransitionTracker.h"
#include "ConfigurationParams.h"
#include "FrameData.h"
#include <chrono>
#include <iostream>
#include "iris/Log.h"

namespace iris
{
	class TransitionTrackerByTime :public TransitionTracker {
	public:
		TransitionTrackerByTime(unsigned int starterFpsReserve, TransitionTrackerParams* params);
		

		void SetTransitions(bool lumTransition, bool redTransition, FrameData& data) override;

		/// <summary>
		/// Checks if in the current frame (moment of the video) the video has 
		/// failed the flash criteria
		/// </summary>
		/// <param name="framePos"> current frame index </param>
		/// <param name="fps">video frame rate</param>
		/// <param name="data">data to persist</param>
		void EvaluateFrameMoment(int framePos, int fps, FrameData& data) override;

		/// <summary>
		/// Add the first frame to the FrameTimeStamps structs
		/// </summary>
		/// <param name="data">data to persist</param>
		void SetFirstFrame(FrameData& data)override;

	private:

		struct FrameTimeStamps
		{
			std::vector<float> frameTimeStamp; //vector of current frame entry times frames in the window
												
			std::vector<float> timesBetweenPairOfFrames; //vector of current times between frames in the window

			float timesSum = 0; //total sum of times between each pair of Frames 

			/// <summary>
			/// Set time barrier in milliseconds 
			/// </summary>
			/// <param name="seconds"> time barrier in seconds </param>
			void SetTimeBarrier(int seconds) {
				timeBarrier = seconds * 1000;
			}
			
			/// <summary>
			/// Check if when adding a new frame it is needed to update frameTimeStamp and timesBetweenPairOfFrames vectors (timeBarrier has been surpassed),
			/// then add newFrameTime to frameTimeStamp and timesBetweenPairOfFrames vectors.
			/// This function returns an integer (number of frames to remove from the count vectors)
			/// </summary>
			/// <param name="newFrameTime"> new frame MS </param>
			int GetFrameNumToRemove(const unsigned long & newFrameTime)
			{
				int framesToRemove = 0; //number of frames to that are no longer inside the time window
				long newTimeBetFrames = 0;//time between frame to add and last recorded frame time stamp

				if (frameTimeStamp.size() > 0) 
				{
					newTimeBetFrames= newFrameTime - frameTimeStamp[frameTimeStamp.size() - 1];
					
					if (timesSum + newTimeBetFrames == timeBarrier) 
					{
						framesToRemove++;
					}
					//Remove frames until new frame fits inside the window
					while (timesSum + newTimeBetFrames > timeBarrier)
					{
						timesSum -= timesBetweenPairOfFrames[0];
						frameTimeStamp.erase(frameTimeStamp.begin());
						timesBetweenPairOfFrames.erase(timesBetweenPairOfFrames.begin());
						
						framesToRemove++;
					}
				}
				AddNewFrame(newFrameTime, newTimeBetFrames);
				
				return framesToRemove;
			}

			/// <summary>
			/// add the new frame time in the frameTimeStamp and the timesBetweenPairOfFrames vectors
			/// also update the timesSum (times between frames) variable
			/// </summary>
			/// <param name="newFrameTime"> new frame MS </param>
			/// <param name="newTimeBetFrames"> time between frame to add and last recorded frame time stamp </param>
			void AddNewFrame(const unsigned long &  newFrameTime, const unsigned long& newTimeBetFrames)
			{
				if (frameTimeStamp.size() > 0)
				{
					timesSum += newTimeBetFrames;
					timesBetweenPairOfFrames.emplace_back(newTimeBetFrames);
				}
				frameTimeStamp.emplace_back(newFrameTime);
			}
		private:
			//Time barrier measured in seconds
			//Used to know when timesBetweenPairOfFrames vector sum has surpased the desired second window and vectors needs to be updated
			int timeBarrier; 
		};
		

		FrameTimeStamps m_oneSecondTimeStamps; //1 second window

		FrameTimeStamps m_fiveSecondTimeStamps; //5 second window

		FrameTimeStamps m_fourSecondTimeStamps; //4 second window

		int m_oneSecondFramesToRemove=0 , m_fiveSecondFramesToRemove = 0; //Number of frames to be removed from the count vector of the Counter structs


	};

}
