//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

/*
**** RelativeLuminance ****
Abstract class for Flash detection
*/

#include "RelativeLuminance.h"
#include <vector>
#include <opencv2/core.hpp>
#include "ConfigurationParams.h"
#include "IrisFrame.h"

namespace iris
{
    cv::Scalar RelativeLuminance::rgbValues(0.0722f, 0.7152f, 0.2126f);

    RelativeLuminance::RelativeLuminance(short fps, const cv::Size& frameSize, FlashParams* params)
        : Flash(fps, frameSize, params)
    {
    }

    RelativeLuminance::~RelativeLuminance()
    {
        ReleaseLastFrame();
        ReleaseCurrentFrame();
    }

    /// <summary>
    /// Set the new current frame and move the previous one as the last frame.
    /// in RelativeLuminance this method and class are responsible to release memory for the frame created
    /// </summary>
    /// <param name="sRgbFrame"></param>
    void RelativeLuminance::SetCurrentFrame(const IrisFrame& irisFrame)
    {
        cv::Mat* frame = new cv::Mat(irisFrame.sRgbFrame->size(), CV_32FC1);
        irisFrame.sRgbFrame->forEach<cv::Vec3f>(ConvertToRelativeLuminance(frame));
        
        ReleaseLastFrame();
        Flash::SetCurrentFrame(frame);
    }

    void RelativeLuminance::SetCurrentFrame(cv::Mat* bgrFrame)
    {
        cv::Mat* frame = new cv::Mat(bgrFrame->size(), CV_32FC1);
        bgrFrame->forEach<cv::Vec3f>(ConvertToRelativeLuminance(frame));

        ReleaseLastFrame();
        Flash::SetCurrentFrame(frame);
    }
}
