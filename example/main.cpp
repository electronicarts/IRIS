//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include "iris/Configuration.h"
#include "iris/Log.h"
#include "iris/VideoAnalyser.h"
#include <algorithm>
#include <filesystem>



char* getCmdOption(char** begin, char** end, const std::string& option)
{
	char** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end)
	{
		return *itr;
	}
	return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
	return std::find(begin, end, option) != end;
}

void CreateResultsDir(iris::Configuration& config)
{
	if (!std::filesystem::exists(config.GetResultsPath()))
	{
		try
		{
			std::filesystem::create_directories(config.GetResultsPath());
			LOG_CORE_TRACE("Created results dir at {}", config.GetResultsPath());
		}
		catch (std::filesystem::filesystem_error const& ex)
		{
			LOG_CORE_ERROR(ex.what());
		}
	}
}

bool GetVideoFiles(std::vector<std::string>& videoFiles)
{
	if (std::filesystem::exists("TestVideos"))
	{
		if (std::filesystem::is_empty("TestVideos"))
		{
			LOG_CORE_TRACE("Dir TestVideos is empty");
			LOG_CORE_ERROR("Directory TestVideos is empty. Specify a video or move them to this directory to be analyzed.");
			return false;
		}
		else
		{
			std::filesystem::directory_iterator dirIterator("TestVideos");
			videoFiles.reserve(std::distance(dirIterator, std::filesystem::directory_iterator{}));

			for (const auto& entry : std::filesystem::directory_iterator{ "TestVideos" })
			{
				videoFiles.emplace_back(entry.path().string());
			}
			return true;
		}
	}
	else
	{
		std::filesystem::create_directory("TestVideos");
		LOG_CORE_TRACE("TestVideos directory created");
		return false;
	}
}

int main(int argc, char* argv[])
{
	iris::Log::Init(true, true);

	iris::Configuration configuration;
	
	bool flagJson = false;
	const char* sourceVideo = nullptr;
	
	if (cmdOptionExists(argv, argv + argc, "-j"))
	{
		std::string fJson = getCmdOption(argv, argv + argc, "-j");
		flagJson = (fJson == "true" || fJson == "1");
	}

	if (cmdOptionExists(argv, argv + argc, "-v"))
	{
		sourceVideo = getCmdOption(argv, argv + argc, "-v");
	}

	if (cmdOptionExists(argv, argv + argc, "-l"))
	{
		std::string lumType = getCmdOption(argv, argv + argc, "-l");
		configuration.SetLuminanceType(lumType);
	}

	//load configuration
	configuration.Init();

	//Overwrite configuration
	if (cmdOptionExists(argv, argv + argc, "-p"))
	{
		std::string patternDetection = getCmdOption(argv, argv + argc, "-p");
		if (patternDetection == "true" || patternDetection == "1")
		{
			configuration.SetPatternDetectionStatus(true);
		}
		else if(patternDetection == "false" || patternDetection == "0")
		{
			configuration.SetPatternDetectionStatus(false);
		}
	}

	if (cmdOptionExists(argv, argv + argc, "-a"))
	{
		float areaProportion = atof(getCmdOption(argv, argv + argc, "-a"));
		configuration.SetSafeArea(areaProportion);
	}

	//Run video analysis
	CreateResultsDir(configuration);


	if (sourceVideo != nullptr) //Run specific video
	{
		iris::VideoAnalyser vA(&configuration);
		vA.AnalyseVideo(flagJson, sourceVideo);
	}
	else
	{
		std::vector<std::string> videoFiles;
		bool filesExist = GetVideoFiles(videoFiles);
		if (filesExist)
		{
			iris::VideoAnalyser vA(&configuration);

			for (int i = 0; i < videoFiles.size(); ++i)
			{
				vA.AnalyseVideo(flagJson, videoFiles[i].c_str());
			}
		}
	}
	
	iris::Log::ShutDown();

	return 0;
}
