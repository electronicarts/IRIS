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

    Flash::Flash(short videoFPS, const cv::Size& frameSize, FlashParams* flashParams)
    {
        Flash::fps = videoFPS;
        m_params = flashParams;
        m_avgDiffInSecond.reserve(fps);
        m_avgDiffInSecond.emplace_back(0); //initial frame
        m_frameSize = frameSize.area();
        m_safeArea = (int)(frameSize.area() * m_params->areaProportion);

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

    float Flash::FrameMean()
    {
        return (float)cv::mean(*currentFrame)[0];
    }

    void Flash::SetCurrentFrame(cv::Mat* flashValuesFrame)
    {
        lastFrame = currentFrame;
        currentFrame = flashValuesFrame;

        m_avgLastFrame = m_avgCurrentFrame;
        m_avgCurrentFrame = FrameMean();
    }

    float Flash::CheckSafeArea(cv::Mat* frameDifference)
    {
        int variation = cv::countNonZero(*frameDifference);
        m_flashArea = variation / (float)m_frameSize;

        if (variation >= m_safeArea)
        {
            return m_avgCurrentFrame - m_avgLastFrame;
        }

        return 0;
    }

    bool Flash::IsFlashTransition(const float& lastAvgDiffAcc, const float& avgDiffAcc, const float& threshold)
    {
        //if the luminance of the darker image is not below 0.8, no transition occurs (not applicable to red saturation)
        float darkerDiff = m_avgLastFrame < m_avgCurrentFrame ? m_avgLastFrame : m_avgCurrentFrame;
        
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

    Flash::CheckTransitionResult Flash::CheckTransition(float avgDiff, float lastAvgDiffAcc)
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

    float Flash::roundoff(float value, char prec)
    {
        float pow_10 = std::pow(10.0f, (float)prec);
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


    ///Old calculation
    //float Flash::CheckSafeArea(cv::Mat* frameDifference)
    //{
    //    cv::Mat positiveValues(frameDifference->size(), CV_32FC1);
    //    cv::Mat negativeValues(frameDifference->size(), CV_32FC1);

    //    cv::threshold(*frameDifference, positiveValues, 0, 320, cv::ThresholdTypes::THRESH_TOZERO);
    //    cv::threshold(*frameDifference, negativeValues, 0, 320, cv::ThresholdTypes::THRESH_TOZERO_INV);
    //    
    //    int posVariationArea = cv::countNonZero(positiveValues);
    //    int negVariationArea = cv::countNonZero(negativeValues);

    //    float posVariationAverage = 0;
    //    float negVariationAverage = 0;

    //    if (posVariationArea + negVariationArea >= m_safeArea) //minimum variation size 
    //    {
    //        if (posVariationArea > 0)
    //        {
    //            posVariationAverage = (float)cv::sum(positiveValues)[0] / posVariationArea;
    //        }
    //        if (negVariationArea > 0)
    //        {
    //            negVariationAverage = (float)cv::sum(negativeValues)[0] / negVariationArea;
    //        }
    //    }

    //    // Get highest abs average diff to mark tendency
    //    if (abs(posVariationAverage) > abs(negVariationAverage))
    //    {
    //        return posVariationAverage;
    //    }
    //    else
    //    {
    //        return negVariationAverage;
    //    }
    //}
}
