//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include "iris/Configuration.h"
#include "iris/Log.h"
#include "ConfigurationParams.h"
#include "utils/FrameConverter.h"
#include <string>

namespace iris
{
	Configuration::Configuration() : m_analyseByTime(false), m_patternDetectionEnabled(false), 
		m_frameResizeEnabled(false), m_frameResizeProportion(1.0f)
	{
	}

	Configuration::~Configuration()
	{
		if (m_luminanceFlashParams != nullptr)
		{
			delete m_luminanceFlashParams; m_luminanceFlashParams = nullptr;
		}

		if (m_redSaturationFlashParams != nullptr)
		{
			delete m_redSaturationFlashParams; m_redSaturationFlashParams = nullptr;
		}

		if (m_frameSrgbConverterParams != nullptr)
		{
			delete m_frameSrgbConverterParams; m_frameSrgbConverterParams = nullptr;
		}

		if (m_transitionTrackerParams != nullptr)
		{
			delete m_transitionTrackerParams; m_transitionTrackerParams = nullptr;
		}

		if (m_patternDetectionParams != nullptr)
		{
			delete m_patternDetectionParams; m_patternDetectionParams = nullptr;
		}
	}

	void Configuration::Init(const char* path)
	{
		EA::EACC::Utils::JsonWrapper jsonFile;

		jsonFile.OpenFile((std::string(path) + "appsettings.json").c_str()); //load json configuration file

		m_resultsPath = jsonFile.ContainsParam("ResultsPath") ? jsonFile.GetParam<std::string>("ResultsPath") : "Results/";

		m_patternDetectionEnabled = jsonFile.GetParam<bool>("VideoAnalyser", "PatternDetectionEnabled");
		m_frameResizeEnabled = jsonFile.GetParam<bool>("VideoAnalyser", "FrameResizeEnabled");

		m_frameResizeProportion = jsonFile.GetParam<float>("VideoAnalyser", "ResizeFrameProportion");

		m_luminanceFlashParams = new FlashParams(
			jsonFile.GetParam<float>("Luminance", "RelativeLuminanceFlashThreshold"), 
			jsonFile.GetParam<float>("FlashDetection", "AreaProportion"), 
			jsonFile.GetParam<float>("Luminance", "RelativeDarkLuminanceThreshold"));
	
		//Red Saturation
		m_redSaturationFlashParams = new FlashParams(
			jsonFile.GetParam<float>("RedSaturation", "FlashThreshold"), 
			jsonFile.GetParam<float>("FlashDetection", "AreaProportion"), 
			jsonFile.GetParam<float>("RedSaturation", "RedDarkThreshold"));
		
		//FrameRgbConverter Params 
		m_frameSrgbConverterParams = new EA::EACC::Utils::FrameConverterParams(
			jsonFile.GetVector<float>("FlashDetection", "sRGBValues"));

		//Transition Tracker Params
		m_transitionTrackerParams = new TransitionTrackerParams(
			jsonFile.GetParam<uint>("TransitionTracker", "MaxTransitions"), 
			jsonFile.GetParam<uint>("TransitionTracker", "MinTransitions"),
			jsonFile.GetParam<uint>("TransitionTracker", "ExtendedFailSeconds"),
			jsonFile.GetParam<uint>("TransitionTracker", "ExtendedFailWindow"),
			jsonFile.GetParam<uint>("TransitionTracker", "WarningTransitions"));


		//Pattern Detection
		int minStripes = jsonFile.GetParam<int>("PatternDetection", "MinStripes");
		float darkLumThreshold = 0.0f;
		
		darkLumThreshold = jsonFile.GetParam<float>("PatternDetection", "RelativeDarkLuminanceThreshold"); 

		m_patternDetectionParams = new PatternDetectionParams(
			minStripes, 
			darkLumThreshold, 
			jsonFile.GetParam<float>("PatternDetection", "TimeThreshold"),
			jsonFile.GetParam<float>("PatternDetection", "AreaProportion"));
	}

	void Configuration::SetSafeArea(float areaProportion)
	{
		m_luminanceFlashParams->areaProportion = areaProportion;
		m_redSaturationFlashParams->areaProportion = areaProportion;
	}

	float Configuration::GetSafeAreaProportion()
	{
		return m_luminanceFlashParams->areaProportion;
	}

	void Configuration::SetRedSaturationFlashThreshold(float newFlashThreshold) { m_redSaturationFlashParams->flashThreshold = newFlashThreshold; };
	void Configuration::SetRedSaturationDarkThreshold(float newDarkThreshold) { m_redSaturationFlashParams->darkThreshold = newDarkThreshold; };

	void Configuration::SetLuminanceFlashThreshold(float newFlashThreshold) { m_luminanceFlashParams->flashThreshold = newFlashThreshold; };
	void Configuration::SetLuminanceDarkThreshold(float newDarkThreshold) { m_luminanceFlashParams->darkThreshold = newDarkThreshold; };

	void Configuration::SetMinimumTransitionsForWarning(int count) { m_transitionTrackerParams->warningTransitions = count; }
}