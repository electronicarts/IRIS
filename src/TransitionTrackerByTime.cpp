#include "TransitionTrackerByTime.h"

namespace iris
{
	TransitionTrackerByTime::TransitionTrackerByTime(unsigned int starterFpsReserve, TransitionTrackerParams* params)
	{
		m_params = params;
		m_luminanceTransitionCount.count.reserve(starterFpsReserve); m_luminanceTransitionCount.count.emplace_back(0);
		m_redTransitionCount.count.reserve(starterFpsReserve); m_redTransitionCount.count.emplace_back(0);
		m_luminanceExtendedCount.count.reserve(m_params->extendedFailWindow * starterFpsReserve); m_luminanceExtendedCount.count.emplace_back(0);
		m_redExtendedCount.count.reserve(m_params->extendedFailWindow * starterFpsReserve); m_redExtendedCount.count.emplace_back(0);

		m_oneSecondTimeStamps.frameTimeStamp.reserve(starterFpsReserve + 1);
		m_oneSecondTimeStamps.timesBetweenPairOfFrames.reserve(starterFpsReserve);

		m_fiveSecondTimeStamps.frameTimeStamp.reserve(m_params->extendedFailSeconds * starterFpsReserve + 1);
		m_fiveSecondTimeStamps.timesBetweenPairOfFrames.reserve(m_params->extendedFailWindow * starterFpsReserve);

		m_fourSecondTimeStamps.frameTimeStamp.reserve(m_params->extendedFailSeconds * starterFpsReserve + 1);
		m_fourSecondTimeStamps.timesBetweenPairOfFrames.reserve(m_params->extendedFailWindow * starterFpsReserve);

		m_oneSecondTimeStamps.SetTimeBarrier(1);
		m_fiveSecondTimeStamps.SetTimeBarrier(m_params->extendedFailWindow);
		m_fourSecondTimeStamps.SetTimeBarrier(m_params->extendedFailSeconds);

	}

	void TransitionTrackerByTime::SetFirstFrame(FrameData& data) 
	{
		m_oneSecondTimeStamps.AddNewFrame(data.TimeStampVal, 0);
		m_fiveSecondTimeStamps.AddNewFrame(data.TimeStampVal, 0);
		m_fourSecondTimeStamps.AddNewFrame(data.TimeStampVal, 0);
	}

	void TransitionTrackerByTime::SetTransitions(bool lumTransition, bool redTransition, FrameData& data)
	{
		m_oneSecondFramesToRemove = m_oneSecondTimeStamps.GetFrameNumToRemove(data.TimeStampVal);
		m_fiveSecondFramesToRemove = m_fiveSecondTimeStamps.GetFrameNumToRemove(data.TimeStampVal);
		m_fourSecondTimeStamps.GetFrameNumToRemove(data.TimeStampVal);

		//update transition lists as 1s has passed
		for (m_oneSecondFramesToRemove; m_oneSecondFramesToRemove > 0; m_oneSecondFramesToRemove--)
		{
			m_luminanceTransitionCount.updatePassed();
			m_redTransitionCount.updatePassed();
		}
		//update extended failure as 5s have passed			 	
		for (m_fiveSecondFramesToRemove; m_fiveSecondFramesToRemove > 0; m_fiveSecondFramesToRemove--)
		{
			m_luminanceExtendedCount.updatePassed();
			m_redExtendedCount.updatePassed();
		}


		data.LuminanceTransitions = m_luminanceTransitionCount.updateCurrent(lumTransition);
		data.RedTransitions = m_redTransitionCount.updateCurrent(redTransition);

		data.LuminanceExtendedFailCount = m_luminanceExtendedCount.updateCurrent(m_luminanceTransitionCount.current <= m_params->maxTransitions && m_luminanceTransitionCount.current >= m_params->minTransitions);
		data.RedExtendedFailCount = m_redExtendedCount.updateCurrent(m_redTransitionCount.current <= m_params->maxTransitions && m_redTransitionCount.current >= m_params->minTransitions);

	}

	void TransitionTrackerByTime::EvaluateFrameMoment(int framePos, int fps, FrameData& data)
	{
		int framesInFourSeconds = m_oneSecondTimeStamps.timesBetweenPairOfFrames.size() * m_params->extendedFailSeconds; //only works on videos, must found a solution to get the exact number of frames in the last 4 seconds

		if (m_luminanceTransitionCount.current > m_params->maxTransitions) //FAIL as max allowed transitions has been surpassed
		{
			m_luminanceResults.flashFail = true;
			data.luminanceFrameResult = FlashResult::FlashFail;
			m_luminanceIncidents.flashFailFrames += 1;
		}
		else if (m_luminanceExtendedCount.current >= framesInFourSeconds && m_luminanceTransitionCount.current >= 4) //EXTENDED FAILURE
		{
			m_luminanceResults.extendedFail = true;
			data.luminanceFrameResult = FlashResult::ExtendedFail;
			m_luminanceIncidents.extendedFailFrames += 1;
		}
		else if (m_luminanceTransitionCount.current >= 4)
		{
			m_luminanceResults.passWithWarning = true;
			data.luminanceFrameResult = FlashResult::PassWithWarning;
			m_luminanceIncidents.passWithWarningFrames += 1;
		}

		if (m_redTransitionCount.current > m_params->maxTransitions) //FAIL as max allowed transitions has been surpassed
		{
			m_redResults.flashFail = true;
			data.redFrameResult = FlashResult::FlashFail;
			m_redIncidents.flashFailFrames += 1;
		}
		else if (m_redExtendedCount.current >= framesInFourSeconds && m_redTransitionCount.current >= 4) //EXTENDED FAILURE
		{
			m_redResults.extendedFail = true;
			data.redFrameResult = FlashResult::ExtendedFail;
			m_redIncidents.extendedFailFrames += 1;
		}
		else if (m_redTransitionCount.current >= 4)
		{
			m_redResults.passWithWarning = true;
			data.redFrameResult = FlashResult::PassWithWarning;
			m_redIncidents.passWithWarningFrames += 1;
		}
	}
}