#include "TransitionTrackerByFPS.h"

namespace iris
{
	TransitionTrackerByFPS::TransitionTrackerByFPS(unsigned int fps, TransitionTrackerParams* params)
		: TransitionTracker()	
	{
		m_params = params;
		m_luminanceTransitionCount.count.reserve(fps); m_luminanceTransitionCount.count.emplace_back(0);
		m_redTransitionCount.count.reserve(fps); m_redTransitionCount.count.emplace_back(0);
		m_luminanceExtendedCount.count.reserve(m_params->extendedFailWindow * fps); m_luminanceExtendedCount.count.emplace_back(0);
		m_redExtendedCount.count.reserve(m_params->extendedFailWindow * fps); m_redExtendedCount.count.emplace_back(0);
	}

	void TransitionTrackerByFPS::SetTransitions(bool lumTransition, bool redTransition, FrameData& data)
	{
		data.LuminanceTransitions = m_luminanceTransitionCount.updateCurrent(lumTransition);
		data.RedTransitions = m_redTransitionCount.updateCurrent(redTransition);

		data.LuminanceExtendedFailCount = m_luminanceExtendedCount.updateCurrent(m_luminanceTransitionCount.current <= m_params->maxTransitions && m_luminanceTransitionCount.current >= m_params->minTransitions);
		data.RedExtendedFailCount = m_redExtendedCount.updateCurrent(m_redTransitionCount.current <= m_params->maxTransitions && m_redTransitionCount.current >= m_params->minTransitions);
	}

	void TransitionTrackerByFPS::EvaluateFrameMoment(int framePos, int fps, FrameData& data)
	{
		if (m_luminanceTransitionCount.current > m_params->maxTransitions) //FAIL as max allowed transitions has been surpassed
		{
			m_luminanceResults.flashFail = true;
			data.luminanceFrameResult = FlashResult::FlashFail;
			m_luminanceIncidents.flashFailFrames += 1;
		}
		else if (m_luminanceExtendedCount.current >= m_params->extendedFailSeconds * fps && m_luminanceTransitionCount.current >= 4) //EXTENDED FAILURE
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
		else if (m_redExtendedCount.current >= m_params->extendedFailSeconds * fps && m_redTransitionCount.current >= 4) //EXTENDED FAILURE
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

		//update transition lists as 1s has passed
		if (framePos >= fps - 1)
		{
			m_luminanceTransitionCount.updatePassed();
			m_redTransitionCount.updatePassed();

			// update extended failure as 5s have passed
			if (framePos >= m_params->extendedFailWindow * fps - 1)
			{
				m_luminanceExtendedCount.updatePassed();
				m_redExtendedCount.updatePassed();
			}
		}
	}
}