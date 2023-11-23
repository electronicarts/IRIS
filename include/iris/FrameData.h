//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <string>
#include <math.h>       /* fmod */
#include "utils/JsonWrapper"
#include "iris/TotalFlashIncidents.h"

namespace iris
{
	static std::string msToTimeSpan(int ms)
	{
		float seconds = fmodf((ms / 1000.0), 60);
		int minutes = floor((ms / (1000 * 60)) % 60);
		int hours = floor((ms / (1000 * 60 * 60)) % 24);

		std::string h = (hours < 10) ? "0" + std::to_string(hours) : std::to_string(hours);
		std::string m = (minutes < 10) ? "0" + std::to_string(minutes) : std::to_string(minutes);
		std::string s = (seconds < 10) ? "0" + std::to_string(seconds) : std::to_string(seconds);

		return h + ":" + m + ":" + s;
	}

	class FrameData
	{
	public:

		FrameData() {};
		FrameData(int frame, long timeMs) : Frame(frame)
		{
			TimeStampMs = msToTimeSpan(timeMs);
		};

		//To convert the area proportion into a percentage
		std::string proportionToPercentage(float proportion)
		{
			std::string str = std::to_string(proportion * 100);
			return str.substr(0, str.find('.') + 3) + '%'; //truncate to two decimal points
		}
		
		std::string ToCSV()
		{
			std::string csvOutput = std::to_string(Frame) 
				+ ',' + TimeStampMs 
				+ ',' + std::to_string(LuminanceAverage) 
				+ ',' + LuminanceFlashArea 
				+ ',' + std::to_string(AverageLuminanceDiff) 
				+ ',' + std::to_string(AverageLuminanceDiffAcc)
				+ ',' + std::to_string(RedAverage)
				+ ',' + RedFlashArea
				+ ',' + std::to_string(AverageRedDiff) 
				+ ',' + std::to_string(AverageRedDiffAcc) 
				+ ',' + std::to_string(LuminanceTransitions) 
				+ ',' + std::to_string(RedTransitions) 
				+ ',' + std::to_string(LuminanceExtendedFailCount)
				+ ',' + std::to_string(RedExtendedFailCount)
				+ ',' + std::to_string((int)luminanceFrameResult)
				+ ',' + std::to_string((int)redFrameResult)
				+ ',' + patternArea
				+ ',' + std::to_string(patternDetectedLines)
				+ ',' + std::to_string((int)patternFrameResult) + '\0';
			return csvOutput;
		}

		std::string CsvColumns()
		{
			std::string propertyNames [] = {
				"Frame",
				"TimeStamp",
				"AverageLuminance",
				"FlashAreaLuminance",
				"AverageLuminanceDiff",
				"AverageLuminanceDiffAcc",
				"AverageRed",
				"FlashAreaRed",
				"AverageRedDiff",
				"AverageRedDiffAcc",
				"LuminanceTransitions",
				"RedTransitions",
				"LuminanceExtendedFailCount",
				"RedExtendedFailCount",
				"LuminanceFrameResult",
				"RedFrameResult",
				"PatternArea",
				"PatternDetectedLines",
				"PatternFrameResult"
			};

			std::string columns;

			bool first = true;
			for (auto property : propertyNames)
			{
				if (!first)
				{
					columns += ',' + property;
				}
				else
				{
					columns += property;
					first = false;
				}
			}

			return columns;
		}

		/// <summary>
		/// Frame index
		/// </summary>
		int Frame = 0;

		/// <summary>
		/// frame timestamp in milliseconds
		/// </summary>
		std::string TimeStampMs = "00:00";
		std::string LuminanceFlashArea = "0.00%";
		float LuminanceAverage = 0;
		float AverageLuminanceDiff = 0;
		float AverageLuminanceDiffAcc = 0;
		std::string RedFlashArea = "0.00%";
		float RedAverage = 0;
		float AverageRedDiff = 0;
		float AverageRedDiffAcc = 0;
		float PatternRisk = 0;
		int LuminanceTransitions = 0;
		int RedTransitions = 0;
		int LuminanceExtendedFailCount = 0;
		int RedExtendedFailCount = 0;
		FlashResult luminanceFrameResult = FlashResult::Pass;
		FlashResult redFrameResult = FlashResult::Pass;
		std::string  patternArea = "0.00%";
		int patternDetectedLines = 0;
		PatternResult patternFrameResult = PatternResult::Pass;
	};

	//Serializes FrameData to Json object
	static void to_json(json& j, const FrameData& data)
	{
		j = json
		{ {"Frame", data.Frame}, 
			{"TimeStampString", data.TimeStampMs}, 
			{"AverageLuminance", data.LuminanceAverage},
			{"FlashAreaLuminance", data.LuminanceFlashArea},
			{"AverageLuminanceDiff", data.AverageLuminanceDiff}, 
			{"AverageLuminanceDiffAcc", data.AverageLuminanceDiffAcc},
			{"AverageRed", data.RedAverage},
			{"FlashAreaRed", data.RedFlashArea},
			{"AverageRedDiff", data.AverageRedDiff}, 
			{"AverageRedDiffAcc", data.AverageRedDiffAcc}, 
			{"LuminanceTransitions", data.LuminanceTransitions},
			{"RedTransitions", data.RedTransitions}, 
			{"LuminanceExtendedFailCount", data.LuminanceExtendedFailCount},
			{"RedExtendedFailCount", data.RedExtendedFailCount},
			{"LuminanceFrameResult", data.luminanceFrameResult}, 
			{"RedFrameResult", data.redFrameResult}, 
			{"PatternArea", data.patternArea}, 
			{"PatternDetectedLines", data.patternDetectedLines}, 
			{"PatternFrameResult", data.patternFrameResult} };
	};

