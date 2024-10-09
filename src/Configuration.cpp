//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include "iris/Configuration.h"
#include "iris/Log.h"
#include "ConfigurationParams.h"
#include "utils/FrameConverter.h"
#include <string>

namespace iris
{
	Configuration::Configuration()
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

		if (m_frameCDLuminanceConverterParams != nullptr)
		{
			delete m_frameCDLuminanceConverterParams;  m_frameCDLuminanceConverterParams = nullptr;
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

		if (m_luminanceType == LuminanceType::UN_SET)
		{ 
			std::string lumType = jsonFile.GetParam<std::string>("VideoAnalyser", "LuminanceType");
			m_luminanceType = lumType == "CD" ? LuminanceType::CD : LuminanceType::RELATIVE;
		}

		m_patternDetectionEnabled = jsonFile.GetParam<bool>("VideoAnalyser", "PatternDetectionEnabled");
		m_frameResizeEnabled = jsonFile.GetParam<bool>("VideoAnalyser", "FrameResizeEnabled");

		m_frameResizeProportion = jsonFile.GetParam<float>("VideoAnalyser", "ResizeFrameProportion");
		
		//Luminance
		if (m_luminanceType == LuminanceType::CD)
		{ 
			m_luminanceFlashParams = new FlashParams(
				jsonFile.GetParam<double>("Luminance", "CdLuminanceFlashThreshold"), 
				jsonFile.GetParam<double>("FlashDetection", "AreaProportion"), 
				jsonFile.GetParam<double>("Luminance", "CdDarkLuminanceThreshold"));
		}
		else 
		{ 
			m_luminanceFlashParams = new FlashParams(
				jsonFile.GetParam<double>("Luminance", "RelativeLuminanceFlashThreshold"), 
				jsonFile.GetParam<double>("FlashDetection", "AreaProportion"), 
				jsonFile.GetParam<double>("Luminance", "RelativeDarkLuminanceThreshold"));
		}
		
		//Red Saturation
		m_redSaturationFlashParams = new FlashParams(
			jsonFile.GetParam<double>("RedSaturation", "FlashThreshold"), 
			jsonFile.GetParam<double>("FlashDetection", "AreaProportion"), 
			jsonFile.GetParam<double>("RedSaturation", "RedDarkThreshold"));
		
		//FrameRgbConverter Params 
		m_frameSrgbConverterParams = new EA::EACC::Utils::FrameConverterParams(
			jsonFile.GetVector<double>("FlashDetection", "sRGBValues"));

		//FrameRgbConverter Params for CD Luminance conversion
		if (m_luminanceType == LuminanceType::CD)
		{
			m_frameCDLuminanceConverterParams = new EA::EACC::Utils::FrameConverterParams(
				jsonFile.GetVector<double>("Luminance", "CdLuminanceValues"));
		}

		//Transition Tracker Params
		m_transitionTrackerParams = new TransitionTrackerParams(
			jsonFile.GetParam<uint>("TransitionTracker", "MaxTransitions"), 
			jsonFile.GetParam<uint>("TransitionTracker", "MinTransitions"),
			jsonFile.GetParam<uint>("TransitionTracker", "ExtendedFailSeconds"), 
			jsonFile.GetParam<uint>("TransitionTracker", "ExtendedFailWindow"),
			jsonFile.GetParam<bool>("TransitionTracker", "AnalyseByTime"));

		//Pattern Detection
		int minStripes = jsonFile.GetParam<int>("PatternDetection", "MinStripes");
		double darkLumThreshold = 0.0f;
		
		if (m_luminanceType == LuminanceType::CD)
		{ 
			darkLumThreshold = jsonFile.GetParam<double>("PatternDetection", "CDDarkLuminanceThreshold"); 
		}
		else 
		{ 
			darkLumThreshold = jsonFile.GetParam<double>("PatternDetection", "RelativeDarkLuminanceThreshold"); 
		}

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
}