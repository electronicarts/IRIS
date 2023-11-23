//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <vector>
#include "../src/FrameData.h"
#include "iris/TotalFlashIncidents.h"

namespace iris
{
	enum class AnalysisResult
	{ 
        /// <summary>
        /// Failed test
        /// </summary>
        Fail,
        /// <summary>
        /// Passed test
        /// </summary>
        Pass,
        /// <summary>
        /// Passed test but there was flashing between 2-3Hz
        /// </summary>
        PassWithWarning,
        /// <summary>
        /// Luminance flash failure
        /// </summary>
        LuminanceFlashFailure,
        /// <summary>
        /// Luminance extended flash failure
        /// </summary>
        LuminanceExtendedFlashFailure,
        /// <summary>
        /// Red saturation flash failure
        /// </summary>
        RedFlashFailure,
        /// <summary>
        /// Red saturation extended flash failure
        /// </summary>
        RedExtendedFlashFailure,
        /// <summary>
        /// Pattern failure
        /// </summary>
        PatternFailure
	};

	struct Result
	{
		int VideoLen = 0;
        int AnalysisTime = 0;
        int TotalFrame = 0;
        int patternFailFrames = 0;
        AnalysisResult OverallResult = AnalysisResult::Pass;
        std::vector<AnalysisResult> Results;
        //total amount of frames that were counted that belonged to each incident type
        TotalFlashIncidents totalLuminanceIncidents;
        TotalFlashIncidents totalRedIncidents;
	};


    //Serializes FrameData to Json object
    static void to_json(json& j, const AnalysisResult& result)
    {
        std::string analysisRes;

        switch (result)
        {
        case AnalysisResult::Pass: analysisRes = "Pass"; break;
        case AnalysisResult::Fail: analysisRes = "Fail"; break;
        case AnalysisResult::PassWithWarning: analysisRes = "PassWithWarning"; break;
        case AnalysisResult::LuminanceFlashFailure: analysisRes = "LuminanceFlashFailure"; break;
        case AnalysisResult::LuminanceExtendedFlashFailure: analysisRes = "LuminanceExtendedFlashFailure"; break;
        case AnalysisResult::RedFlashFailure: analysisRes = "RedFlashFailure"; break;
        case AnalysisResult::RedExtendedFlashFailure: analysisRes = "RedExtendedFlashFailure"; break;
        case AnalysisResult::PatternFailure: analysisRes = "PatternFailure"; break;
        }

        j = analysisRes;
    };
}