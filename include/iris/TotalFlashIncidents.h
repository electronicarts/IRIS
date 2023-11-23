//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "utils/JsonWrapper.h"

namespace iris
{

enum class FlashResult
{
	Pass = 0, PassWithWarning, ExtendedFail, FlashFail
};

enum class PatternResult
{
	Pass = 0, Fail
};


struct TotalFlashIncidents //amount of frames of each incident type
{
	int extendedFailFrames = 0;
	int flashFailFrames = 0;
	int passWithWarningFrames = 0;

	int getTotalFailedFrames() const { return extendedFailFrames + flashFailFrames; }
};

static void to_json(json& j, const TotalFlashIncidents& totalFlashIncidents)
{
	j = json{ {"ExtendedFailFrames", totalFlashIncidents.extendedFailFrames}, 
		{"FlashFailFrames", totalFlashIncidents.flashFailFrames}, 
		{"PassWithWarningFrames", totalFlashIncidents.passWithWarningFrames}, 
		{"TotalFailedFrames", totalFlashIncidents.getTotalFailedFrames()}
	};
};

}