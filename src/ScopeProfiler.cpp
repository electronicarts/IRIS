#include "iris/ScopeProfiler.h"
#include "iris/Log.h"

#include <thread>
#include <iostream>

ScopeProfiler* ScopeProfiler::instance = nullptr;


ScopeProfiler::ScopeProfiler() {
}

void ScopeProfiler::WriteHeader(const std::string& videoName,const std::string& end)
{
	m_OutputStream.close();

	std::string newFilepath = filepath + videoName + "/"+ filepathMid + end + filepathEnd;
	if (videoName == "" && end == "") 
	{
		newFilepath = filepath + filepathMid + filepathEnd;
	}
	LOG_CORE_INFO("JSON Profiler path: " + newFilepath);
	m_OutputStream.open(newFilepath);
	m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
	m_OutputStream.flush();
}

void ScopeProfiler::RegisterEvent(const std::string& eventName) {
	ProfileEvent newProfile;
	newProfile.name = eventName;
	newProfile.processId = 0;
	newProfile.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
	newProfile.chrono_StartTime = std::chrono::high_resolution_clock::now();
	m_Events[eventName] = newProfile;
}

void ScopeProfiler::FinishEvent(const std::string& eventName) {
	auto endTime = std::chrono::high_resolution_clock::now();
	m_Events[eventName].startTime = std::chrono::time_point_cast<std::chrono::microseconds>(m_Events[eventName].chrono_StartTime).time_since_epoch().count();
	m_Events[eventName].endTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch().count();

	WriteProfile(m_Events[eventName]);
}

void ScopeProfiler::WriteFooter()
{
	m_OutputStream.seekp(-1, std::ios::end);
	m_OutputStream << "]}";
	m_OutputStream.flush();
	m_OutputStream.close();
}

void ScopeProfiler::WriteProfile(const ProfileEvent& eventName)
{
	m_OutputStream << "{";
	m_OutputStream << "\"cat\":\"function\",";
	m_OutputStream << "\"dur\":" << (eventName.endTime - eventName.startTime) << ',';
	m_OutputStream << "\"name\":\"" << eventName.name << "\",";
	m_OutputStream << "\"ph\":\"X\",";
	m_OutputStream << "\"pid\":0,";
	m_OutputStream << "\"tid\":" << eventName.threadId << ",";
	m_OutputStream << "\"ts\":" << eventName.startTime;
	m_OutputStream << "}";
	m_OutputStream << ",";

	m_OutputStream.flush();
}
