//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

/*
**** Flash ****
Abstract class for Flash detection
*/

#include "Flash.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "ConfigurationParams.h"
#include <limits>
#include <math.h>
#include "iris/Log.h"

namespace iris
{
    short Flash::fps = 0;

    Flash::Flash(short fps, const cv::Size& frameSize, FlashParams* flashParams)
    {
        Flash::fps = fps;
        m_params = flashParams;
        m_avgDiffInSecond.reserve(fps);
        m_avgDiffInSecond.emplace_back(0); //initial frame
        m_frameSize = frameSize.area();
        m_safeArea = frameSize.area() * m_params->areaProportion;

        LOG_CORE_INFO("Flash Area in pixels: {0}", m_safeArea);
        LOG_CORE_INFO("Number of pixels in frame: {0}", frameSize.area());
    }

    Flash::~Flash(){}

    void Flash::ReleaseLastFrame()
    {
        if (lastFrame != nullptr) {
            lastFrame->release();
            delete lastFrame;
            lastFrame = nullptr;
        }
    }

    void Flash::ReleaseCurrentFrame()
    {
        if (currentFrame != nullptr) {
            currentFrame->release();
            delete currentFrame;
            currentFrame = nullptr;
        }
    }

    cv::Mat* Flash::FrameDifference()
    {
        if (currentFrame == nullptr || lastFrame == nullptr) {
            return nullptr;
        }
        cv::Mat* ptrFrameDiff = new cv::Mat();
        cv::subtract(*currentFrame, *lastFrame, *ptrFrameDiff);
        return ptrFrameDiff;
    }

    double Flash::FrameMean()
    {
        return cv::mean(*currentFrame)[0];
    }

    void Flash::SetCurrentFrame(cv::Mat* flashValuesFrame)
    {
        lastFrame = currentFrame;
        currentFrame = flashValuesFrame;

        m_avgLastFrame = m_avgCurrentFrame;
        m_avgCurrentFrame = FrameMean();
    }

    double Flash::CheckSafeArea(cv::Mat* frameDifference)
    {
        int variation = cv::countNonZero(*frameDifference);
        m_flashArea = variation / (float)m_frameSize;

        if (variation >= m_safeArea)
        {
            return m_avgCurrentFrame - m_avgLastFrame;
        }

        return 0;
    }

    bool Flash::IsFlashTransition(const double& lastAvgDiffAcc, const double& avgDiffAcc, const double& threshold)
    {
        //if the luminance of the darker image is not below 0.8, no transition occurs (not applicable to red saturation)
        double darkerDiff = m_avgLastFrame < m_avgCurrentFrame ? m_avgLastFrame : m_avgCurrentFrame;
        
        //if tendency hasn't changed, check if last avg was a transition or part of 
        if (SameSign(lastAvgDiffAcc, avgDiffAcc) && std::abs(lastAvgDiffAcc) >= threshold)
        {
            return false;
        }
        else if (std::abs(avgDiffAcc) >= threshold && darkerDiff < m_params->darkThreshold) //check if current value is transition
        {
            return true;
        }
        return false; //no flash
    }

    Flash::CheckTransitionResult Flash::CheckTransition(double avgDiff, double lastAvgDiffAcc)
    {
        CheckTransitionResult result;
        result.lastAvgDiffAcc = lastAvgDiffAcc;

        if (SameSign(lastAvgDiffAcc, avgDiff))  //accumulate increase/decrease (positive/negative)
        {
            if (m_avgDiffInSecond.size() == m_avgDiffInSecond.capacity())
            {
                lastAvgDiffAcc -= m_avgDiffInSecond[0];
                m_avgDiffInSecond.erase(m_avgDiffInSecond.begin());
            }

            m_avgDiffInSecond.emplace_back(avgDiff);
            avgDiff += lastAvgDiffAcc; //accumulate value
        }
        else
        {
            m_avgDiffInSecond.clear();
            m_avgDiffInSecond.emplace_back(avgDiff);
        }

        result.checkResult = IsFlashTransition(result.lastAvgDiffAcc, avgDiff, m_params->flashThreshold);
        result.lastAvgDiffAcc = avgDiff; //new start acc value

        return result;
    }

    double Flash::roundoff(double value, unsigned char prec)
    {
        double pow_10 = std::pow(10.0, (double)prec);
        return std::round(value * pow_10) / pow_10;
    }

    void Flash::CalculateSrgbValues()
    {
        for (int i = 0; i < 256; i++)
        {
            double color = (double)i / 255.0; //normalised value
            //if RsRGB <= 0.04045 then R = RsRGB/12.92 else R = ((RsRGB+0.055)/1.055) ^ 2.4
            if (color <= 0.04045)
            {
                color = color / 12.92;
            }
            else
            {
                color = pow((color + 0.055) / 1.055, 2.4);
            }

            LOG_CORE_TRACE("{0},", color);
        }
    }

}
