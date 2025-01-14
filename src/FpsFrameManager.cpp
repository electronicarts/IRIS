//Copyright (C) 2024 Electronic Arts, Inc.  All rights reserved.
#include "FpsFrameManager.h"
#include "iris/FrameData.h"

namespace iris
{

int FpsFrameManager::RegisterManager(const int& maxFrames, const float& maxTime)
{
	m_managers.emplace_back(FrameManager{ maxFrames });
	return m_managers.size() - 1;
}

void FpsFrameManager::AddFrame(const iris::FrameData& data)
{
	for (auto& manager: m_managers)
	{
		int condition = manager.currentFrames >= manager.maxFrames;
		manager.framesToRemove = condition;
		manager.currentFrames += 1 - condition;
	}
}

int FpsFrameManager::GetFramesToRemove(const int& index) const
{
	return m_managers[index].framesToRemove;
}

int FpsFrameManager::GetCurrentFrameNum(const int& index) const
{
	return m_managers[index].maxFrames; //max for fps as it's always set
}

void FpsFrameManager::ResetManager(const int& index, bool removeLast)
{
	m_managers[index].framesToRemove = 0;
	m_managers[index].currentFrames = 1; //assume we are always adding a frame when resetting
}

}