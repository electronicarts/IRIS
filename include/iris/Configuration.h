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
	struct TransitionTrackerParams;
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

		float GetSafeAreaProportion();

		inline FlashParams* GetLuminanceFlashParams() { return m_luminanceFlashParams; }
		inline FlashParams* GetRedSaturationFlashParams() { return m_redSaturationFlashParams; }
		inline EA::EACC::Utils::FrameConverterParams* GetFrameSrgbConverterParams() { return m_frameSrgbConverterParams; }
		inline TransitionTrackerParams* GetTransitionTrackerParams() { return m_transitionTrackerParams; }
		inline PatternDetectionParams* GetPatternDetectionParams() { return m_patternDetectionParams; }

		inline bool PatternDetectionEnabled() { return m_patternDetectionEnabled; }
		inline void SetPatternDetectionStatus(bool status) { m_patternDetectionEnabled = status; }
		
		inline bool AnalyseByTimeEnabled() { return m_analyseByTime; }
		inline void SetAnalyseByTimeStatus(bool status) { m_analyseByTime = status; }

		inline bool FrameResizeEnabled() { return m_frameResizeEnabled; }
		inline void SetFrameResizeEnabled(bool status) { m_frameResizeEnabled = status; }

		inline float GetFrameResizeProportion() { return m_frameResizeProportion; }
		inline void SetFrameResizeProportion(float proportion) { m_frameResizeProportion = proportion; }

		void SetRedSaturationFlashThreshold(float newFlashThreshold);
		void SetRedSaturationDarkThreshold(float newDarkThreshold);

		void SetLuminanceFlashThreshold(float newFlashThreshold);
		void SetLuminanceDarkThreshold(float newDarkThreshold);

		void SetMinimumTransitionsForWarning(int count);

		void SetSafeArea(float areaProportion);

		inline const std::string& GetResultsPath() { return m_resultsPath; }
		void SetResultsPath(const std::string& resultsPath) { m_resultsPath = resultsPath; };
	private:

		FlashParams* m_luminanceFlashParams = nullptr;
		FlashParams* m_redSaturationFlashParams = nullptr;
		EA::EACC::Utils::FrameConverterParams* m_frameSrgbConverterParams = nullptr;
		TransitionTrackerParams* m_transitionTrackerParams = nullptr;
		PatternDetectionParams* m_patternDetectionParams = nullptr;

		bool m_patternDetectionEnabled = true;
		bool m_frameResizeEnabled = true;

		float m_frameResizeProportion = 1;
		bool m_analyseByTime = false;

		std::string m_resultsPath;
	};
}
