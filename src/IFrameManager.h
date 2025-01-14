//Copyright (C) 2024 Electronic Arts, Inc.  All rights reserved.
#pragma once


namespace iris
{
class FrameData;

class IFrameManager
{
public:
	virtual ~IFrameManager() {};

	/// <summary>
	/// Adds a new manager, then returns the index to access the new elements.
	/// </summary>
	/// <param name="maxFrames:"> max capacity of frames in window </param>
	/// <param name="maxTime:"> max time capacity in window (seconds) </param> 
	virtual int RegisterManager(const int& maxFrames, const float& maxTime = 0) = 0;

	/// <summary>
	/// Adds a new frame to the manager. If using real-time, the timeStampValue from data is used.
	/// Updates tracking time/fps and framesToRemove.
	/// </summary>
	virtual void AddFrame(const iris::FrameData& data) = 0;

	/// <summary>
	/// Returns the number of frames that need to be removed from the counter vectors.
	/// </summary>
	/// <param name="index:"> Integer to access the desired element in the vectors. </param>
	virtual int	GetFramesToRemove(const int& index) const = 0;

	/// <summary>
	/// Returns the current number of frames.
	/// </summary>
	/// <param name="index:"> Integer to access the desired element in the vectors. </param>
	virtual int GetCurrentFrameNum(const int& index) const = 0;

	/// <summary>
	/// Reset the manager.
	/// </summary>
	/// <param name="index:"> Integer to access the desired element in the vectors. </param>
	virtual void ResetManager(const int& index, bool removeLast = false) = 0;

};

}