	struct FrameDataJson
	{
		void reserve(const int& size) 
		{
			frame.reserve(size);
			timeStampMs.reserve(size);

			luminanceFlashArea.reserve(size);
			luminanceAverage.reserve(size);
			averageLuminanceDiff.reserve(size);
			averageLuminanceDiffAcc.reserve(size);

			redFlashArea.reserve(size);
			redAverage.reserve(size);
			averageRedDiff.reserve(size);
			averageRedDiffAcc.reserve(size);

			luminanceTransitions.reserve(size);
			redTransitions.reserve(size);

			luminanceExtendedFailCount.reserve(size);
			redExtendedFailCount.reserve(size);
			luminanceFrameResult.reserve(size);
			redFrameResult.reserve(size);

			patternArea.reserve(size);
			patternDetectedLines.reserve(size);
			patternFrameResult.reserve(size);

		}

		void reserveLineGraphData(const int& size)
		{
			timeStampMs.reserve(size);
			luminanceTransitions.reserve(size);
			redTransitions.reserve(size);
			luminanceFrameResult.reserve(size);
			redFrameResult.reserve(size);
			patternFrameResult.reserve(size);
		}

		void push_back(const FrameData& data)
		{
			frame.push_back(data.Frame);
			timeStampMs.push_back(data.TimeStampMs);

			luminanceFlashArea.push_back(data.LuminanceFlashArea);
			luminanceAverage.push_back(data.LuminanceAverage);
			averageLuminanceDiff.push_back(data.AverageLuminanceDiff);
			averageLuminanceDiffAcc.push_back(data.AverageLuminanceDiffAcc);

			redFlashArea.push_back(data.RedFlashArea);
			redAverage.push_back(data.RedAverage);
			averageRedDiff.push_back(data.AverageRedDiff);
			averageRedDiffAcc.push_back(data.AverageRedDiffAcc);

			luminanceTransitions.push_back(data.LuminanceTransitions);
			redTransitions.push_back(data.RedTransitions);

			luminanceExtendedFailCount.push_back(data.LuminanceExtendedFailCount);
			redExtendedFailCount.push_back(data.RedExtendedFailCount);
			luminanceFrameResult.push_back((int)data.luminanceFrameResult);
			redFrameResult.push_back((int)data.redFrameResult);

			patternArea.push_back(data.patternArea);
			patternDetectedLines.push_back(data.patternDetectedLines);
			patternFrameResult.push_back((int)data.patternFrameResult);
		}

		void push_back_lineGraphData(const FrameData& data)
		{
			timeStampMs.push_back(data.TimeStampMs);
			luminanceTransitions.push_back(data.LuminanceTransitions);
			redTransitions.push_back(data.RedTransitions);
			luminanceFrameResult.push_back((int)data.luminanceFrameResult);
			redFrameResult.push_back((int)data.redFrameResult);
			patternFrameResult.push_back((int)data.patternFrameResult);
		}

		std::vector<int> frame;
		std::vector<std::string> timeStampMs;
		std::vector<std::string> luminanceFlashArea;
		std::vector<float> luminanceAverage;
		std::vector<float> averageLuminanceDiff;
		std::vector<float> averageLuminanceDiffAcc;
		std::vector <std::string> redFlashArea;
		std::vector<float> redAverage;
		std::vector<float> averageRedDiff;
		std::vector<float> averageRedDiffAcc;
		std::vector<int> luminanceTransitions;
		std::vector<int> redTransitions;
		std::vector<int> luminanceExtendedFailCount;
		std::vector<int> redExtendedFailCount;
		std::vector<short> luminanceFrameResult;
		std::vector<short> redFrameResult;
		std::vector<std::string> patternArea;
		std::vector<int> patternDetectedLines;
		std::vector<short> patternFrameResult;
	};

	//Serializes FrameData to Json object
	static void to_json(json& j, const FrameDataJson& data)
	{
		if (data.frame.capacity() == 0) //if frame vector is empty, line graph data serialization
		{
			j = json
			{ 
				{"TimeStampString", data.timeStampMs},
				{"LuminanceTransitions", data.luminanceTransitions},
				{"RedTransitions", data.redTransitions},
				{"LuminanceFrameResult", data.luminanceFrameResult},
				{"RedFrameResult", data.redFrameResult},
				{"PatternFrameResult", data.patternFrameResult}
			};
		}
		else
		{
			j = json
			{ {"Frame", data.frame},
				{"TimeStampString", data.timeStampMs},
				{"AverageLuminance", data.luminanceAverage},
				{"FlashAreaLuminance", data.luminanceFlashArea},
				{"AverageLuminanceDiff", data.averageLuminanceDiff},
				{"AverageLuminanceDiffAcc", data.averageLuminanceDiffAcc},
				{"AverageRed", data.redAverage},
				{"FlashAreaRed", data.redFlashArea},
				{"AverageRedDiff", data.averageRedDiff},
				{"AverageRedDiffAcc", data.averageRedDiffAcc},
				{"LuminanceTransitions", data.luminanceTransitions},
				{"RedTransitions", data.redTransitions},
				{"LuminanceExtendedFailCount", data.luminanceExtendedFailCount},
				{"RedExtendedFailCount", data.redExtendedFailCount},
				{"LuminanceFrameResult", data.luminanceFrameResult},
				{"RedFrameResult", data.redFrameResult},
				{"PatternArea", data.patternArea},
				{"PatternDetectedLines", data.patternDetectedLines},
				{"PatternFrameResult", data.patternFrameResult} };
		}
	};

}



