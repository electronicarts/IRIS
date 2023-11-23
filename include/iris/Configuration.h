//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "utils/JsonWrapper.h"

namespace EA::EACC::Utils
{
	struct FrameConverterParams;
}

namespace iris
{
	struct FlashParams;
	struct TransitionEvaluatorParams;
	struct PatternDetectionParams;
	struct PatternAnalyserParams;
	struct StraightLineDetectorParams;
	struct CircularLineDetectorParams;
	struct RiskEstimationParams;

	class Configuration
	{
	public:
		Configuration();
		~Configuration();

		/// <summary>
		/// Initializes configuration parameters
		/// </summary>
		/// <param name="path">path to appsettings files</param>
		void Init(const char* path = "");

		enum class LuminanceType {UN_SET = 0, CD, RELATIVE};

		inline void SetLuminanceType(LuminanceType luminance) { m_luminanceType = luminance; }
		inline void SetLuminanceType(const std::string& luminance) { m_luminanceType = luminance == "CD" ?LuminanceType::CD : LuminanceType::RELATIVE; }
		LuminanceType GetLuminanceType() { return m_luminanceType; }

		float GetSafeAreaProportion();

		inline FlashParams* GetLuminanceFlashParams() { return m_luminanceFlashParams; }
		inline FlashParams* GetRedSaturationFlashParams() { return m_redSaturationFlashParams; }
		inline EA::EACC::Utils::FrameConverterParams* GetFrameSrgbConverterParams() { return m_frameSrgbConverterParams; }
		inline EA::EACC::Utils::FrameConverterParams* GetFrameCDLuminanceConverterParams() { return m_frameCDLuminanceConverterParams; }
		inline TransitionEvaluatorParams* GetTransitionEvaluatorParams() { return m_transitionEvaluatorParams; }
		inline PatternDetectionParams* GetPatternDetectionParams() { return m_patternDetectionParams; }

		inline bool PatternDetectionEnabled() { return m_patternDetectionEnabled; }
		inline void SetPatternDetectionStatus(bool status) { m_patternDetectionEnabled = status; }

		void SetSafeArea(float areaProportion);

		inline const std::string& GetResultsPath() { return m_resultsPath; }
		void SetResultsPath(const std::string& resultsPath) { m_resultsPath = resultsPath; };
	private:

		FlashParams* m_luminanceFlashParams = nullptr;
		FlashParams* m_redSaturationFlashParams = nullptr;
		EA::EACC::Utils::FrameConverterParams* m_frameSrgbConverterParams = nullptr;
		EA::EACC::Utils::FrameConverterParams* m_frameCDLuminanceConverterParams = nullptr;
		TransitionEvaluatorParams* m_transitionEvaluatorParams = nullptr;
		PatternDetectionParams* m_patternDetectionParams = nullptr;

		LuminanceType m_luminanceType = LuminanceType::UN_SET;
		bool m_patternDetectionEnabled = true;
		std::string m_resultsPath;
	};
}