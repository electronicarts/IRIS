//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include "FlashDetection.h"
#include "iris/Configuration.h"
#include "utils/FrameConverter.h"
#include "RelativeLuminance.h"
#include "CDLuminance.h"
#include "RedSaturation.h"
#include <opencv2/core.hpp>
#include "FrameData.h"
#include "IrisFrame.h"
#include "iris/Log.h"
#include "iris/Result.h"

namespace iris
{
	FlashDetection::FlashDetection(Configuration* configuration, const short& fps, const cv::Size& frameSize)
		: m_transitionEvaluator(fps, configuration->GetTransitionEvaluatorParams()), m_sRgbConverter(configuration->GetFrameSrgbConverterParams()), m_fps(fps)

	{
		if (configuration->GetLuminanceType() == Configuration::LuminanceType::RELATIVE)
		{
			m_luminance = new RelativeLuminance(fps, frameSize, configuration->GetLuminanceFlashParams());
		}
		else
		{
			m_cdLuminanceConverter = new EA::EACC::Utils::FrameConverter(configuration->GetFrameCDLuminanceConverterParams());
			m_luminance = new CDLuminance(m_cdLuminanceConverter, fps, frameSize, configuration->GetLuminanceFlashParams());
		}

		m_redSaturation = new RedSaturation(fps, frameSize, configuration->GetRedSaturationFlashParams());
		//m_luminance->CalculateSrgbValues();
	}

	FlashDetection::~FlashDetection()
	{
		if (m_luminance != nullptr)
		{
			delete m_luminance; m_luminance = nullptr;
		}
		if (m_redSaturation != nullptr)
		{
			delete m_redSaturation; m_redSaturation = nullptr;
		}
		if (m_cdLuminanceConverter != nullptr)
		{
			delete m_cdLuminanceConverter; m_cdLuminanceConverter = nullptr;
		}
	}

	void FlashDetection::setLuminance(IrisFrame& irisFrame)
	{
		m_luminance->SetCurrentFrame(irisFrame);
		irisFrame.luminanceFrame = m_luminance->getCurrentFrame();
	}

	void FlashDetection::checkFrame(const IrisFrame& irisFrame, const int& framePos, FrameData& data)
	{
		//red saturarion
		m_redSaturation->SetCurrentFrame(irisFrame.sRgbFrame);

		data.LuminanceAverage = m_luminance->GetFrameMean();
		data.RedAverage = m_redSaturation->GetFrameMean();

		if (framePos != 0) //check difference between frame(n) and frame (n - 1)
		{
			frameDifference(data);
			m_transitionEvaluator.EvaluateSecond(framePos, m_fps, data);
		}

		data.AverageLuminanceDiffAcc = m_lastAvgLumDiffAcc;
		data.AverageRedDiffAcc = m_lastAvgRedDiffAcc;
	}

	void FlashDetection::frameDifference(FrameData& data)
	{
		//Red Saturation difference 
		cv::Mat* redSaturationDiff = m_redSaturation->FrameDifference();
		
		//Luminance difference
		cv::Mat* luminanceDiff = m_luminance->FrameDifference();
		
		float averageLuminaceDiff = m_luminance->CheckSafeArea(luminanceDiff);
		float averageRedDiff = m_redSaturation->CheckSafeArea(redSaturationDiff);

		Flash::CheckTransitionResult redTranstion = m_redSaturation->CheckTransition(averageRedDiff, m_lastAvgRedDiffAcc);
		m_lastAvgRedDiffAcc = redTranstion.lastAvgDiffAcc;

		Flash::CheckTransitionResult luminanceTransition = m_luminance->CheckTransition(averageLuminaceDiff, m_lastAvgLumDiffAcc);
		m_lastAvgLumDiffAcc = luminanceTransition.lastAvgDiffAcc;

		//Evaluate and count new transitions
		m_transitionEvaluator.SetTransitions(luminanceTransition.checkResult, redTranstion.checkResult, data);

		data.LuminanceFlashArea = data.proportionToPercentage(m_luminance->GetFlashArea());
		data.RedFlashArea = data.proportionToPercentage(m_redSaturation->GetFlashArea());

		data.AverageLuminanceDiff = averageLuminaceDiff;
		data.AverageRedDiff = averageRedDiff;

		delete redSaturationDiff;
		delete luminanceDiff;
	}
	
	bool FlashDetection::isFail()
	{
		if (m_transitionEvaluator.getFlashFail())
		{
			LOG_CORE_INFO("Flash FAIL");
		}

		if (m_transitionEvaluator.getExtendedFailure())
		{
			LOG_CORE_INFO("Extended Failure");
		}

		return m_transitionEvaluator.getFlashFail() || m_transitionEvaluator.getExtendedFailure();
	}

	cv::Mat* FlashDetection::getLuminanceFrame()
	{
		return m_luminance->getCurrentFrame();
	}

	void FlashDetection::setResult(Result& result)
	{
		//set incident counters
		result.totalLuminanceIncidents = m_transitionEvaluator.getLuminanceIncidents();
		result.totalRedIncidents = m_transitionEvaluator.getRedIncidents();

		//set overall result and results vector
		if (m_transitionEvaluator.getLumFlashFail())
		{
			result.OverallResult = AnalysisResult::Fail;
			result.Results.emplace_back(AnalysisResult::LuminanceFlashFailure);

			LOG_CORE_INFO("Luminance Flash FAIL");
		}

		if (m_transitionEvaluator.getLumExtendedFailure())
		{
			result.OverallResult = AnalysisResult::Fail;
			result.Results.emplace_back(AnalysisResult::LuminanceExtendedFlashFailure);
			LOG_CORE_INFO("Luminance Extended Failure");
		}

		if (m_transitionEvaluator.getRedFlashFail())
		{
			result.OverallResult = AnalysisResult::Fail;
			result.Results.emplace_back(AnalysisResult::RedFlashFailure);

			LOG_CORE_INFO("Red Flash FAIL");
		}

		if (m_transitionEvaluator.getRedExtendedFailure())
		{
			result.OverallResult = AnalysisResult::Fail;
			result.Results.emplace_back(AnalysisResult::RedExtendedFlashFailure);
			LOG_CORE_INFO("Red Extended Failure");
		}

		if (result.OverallResult != AnalysisResult::Fail && ( m_transitionEvaluator.getLumPassWithWarning() || m_transitionEvaluator.getRedPassWithWarning() ))
		{
			result.OverallResult = AnalysisResult::PassWithWarning;
			LOG_CORE_INFO("Pass with Warning");
		}

	}
}