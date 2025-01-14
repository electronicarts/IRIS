//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include "TransitionTracker.h"
#include "IFrameManager.h"
#include "iris/FrameData.h"
#include "ConfigurationParams.h"

namespace iris
{
	TransitionTracker::TransitionTracker(unsigned int fps, TransitionTrackerParams* params, IFrameManager* frameManager) 
		: m_params(params), m_frameManager(frameManager)
	{
		m_luminanceTransitionCount.count.reserve(fps); 
		m_redTransitionCount.count.reserve(fps); 
		m_luminanceExtendedCount.count.reserve(EXTENDED_FAIL_WINDOW * fps); 
		m_redExtendedCount.count.reserve(EXTENDED_FAIL_WINDOW * fps);

		m_redTransitionCount.count.emplace_back(0);
		m_luminanceExtendedCount.count.emplace_back(0);
		m_luminanceTransitionCount.count.emplace_back(0);
		m_redExtendedCount.count.emplace_back(0);

		m_failManagerIndx = m_frameManager->RegisterManager(fps, FAIL_TIME_WINDOW);
		m_extendedSecManagerIndx = m_frameManager->RegisterManager(EXTENDED_FAIL_SECONDS * fps, EXTENDED_FAIL_SECONDS);
		m_extendedWinManagerIndx = m_frameManager->RegisterManager(EXTENDED_FAIL_WINDOW * fps, EXTENDED_FAIL_WINDOW);
		
	}

	void TransitionTracker::SetTransitions(bool lumTransition, bool redTransition, FrameData& data, const int& framePos)
	{
		UpdateCounters(framePos);

		data.LuminanceTransitions = m_luminanceTransitionCount.updateCurrent(lumTransition);
		data.RedTransitions = m_redTransitionCount.updateCurrent(redTransition);

		data.LuminanceExtendedFailCount = m_luminanceExtendedCount.updateCurrent(
			m_luminanceTransitionCount.current <= m_params->maxTransitions 
			&& m_luminanceTransitionCount.current >= m_params->minTransitions);
		
		data.RedExtendedFailCount = m_redExtendedCount.updateCurrent(
			m_redTransitionCount.current <= m_params->maxTransitions 
			&& m_redTransitionCount.current >= m_params->minTransitions);
	}

	void TransitionTracker::EvaluateFrameMoment(FrameData& data)
	{
		int framesInFourSeconds = m_frameManager->GetCurrentFrameNum(m_extendedSecManagerIndx);

		if (m_luminanceTransitionCount.current > m_params->maxTransitions) //FAIL as max allowed transitions has been surpassed
		{
			m_luminanceResults.flashFail = true;
			data.luminanceFrameResult = FlashResult::FlashFail;
			m_luminanceIncidents.flashFailFrames += 1;
		}
		else if (m_luminanceExtendedCount.current >= framesInFourSeconds && m_luminanceTransitionCount.current >= m_params->minTransitions) //EXTENDED FAILURE
		{
			m_luminanceResults.extendedFail = true;
			data.luminanceFrameResult = FlashResult::ExtendedFail;
			m_luminanceIncidents.extendedFailFrames += 1;
		}
		else if (m_luminanceTransitionCount.current >= m_params->warningTransitions)
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
		else if (m_redExtendedCount.current >= framesInFourSeconds && m_redTransitionCount.current >= m_params->minTransitions) //EXTENDED FAILURE
		{
			m_redResults.extendedFail = true;
			data.redFrameResult = FlashResult::ExtendedFail;
			m_redIncidents.extendedFailFrames += 1;
		}
		else if (m_redTransitionCount.current >= m_params->warningTransitions)
		{
			m_redResults.passWithWarning = true;
			data.redFrameResult = FlashResult::PassWithWarning;
			m_redIncidents.passWithWarningFrames += 1;
		}
	}

	void TransitionTracker::UpdateCounters(const int& framePos)
	{
		int oneSecondFramesToRemove = m_frameManager->GetFramesToRemove(m_failManagerIndx);
		int fiveSecondFramesToRemove = m_frameManager->GetFramesToRemove(m_extendedWinManagerIndx);

		//update transition counters as 1s has passed
		for (oneSecondFramesToRemove; oneSecondFramesToRemove > 0; oneSecondFramesToRemove--)
		{
			m_luminanceTransitionCount.updatePassed();
			m_redTransitionCount.updatePassed();
		}
		//update extended failure counters as 5s have passed			 	
		for (fiveSecondFramesToRemove; fiveSecondFramesToRemove > 0; fiveSecondFramesToRemove--)
		{
			m_luminanceExtendedCount.updatePassed();
			m_redExtendedCount.updatePassed();
		}
	}
}