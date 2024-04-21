#pragma once

#include<chrono>
#include<string>
#include <fstream>
#include <map>

#ifdef IRIS_SHARED
#ifdef IRIS_EXPORT
#define IRIS_API __declspec(dllexport)
#else
#define IRIS_API __declspec(dllimport)
#endif
#else
#define IRIS_API
#endif


#ifdef PROFILING

#define BEGIN_PROFILING() ScopeProfiler::GetInstance()->WriteHeader("","")
#define BEGIN_PROFILING_PATH(videoName) ScopeProfiler::GetInstance()->WriteHeader(videoName, "")
#define BEGIN_PROFILING_PATH_END(videoName, strEnd) ScopeProfiler::GetInstance()->WriteHeader(videoName, strEnd)

#define PROFILING_IN(eventName) ScopeProfiler::GetInstance()->RegisterEvent(eventName)
#define PROFILING_OUT(eventName) ScopeProfiler::GetInstance()->FinishEvent(eventName)

#define END_PROFILING() ScopeProfiler::GetInstance()->WriteFooter()

#else
#define BEGIN_PROFILING() 
#define BEGIN_PROFILING_PATH(videoName) 
#define BEGIN_PROFILING_PATH_END(videoName, strEnd) 

#define PROFILING_IN(eventName) 
#define PROFILING_OUT(eventName)

#define END_PROFILING() 
#endif


class ScopeProfiler {
public:

	//Singleton
	inline static ScopeProfiler* GetInstance() {
		if (!instance) {
			instance = new ScopeProfiler();
		}
		return instance;
	}

	/// <summary>
	/// Open file and Write the header in the .json file
	/// </summary>
	void WriteHeader(const std::string&, const std::string&);
	
	/// <summary>
	/// Creates the event and stores it in m_Events
	/// </summary>
	void RegisterEvent(const std::string&);
	
	/// <summary>
	/// Write event info on .json file
	/// </summary>
	void FinishEvent(const std::string&);

	/// <summary>
	/// Write the Footer in the .json file
	/// </summary>
	void WriteFooter();


private:
	struct ProfileEvent {
		std::string name;
		std::string category = "function";
		long long startTime, endTime;
		unsigned int processId;
		unsigned int threadId;
		std::chrono::time_point<std::chrono::high_resolution_clock> chrono_StartTime;
	};

	std::ofstream m_OutputStream;

	IRIS_API static ScopeProfiler* instance;

	std::map<std::string, ProfileEvent> m_Events;

	const std::string filepath = "Results/";
	const std::string filepathMid = "ProfilingResults";
	const std::string filepathEnd = ".json";

	ScopeProfiler();

	/// <summary>
	/// Write event info in the .json file
	/// </summary>
	void WriteProfile(const ProfileEvent&);

};

