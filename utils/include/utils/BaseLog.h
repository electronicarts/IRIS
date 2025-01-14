//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

// Implements a small wrapper of a logging library to use logging in C++ projects.
// To use ILog, implement a Log class that inherits from ILog and define the sinks
// needed for the loggers.

#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>

#ifdef UTILS_SHARED
	#ifdef UTILS_EXPORT
		#define UTILS_API __declspec(dllexport)
	#else
		#define UTILS_API __declspec(dllimport)
#endif
#else
	#define UTILS_API
#endif

namespace EA::EACC::Utils
{
	class BaseLog
	{
	public:

		/// <summary>
		/// Call at the end of the application to flush all pending messages
		/// </summary>
		static void ShutDown(){ spdlog::shutdown(); }

		inline static std::shared_ptr<spdlog::logger> GetCoreLogger() { return m_CoreLogger; }

		static void Init() { InitCoreLogger(true, false, 0, BaseLog::RotatingFileSinkParams(""), "%^[%T] [%t] [%l]: %v%$"); };

		struct RotatingFileSinkParams
		{
			RotatingFileSinkParams(const char* fileName) : fileName(fileName){}

			const char* fileName = nullptr;
			std::size_t max_size = -1; //set max_size to -1 to allow for unlimited file size
			std::size_t max_files = 5;
			bool rotate_on_open = true;
		};
	protected:

		/// <summary>
		/// Initializes Core Logger
		/// </summary>
		/// <param name="console">Enable/Disable logging to console</param>
		/// <param name="file">Enable/Disable logging to file</param>
		/// <param name="level">Define a level to filter logging messages by severity</param>
		/// <param name="params">If file logging enabled, provide params to configure file sink</param>
		/// <param name="pattern">Provide logging pattern, if pattern is nullptr, default spdlog logging pattern will be used</param>
		static void InitCoreLogger(bool console, bool file, int level, RotatingFileSinkParams params, const char* pattern);

		/// <summary>
		/// Removes previous file sink in logger and adds a new one
		/// </summary>
		/// <param name="logger">logger object to modift</param>
		/// <param name="params">Params to create new file sink</param>
		/// <param name="pattern">Logging pattern, if nullptr, default spdlog logging pattern will be used</param>
		static void SetLoggerFile(std::shared_ptr<spdlog::logger> logger, RotatingFileSinkParams params, const char* pattern = nullptr);
		
		/// <summary>
		/// Create and return a thread safe console sink
		/// </summary>
		static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> ConsoleSink();
		
		/// <summary>
		/// Create and return a thread safe rotating file sink
		/// </summary>
		/// <param name="params">Params to create file sink</param>
		static std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> RotatingFileSink(RotatingFileSinkParams params);
		
		/// <summary>
		/// Create and return a thread safe basic file sink
		/// </summary>
		/// <param name="fileName">Name of log file</param>
		static std::shared_ptr<spdlog::sinks::basic_file_sink_mt> BasicFileSink(const char* fileName);

		UTILS_API static std::shared_ptr<spdlog::logger> m_CoreLogger;

	};
}

//Core Logger macros
#define LOG_CORE_TRACE(...)			EA::EACC::Utils::BaseLog::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_CORE_DEBUG(...)			EA::EACC::Utils::BaseLog::GetCoreLogger()->debug(__VA_ARGS__)
#define LOG_CORE_INFO(...)			EA::EACC::Utils::BaseLog::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_WARNING(...)		EA::EACC::Utils::BaseLog::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_CORE_ERROR(...)			EA::EACC::Utils::BaseLog::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CORE_CRITICAL(...)		EA::EACC::Utils::BaseLog::GetCoreLogger()->critical(__VA_ARGS__)