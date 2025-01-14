//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <vector>
#include "iris/TotalFlashIncidents.h"

namespace iris
{

	struct TransitionTrackerParams;
	class FrameData;
	class IFrameManager;

	class TransitionTracker
	{

	public:
		
		TransitionTracker(unsigned int fps, TransitionTrackerParams* params, IFrameManager* frameManager);

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
		/// <param name="framePos"> current frame index </param>
		void SetTransitions(bool lumTransition, bool redTransition, FrameData& data, const int& framePos = 0);

		/// <summary>
		/// Checks if in the current frame (moment of the video) the video has failed the flash criteria
		/// </summary>
		/// <param name="data">data to persist</param>
		void EvaluateFrameMoment(FrameData& data);

		/// <summary>
		/// If the new frame exceeds the frame rate, it is necessary to remove 1 or more elements from the counters.
		/// </summary>
		/// <param name="framePos"> current frame index </param>
		void UpdateCounters(const int& framePos);
	protected:

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
				//Real-time only. If a big frame drop occurs
				if (count.empty())
				{
					count.emplace_back(newTransition);
					current = newTransition;
					return newTransition;
				}
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

				//Real-time only. If a big frame drop occurs
				if (count.empty())
				{
					passed = 0;
					current = 0;
				}
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

		IFrameManager* m_frameManager = nullptr;

		const int FAIL_TIME_WINDOW = 1; //window to calculate flash frequency (1s)
		int m_failManagerIndx;

		const int EXTENDED_FAIL_SECONDS = 4; //time at which extended failure can occur (4s)
		int m_extendedSecManagerIndx;

		const int EXTENDED_FAIL_WINDOW = 5; //max time window for extended failure (5s)
		int m_extendedWinManagerIndx;
	};

}