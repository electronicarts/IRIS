//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include "iris/Log.h"
#include <iostream>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace iris
{
	std::shared_ptr<spdlog::logger> Log::m_DataLogger;

	void Log::Init(bool console, bool file, int coreLevel, const char* dataFile)
	{
		try
		{
			//Init Core Logger
			InitCoreLogger(console, file, coreLevel, RotatingFileSinkParams("logs/core.log"), "%^[%T] [%t] [%l]: %v%$");

			//Init Data Logger
			if (dataFile != nullptr) //can be set by video
			{
				auto dataFileSink = RotatingFileSink(RotatingFileSinkParams(dataFile));
				m_DataLogger = std::make_shared<spdlog::logger>("DataLogger", dataFileSink);
			}
			else
			{
				m_DataLogger = std::make_shared<spdlog::logger>("DataLogger");
			}

			m_DataLogger->set_pattern("%v");

		}
		catch (const spdlog::spdlog_ex& ex)
		{
			std::cout << "Core Log initialization failed: " << ex.what() << std::endl;
		}
	}

	void Log::SetDataLoggerFile(const char* fileName)
	{
		SetLoggerFile(m_DataLogger, RotatingFileSinkParams(fileName), "%v");
	}
}