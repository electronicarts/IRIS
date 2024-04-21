//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

/*
#pragma once
#include <vector>
#include "iris/TotalFlashIncidents.h"

namespace iris
{

struct TransitionTrackerParams;
class FrameData;

class TransitionEvaluator
{

public:
	TransitionEvaluator(unsigned int fps, TransitionTrackerParams* params);
	~TransitionEvaluator();
	
	inline bool getLumPassWithWarning() { return m_luminanceResults.passWithWarning; };
	inline bool getRedPassWithWarning() { return m_redResults.passWithWarning; };


	inline bool getFlashFail() { return m_luminanceResults.flashFail || m_redResults.flashFail; };
	inline bool getLumFlashFail() { return m_luminanceResults.flashFail; };
	inline bool getRedFlashFail() { return m_redResults.flashFail; };
		
	inline bool getExtendedFailure() { return m_luminanceResults.extendedFail || m_redResults.extendedFail; };
	inline bool getLumExtendedFailure() { return m_luminanceResults.extendedFail; };
	inline bool getRedExtendedFailure() { return m_redResults.extendedFail; };

	const TotalFlashIncidents& getLuminanceIncidents() { return m_luminanceIncidents; };
	const TotalFlashIncidents& getRedIncidents() { return m_redIncidents; };
		
	/// <summary>
	/// Updates the transition count, extended fail count and the transitions in the last second
	/// from the current frame
	/// </summary>
	/// <param name="lumTransition">true if there is a new luminance transition</param>
	/// <param name="redTransition">true if there is a new red transition</param>
	/// <param name="data">data to persist</param>
	void SetTransitions(bool lumTransition, bool redTransition, FrameData& data);
		
	/// <summary>
	/// Checks if in the current frame (moment of the video) the video has failed the flash criteria
	/// </summary>
	/// <param name="framePos"> current frame index </param>
	/// <param name="fps">video frame rate</param>
	/// <param name="data">data to persist</param>
	void EvaluateSecond(int framePos, int fps, FrameData& data);

private:
	
	struct Counter
	{
		std::vector<int> count; //transition count that surpass the luminance/red threshold from the last second 
								//or frames count where the transitions where between 4 and 6
		int current = 0; //current luminance/red transitions from this moment up to one second before
						 //or current frame count for extended failure
		int passed = 0; //amount of luminance/red transitions that have passed the 1s window 
						//or amount of frames that have passed the 5s window

		// Get current frame's luminance or red transitions from the last second and update the transition count vector
		// or get the current frame's luminance or red extended failure count
		// return new current
		int updateCurrent(const bool& newTransition)
		{
			//update the new transition count
			if (newTransition)
			{
				count.emplace_back(count.back() + 1);
			}
			else
			{
				count.emplace_back(count.back());
			}

			current = count.back() - passed; //current transitions in second
			return current;
		}

		void updatePassed()
		{
			passed = count.front();
			count.erase(count.begin());
		}
	};

	struct FlashResults //possible flash results
	{
		bool passWithWarning = false; //flash pass with warning status
		bool flashFail = false; //flash fail status
		bool extendedFail = false; //extended flash failure status
	};

	Counter m_luminanceTransitionCount;
	Counter m_redTransitionCount;

	Counter m_luminanceExtendedCount;
	Counter m_redExtendedCount;

	FlashResults m_luminanceResults;
	FlashResults m_redResults;

	TotalFlashIncidents m_luminanceIncidents;
	TotalFlashIncidents m_redIncidents;

	TransitionTrackerParams* m_params = nullptr;
};

}
*/