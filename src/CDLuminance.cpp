//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

/*
**** RelativeLuminance ****
Abstract class for Flash detection
*/

#include "CDLuminance.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "ConfigurationParams.h"
#include "IrisFrame.h"
#include  "utils/FrameConverter.h"

namespace iris
{
    CDLuminance::CDLuminance(EA::EACC::Utils::FrameConverter *converter, const short& fps,const cv::Size& frameSize, FlashParams* params)
        : Flash(fps, frameSize, params)
    {
        m_converter = converter;
    }

    CDLuminance::~CDLuminance()
    {
        ReleaseLastFrame();
        ReleaseCurrentFrame();
    }

    /// <summary>
    /// Set the new current frame and move the previous one as the last frame.
    /// in RelativeLuminance this method and class are responsible to release memory for the frame created
    /// </summary>
    /// <param name="sRgbFrame"></param>
    void CDLuminance::SetCurrentFrame(const IrisFrame& irisFrame)
    {
        cv::Mat YCrCb;
        cv::cvtColor(*irisFrame.originalFrame, YCrCb, cv::COLOR_BGR2YCrCb); //luma-chroma colour space
        cv::Mat channelY;
        cv::extractChannel(YCrCb, channelY, 0);
        cv::Mat *frame = m_converter->Convert(channelY);

        ReleaseLastFrame();
        Flash::SetCurrentFrame(frame);
    }

    void CDLuminance::SetCurrentFrame(cv::Mat* bgrFrame)
    {
        cv::Mat YCrCb;
        cv::cvtColor(*bgrFrame, YCrCb, cv::COLOR_BGR2YCrCb); //luma-chroma colour space
        cv::Mat channelY;
        cv::extractChannel(YCrCb, channelY, 0);
        cv::Mat* frame = m_converter->Convert(channelY);

        ReleaseLastFrame();
        Flash::SetCurrentFrame(frame);
    }
}
