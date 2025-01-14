//Copyright (C) 2024 Electronic Arts, Inc.  All rights reserved.

#include "TimeFrameManager.h"
#include "iris/FrameData.h"

namespace iris
{

int TimeFrameManager::RegisterManager(const int& maxFrames, const float& maxTime)
{
	m_managers.emplace_back(
		TimeStampManager
		{ 
			maxFrames, 
			(int)(maxTime * 1000) //convert seconds to milliseconds
		});
	return m_managers.size() - 1;
}

void TimeFrameManager::AddFrame(const iris::FrameData& data)
{
	for (auto& manager : m_managers)
	{
		manager.AddFrame(data.TimeStampVal);
	}
}

int TimeFrameManager::GetFramesToRemove(const int& index) const
{
	return m_managers[index].framesToRemove;
}

int TimeFrameManager::GetCurrentFrameNum(const int& index) const
{
	return m_managers[index].frameTimeStamps.size();
}

void TimeFrameManager::ResetManager(const int& index, bool removeLast)
{
	m_managers[index].Reset(removeLast);
}

}