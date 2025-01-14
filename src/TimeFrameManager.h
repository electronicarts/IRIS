//Copyright (C) 2024 Electronic Arts, Inc.  All rights reserved.
#pragma once
#include "IFrameManager.h"
#include <vector>

namespace iris
{
class FrameData;

class TimeFrameManager : public IFrameManager
{
public:

	TimeFrameManager() = default;
	virtual ~TimeFrameManager() {};

	/// <summary>
	/// Adds a new manager, then returns the index to access the new elements.
	/// </summary>
	/// <param name="maxFrames:"> max capacity of frames in window </param>
	/// <param name="maxTime:"> max time capacity in window (seconds) </param> 
	virtual int RegisterManager(const int& maxFrames, const float& maxTime) override;

	/// <summary>
	/// Adds a new frame to the manager. If using real-time, the timeStampValue from data is used.
	/// Updates tracking time/fps and framesToRemove.
	/// </summary>
	virtual void AddFrame(const iris::FrameData& data) override;

	/// <summary>
	/// Returns the number of frames that need to be removed from the counter vectors.
	/// </summary>
	/// <param name="index:"> Integer to access the desired element in the vectors. </param>
	virtual int	GetFramesToRemove(const int& index) const override;

	/// <summary>
	/// Returns the number of frames in the window.
	/// </summary>
	/// <param name="index:"> Integer to access the desired element in the vectors. </param>
	virtual int GetCurrentFrameNum(const int& index) const override;

	/// <summary>
	/// Reset the manager.
	/// </summary>
	/// <param name="index:"> Integer to access the desired element in the vectors. </param>
	virtual void ResetManager(const int& index, bool removeLast = false) override;

private:

	/// <summary>
	/// Struct to manage time stamps and time between frames in a specific time window
	/// </summary>
	struct TimeStampManager
	{

		TimeStampManager(const int& maxFrames, const int& maxTime) 
			: maxFrames(maxFrames), maxTime(maxTime), framesToRemove(0), timeSum(0)
		{
			frameTimeStamps.reserve(maxFrames);
			timesBetweenFrames.reserve(maxFrames + (maxFrames <= 0) -1);
		}

		int maxFrames; //max capacity
		int maxTime; //max amount of time in seconds that the window can hold
		int framesToRemove; //number of frames to that are no longer inside the time window
		long long timeSum; //sum of all times between frames in the window

		std::vector<long long> frameTimeStamps; //vector of current frame entry times frames in the window
		std::vector<int> timesBetweenFrames; //vector of current times between frames in the window

		/// <summary>
		/// Add frame to frameTimeStamps checking if it fits in the time window
		/// </summary>
		void AddFrame(const long long& timeStamp)
		{
			if (!frameTimeStamps.empty())
			{
				UpdateTime(timeStamp);
			}
			frameTimeStamps.emplace_back(timeStamp);
		}

		/// <summary>
		/// Updates time keeping related vector and variables
		/// </summary>
		void UpdateTime(const long long& newTimeStamp)
		{
			long long newTimeBetweenFrames = newTimeStamp - frameTimeStamps.back();
			int framesToRemove = 0;

			if (newTimeBetweenFrames >= maxTime)
			{
				framesToRemove = frameTimeStamps.size();
				Reset(true); //clear absolutely everything before adding the new frame
			}	
			else
			{
				//Remove frames until new frame fits inside the time window
				while (timeSum + newTimeBetweenFrames >= maxTime)
				{
					timeSum -= timesBetweenFrames.front();
					frameTimeStamps.erase(frameTimeStamps.begin());
					if (!timesBetweenFrames.empty())
					{
						timesBetweenFrames.erase(timesBetweenFrames.begin());
					}
					framesToRemove++;
				}

				timeSum += newTimeBetweenFrames;
				timesBetweenFrames.emplace_back(newTimeBetweenFrames);
			}

			this->framesToRemove = framesToRemove;
		}

		/// <summary>
		/// Reset the time in the manager
		/// </summary>
		/// <param name="removeLast">To specify if the last frame should remain or should be removed</param>
		void Reset(bool removeLast = false)
		{
			timeSum = 0;
			framesToRemove = 0;
			frameTimeStamps.erase(frameTimeStamps.begin(), frameTimeStamps.end() - 1 + removeLast);
			timesBetweenFrames.clear();
		}
	};

	std::vector<TimeStampManager> m_managers;

};

}