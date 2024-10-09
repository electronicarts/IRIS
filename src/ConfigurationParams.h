//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <vector>
#include <iterator>

namespace iris
{
	using uint = unsigned int;

	struct FlashParams
	{
		FlashParams(double flashThresh, double areaP, double darkThresh) : flashThreshold(flashThresh), areaProportion(areaP), darkThreshold(darkThresh) {};
		
		//threshold of Red Saturation or Luminace 
		double flashThreshold; //if flash values overpass threshold, a transition has occurred
		double areaProportion; //minimum area of the screen display for a transition to have occurred
		double darkThreshold; //if darker image is above this threshold, a transitions has not occurred
	};

	struct FrameRgbConverterParams
	{
		FrameRgbConverterParams(std::vector<double> sRgbValues) : sRgbValues(sRgbValues) {};

		std::vector<double> sRgbValues; //The input array containing the decimal values for the sRgb convertion
	};

	struct TransitionTrackerParams
	{
		TransitionTrackerParams(uint maxTransition, uint minTransitions, uint extendedFailSeconds, uint extendedFailWindow, bool analyseByTime)
		: maxTransitions(maxTransition), minTransitions(minTransitions), extendedFailSeconds(extendedFailSeconds), extendedFailWindow (extendedFailWindow), analyseByTime(analyseByTime) {};

		uint maxTransitions; //max allowed transitions and max transitions to count for extended fail
		uint minTransitions; //amount of min transitions to add to extended fail count
		uint extendedFailSeconds; //if extendedFailFramesIS reach this value, extended failure has occured
		uint extendedFailWindow; //seconds in extended fail count window
		bool analyseByTime; //realise the flash analysis by time instead of FPS
	};

	struct PatternDetectionParams
	{
		PatternDetectionParams(int minStripes, float darkLuminanceThreshold, float timeThreshold, float areaP)
			: minStripes(minStripes), darkLuminanceThreshold(darkLuminanceThreshold), timeThreshold(timeThreshold),
			areaProportion(areaP) {};

		int minStripes; //min number of light and dark pattern stripes
		float darkLuminanceThreshold; //max luminance of the darker part of a flash/pattern
		float timeThreshold;
		float areaProportion; //max size a harmful pattern can occupy
	};

}