//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include "utils/BaseLog.h"


namespace EA::EACC::Utils
{
	std::shared_ptr<spdlog::logger> BaseLog::m_CoreLogger;

	void BaseLog::InitCoreLogger(bool console, bool file, int level, RotatingFileSinkParams params, const char* pattern)
	{
		//Init Core Logger
		m_CoreLogger = std::make_shared<spdlog::logger>("CoreLogger");

		//add sinks to logger
		if (console)
		{
			m_CoreLogger->sinks().push_back(ConsoleSink());
		}
		if (file)
		{
			m_CoreLogger->sinks().push_back(RotatingFileSink(params));
		}
		if (pattern != nullptr) //otherwise spdlog default log messages
		{
			m_CoreLogger->set_pattern(pattern);
		}
		m_CoreLogger->set_level((spdlog::level::level_enum)level);
	}

	void BaseLog::SetLoggerFile(std::shared_ptr<spdlog::logger> logger, RotatingFileSinkParams params, const char* pattern)
	{
		if (logger != nullptr && !logger->sinks().empty())
		{
			logger->sinks().pop_back();
		}
		auto sink = RotatingFileSink(params);
		
		if (pattern != nullptr)
		{
			sink->set_pattern(pattern);
		}
		logger->sinks().push_back(sink);
	}

	std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> BaseLog::ConsoleSink()
	{
		return std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	}

	std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> 
		BaseLog::RotatingFileSink(RotatingFileSinkParams params)
	{
		return std::make_shared<spdlog::sinks::rotating_file_sink_mt>(params.fileName, params.max_size, params.max_files, params.rotate_on_open);
	}

	std::shared_ptr<spdlog::sinks::basic_file_sink_mt> BaseLog::BasicFileSink(const char* fileName)
	{
		return std::make_shared<spdlog::sinks::basic_file_sink_mt>(fileName);
	}
}