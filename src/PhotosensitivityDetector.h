//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#pragma once

namespace iris
{
	struct Result;
	class FrameData;
	struct IrisFrame;

class PhotosensitivityDetector
{
public:

	/// <summary>
	/// 
	/// </summary>
	virtual void checkFrame(const IrisFrame& irisFrame, const int& framePos, FrameData& data) = 0;

	/// <summary>
	/// Returns true if a photosensitivity issues have been found
	/// </summary>
	virtual bool isFail() = 0;

	/// <summary>
	/// Sets the flash/pattern detection results for a result object
	/// </summary>
	virtual void setResult(Result& result) = 0;

protected:

};

}