//Copyright (C) 2024 Electronic Arts, Inc.  All rights reserved.
#pragma once
#include "IFrameManager.h"
#include <vector>

namespace iris
{
class FrameData;

class FpsFrameManager : public IFrameManager
{
public:

	FpsFrameManager() = default;
	virtual ~FpsFrameManager() {};

	/// <summary>
	/// Adds a new manager, then returns the index to access the new elements.
	/// </summary>
	/// <param name="maxFrames:"> max capacity of frames in window </param>
	/// <param name="maxTime:"> max time capacity in window (seconds) not used for FPS</param> 
	virtual int RegisterManager(const int& maxFrames, const float& maxTime) override;

	/// <summary>
	/// Adds a new frame to the manager. If using real-time, the timeStampValue from data is used.
	/// Updates tracking time/fps and framesToRemove.
	/// </summary>
	virtual void AddFrame(const iris::FrameData& data) override;

	/// <summary>
	/// Returns the number of frames that need to be removed from the counter vectors.
	/// </summary>
	/// <param name="index:"> Integer to access the desired element in the vectors. </param>
	virtual int	GetFramesToRemove(const int& index) const override;

	/// <summary>
	/// Returns the number of frames in the window.
	/// </summary>
	/// <param name="index:"> Integer to access the desired element in the vectors. </param>
	virtual int GetCurrentFrameNum(const int& index) const override;

	/// <summary>
	/// Reset the manager.
	/// </summary>
	/// <param name="index:"> Integer to access the desired element in the vectors. </param>
	virtual void ResetManager(const int& index, bool removeLast = false) override;

private:

	struct FrameManager
	{
		int maxFrames; //max capacity
		int currentFrames;
		int framesToRemove;
	};

	std::vector<FrameManager> m_managers;

};

}