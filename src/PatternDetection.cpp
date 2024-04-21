//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include "PatternDetection.h"
#include "FrameData.h"
#include "IrisFrame.h"
#include "iris/Result.h"
#include "iris/Log.h"
#include "iris/Configuration.h"
#include "ConfigurationParams.h"
#include "iris/TotalFlashIncidents.h"


#include <map>
#include <unordered_map>
#include <math.h>
#include <opencv2/features2d.hpp>
#include <opencv2/imgproc.hpp>

#ifdef DEBUG_PATTERN_DETECTION
#include <opencv2/highgui.hpp>
#endif

namespace iris
{

PatternDetection::PatternDetection(Configuration* configuration, const short& fps, const cv::Size& frameSize) 
    : m_params(configuration->GetPatternDetectionParams()), m_fps(fps), m_isFail(false), m_patternFailFrames(0)

{
    if (frameSize.width > 480)
    {
        int scale_percent = 50; // percent of original size
        int width = int(frameSize.width * scale_percent / 100);
        int height = int(frameSize.height * scale_percent / 100);
        scaleSize = { width, height };
    }
    else
    {
        scaleSize = frameSize;
    }

    m_safeArea = scaleSize.area() * m_params->areaProportion;
    m_thresholdArea = scaleSize.area() * 0.20; //20% of the screen display
    m_diffThreshold = scaleSize.area() * 0.1;

    m_frameTimeThresh = fps * m_params->timeThreshold;
    m_contourThreshArea = scaleSize.area() * 0.00155;
    m_frameSize = scaleSize.area();

    m_patternFrameCount = {};
    m_patternFrameCount.count.reserve(m_frameTimeThresh);
    m_patternFrameCount.count.push_back(0);

    //TODO:: CACULATE CROPPED IMAGE
    centerPoint = cv::Point(scaleSize.width / 2, scaleSize.height / 2);

    int dilation_size = 1;
    m_dilationElement = cv::getStructuringElement(cv::MORPH_RECT,
        cv::Size(2 * dilation_size + 1, 2 * dilation_size + 1),
        cv::Point(dilation_size, dilation_size));

    int erosion_size = 1;
    m_erosionElement = cv::getStructuringElement(cv::MORPH_RECT,
        cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
        cv::Point(erosion_size, erosion_size));
}

void PatternDetection::checkFrame(const IrisFrame& irisFrame, const int& framePos, FrameData& data)
{
    auto pattern = detectPattern(irisFrame, framePos);

#ifdef DEBUG_PATTERN_DETECTION
    cv::destroyAllWindows();
#endif // DEBUG_PATTERN_DETECTION

    bool harmful = false;
    if (pattern.area >= m_safeArea && pattern.nComponents>= m_params->minStripes && pattern.avgLightLuminance >= 0.25)
    {
        harmful = true;
        data.patternArea = data.proportionToPercentage(pattern.area / (float)m_frameSize);
        data.patternDetectedLines = pattern.nComponents;
    }

    m_patternFrameCount.updateCurrent(harmful);
    checkFrameCount(data);
}

PatternDetection::Pattern PatternDetection::detectPattern(const IrisFrame& irisFrame, const int& framePos)
{
    cv::Mat luminance, luminance_8UC, iftThresh;
    cv::resize(*irisFrame.luminanceFrame, luminance, scaleSize);
    
    //normalize luminance values (ensures proper contrast if existing pattern)
    cv::normalize(luminance, luminance_8UC, 0, 255, cv::NORM_MINMAX);
    luminance_8UC.convertTo(luminance_8UC, CV_8UC1);

    SHOW_IMG(luminance_8UC, "8bit Luminance Frame"); 

    if (hasPattern(luminance_8UC, iftThresh))
    {
        Pattern pattern;
        auto [patternRegionMask, nComponents] = getPatternRegion(iftThresh, luminance_8UC);

        if (nComponents != -1)
        {
            pattern.nComponents = nComponents;
            pattern.area = cv::countNonZero(patternRegionMask);
            setPatternLuminance(pattern, patternRegionMask, luminance_8UC, luminance);
            return pattern;
        }
    }
    
    return { };   
}

void PatternDetection::checkFrameCount(FrameData& data)
{
    if (m_patternFrameCount.current >= m_frameTimeThresh)
    {
        data.patternFrameResult = PatternResult::Fail;
        m_isFail = true;
        m_patternFailFrames += 1;
    }
    else
    {
        data.patternFrameResult = PatternResult::Pass;
    }

    if (m_patternFrameCount.count.size() == m_frameTimeThresh)
    {
        m_patternFrameCount.updatePassed();
    }
}

bool PatternDetection::hasPattern(const cv::Mat& luminanceFrame, cv::Mat& iftThresh)
{
    //obtain the power spectrum then use it to filter the magnitude
    FourierTransform ft(centerPoint);
    FourierTransform::DftComponents dftComps = ft.getPSD(luminanceFrame);
    cv::Mat peaks = ft.getPeaks(dftComps.powerSpectrum);
    ft.filterMagnitude(peaks, dftComps.magnitude);

    //reconstruct image
    cv::Mat ift = ft.getIFT(dftComps);

#ifdef DEBUG_FFT
    SHOW_IMG(ift, "IFT");
#endif // DEBUG_FFT

    iftThresh = highlightPatternArea(ift, luminanceFrame);

    //if the area threshold has not been reached, no harmful pattern exists
    if (cv::countNonZero(iftThresh) < m_diffThreshold)
    {
        return false;
    }
    return true;
}

cv::Mat PatternDetection::highlightPatternArea(cv::Mat& ift, const cv::Mat& luminanceFrame)
{
    if (ift.size() != luminanceFrame.size())
    {
        //resize IFT to match size of original frame
        cv::resize(ift, ift, luminanceFrame.size());

#ifdef DEBUG_IFT
        SHOW_IMG(ift, "IFT Resize");
#endif // DEBUG_IFT
    }

    cv::Mat absDiff;
    cv::absdiff(ift, luminanceFrame, absDiff);
#ifdef DEBUG_IFT
    SHOW_IMG(absDiff, "Absolute Difference");
#endif // DEBUG_IFT
    cv::Mat thresh;
    cv::threshold(absDiff, thresh, 50, 255, cv::ThresholdTypes::THRESH_BINARY);
#ifdef DEBUG_IFT
    SHOW_IMG(thresh, "IFT Abs Diff Binary Threshold");
#endif // DEBUG_IFT

    return thresh;
}

std::tuple<cv::Mat, int> PatternDetection::getPatternRegion(cv::Mat& threshIFT, cv::Mat& luminanceFrame)
{
    //cv::erode(threshIFT, threshIFT, m_erosionElement);

#ifdef DEBUG_PATTERN_REGION
    SHOW_IMG(threshIFT, "Erosion");
#endif // DEBUG_PATTERN_REGION

    //remove elements that are not part of the pattern
    auto threshContours = getContours(threshIFT);
#ifdef DEBUG_PATTERN_REGION
    SHOW_CONTOURS(threshContours, threshIFT, "Erosion Contours");
#endif // DEBUG_PATTERN_REGION
    auto contoursMat = moveBiggerContours(threshContours, threshIFT);

#ifdef DEBUG_PATTERN_REGION
    SHOW_IMG(contoursMat, "Remaining Contours");
#endif  // DEBUG_PATTERN_REGION

//    cv::dilate(contoursMat, contoursMat, m_dilationElement);
//#ifdef DEBUG_PATTERN_REGION
//    SHOW_IMG(contoursMat, "Dilation");
//#endif  // DEBUG_PATTERN_REGION

    auto dilationContours = getContours(contoursMat);
#ifdef DEBUG_PATTERN_REGION
    SHOW_CONTOURS(dilationContours, contoursMat, "Dilation Contours");
#endif // DEBUG_PATTERN_REGION

    if (dilationContours.empty())
    {
        return { cv::Mat(), -1}; //return empty mat and error code
    }

#ifdef DEBUG_PATTERN_REGION
    auto [patternContour, patternComponents] = getPatternContour(dilationContours, contoursMat);
#else
    auto [patternContour, patternComponents] = getPatternContour(dilationContours);
#endif // DEBUG_PATTERN_REGION

    //get region rect
    cv::RotatedRect boundingRect = cv::minAreaRect(patternContour);
    std::vector<cv::Point2f> rect_points2f; 
    rect_points2f.resize(4);
    boundingRect.points(&rect_points2f[0]);
    std::vector<cv::Point> rect_points(rect_points2f.begin(), rect_points2f.end());
    std::vector<std::vector<cv::Point>> contourPoints = {rect_points};
#ifdef DEBUG_PATTERN_REGION
    SHOW_CONTOURS(contourPoints, contoursMat, "Pattern Region");
#endif // DEBUG_PATTERN_REGION

    //separate pattern region from the rest of the frame image using a mask
    cv::Mat patternRegionMask = cv::Mat(threshIFT.size(), CV_8UC1, cv::Scalar(0));
    cv::drawContours(patternRegionMask, contourPoints, -1, cv::Scalar(255), cv::FILLED);

    //isolate the pattern in the frame with the pattern mask
    cv::bitwise_and(luminanceFrame, patternRegionMask, luminanceFrame);

#ifdef DEBUG_PATTERN_REGION
    SHOW_IMG(patternRegionMask, "Pattern Region Mask");
    SHOW_IMG(luminanceFrame, "Approximated Pattern Region");
#endif // DEBUG_PATTERN_REGION

    return { patternRegionMask, patternComponents };
}

void PatternDetection::setPatternLuminance(Pattern& pattern, cv::Mat& patternRegion, const cv::Mat& luminance8UC, const cv::Mat& luminanceFrame)
{
    cv::Mat lightComponents, darkComponents;
    cv::threshold(luminance8UC, lightComponents, 0, 255, cv::THRESH_OTSU);
    SHOW_IMG(lightComponents, "Light Components");

    cv::bitwise_not(lightComponents, darkComponents, patternRegion);
    SHOW_IMG(darkComponents, "Dark Components");

    pattern.avgLightLuminance = cv::mean(luminanceFrame, lightComponents)[0];
    pattern.avgDarkLuminance = cv::mean(luminanceFrame, darkComponents)[0];
}

std::vector<std::vector<cv::Point>> PatternDetection::getContours(const cv::Mat& src)
{
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(                                       // Detect contours of input image
        src,                                                // 8bit edges input frame 
        contours,                                           // Set of points defining a contour
        cv::RETR_EXTERNAL,                                  // Contour retrieval mode
        cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE  // Contour approximation mode
    );

    return contours;
}

cv::Mat PatternDetection::moveBiggerContours(std::vector<std::vector<cv::Point>>& contours, cv::Mat& src)
{
    cv::Mat contoursMat = cv::Mat::zeros(src.size(), src.type());
    //remove small contours
    for (int i = 0; i < contours.size(); i++)
    {
        if (cv::contourArea(contours[i]) > m_contourThreshArea)
        {
            cv::fillConvexPoly(contoursMat, contours[i], cv::Scalar(255), cv::LineTypes::LINE_8);
        }
    }

    return contoursMat;
}

std::vector<cv::Point> PatternDetection::getBiggestContour(const std::vector<std::vector<cv::Point>>& contours)
{
    std::vector<cv::Point> biggestContour = contours[0];
    double biggestArea = cv::contourArea(contours[0]);

    for (const auto& contour : contours) //biggest area should be the pattern
    {
        double area = cv::contourArea(contour);

        if (area > biggestArea)
        {
            biggestArea = area;
            biggestContour = contour;
        }
    }

    return biggestContour;
}

#ifdef DEBUG_PATTERN_REGION
std::tuple<std::vector<cv::Point>, int> PatternDetection::getPatternContour(const std::vector<std::vector<cv::Point>>& contours, const cv::Mat& contoursMat)
#else
std::tuple<std::vector<cv::Point>, int> PatternDetection::getPatternContour(const std::vector<std::vector<cv::Point>>& contours)
#endif // DEBUG_PATTERN_REGION
{
#ifdef DEBUG_PATTERN_REGION
    if (contours.size() < 5)
    {
        return { getBiggestContour(contours), 0 };
    }
    else
    {
        auto [contour, nContours] = getSimilarContours(contours, contoursMat);
        return { contour, nContours };
    }
#else
    if (contours.size() < 5)
    {
        return { getBiggestContour(contours), 0 };
    }
    else
    {
        auto [contour, nContours] = getSimilarContours(contours);
        return { contour, nContours };
    }
#endif // DEBUG_PATTERN_REGION
}

#ifdef DEBUG_PATTERN_REGION
std::tuple<std::vector<cv::Point>, int> PatternDetection::getSimilarContours(const std::vector<std::vector<cv::Point>>& contours, const cv::Mat& contoursMat)
#else
std::tuple<std::vector<cv::Point>, int> PatternDetection::getSimilarContours(const std::vector<std::vector<cv::Point>>& contours)
#endif
{
    using ContourGroup = std::vector<std::vector<cv::Point>>;
    std::vector<ContourGroup> similarContours;

    //group contours by shape similarity
    for (int i = 1; i < contours.size(); i++)
    {
        ContourGroup group{ contours[i] };

        for (int j = 0; j < contours.size(); j++)
        {
            if (j != i)
            {
                double similarity = cv::matchShapes(contours[i], contours[j], cv::ShapeMatchModes::CONTOURS_MATCH_I1, 0);
                if (similarity < 0.7)
                {
                    group.emplace_back(contours[j]);
                }
            }
        }

        similarContours.emplace_back(group);
    }

    //sort contour groups by group size
    std::sort(similarContours.begin(), similarContours.end(), [](const auto& a, const auto& b)
        {
            return a.size() > b.size();
        });

#ifdef DEBUG_PATTERN_REGION
    SHOW_CONTOURS(similarContours[0], contoursMat, "Grouped Contours");
#endif

    //merge contours in one (to properly obtain the min area rect)
    std::vector<cv::Point> contour;
    for (auto& group : similarContours[0])
    {
        contour.insert(contour.end(), group.begin(), group.end());
    }

    return { contour, similarContours[0].size()};
}

bool PatternDetection::isFail()
{
    LOG_CORE_INFO("Pattern FAIL: {}", (m_isFail ? "true" : "false"));
	return m_isFail;
}

void PatternDetection::setResult(Result& result)
{
    if (isFail())
    {
        result.OverallResult = AnalysisResult::Fail;
        result.Results.push_back(AnalysisResult::PatternFailure);
        result.patternFailFrames = m_patternFailFrames;
    }

}


#ifdef DEBUG_PATTERN_DETECTION
void showImg(const cv::Mat& src, const char* wName) 
{
	cv::imshow(wName, src);
	cv::waitKey();
}
void showContours(const std::vector<std::vector<cv::Point>>& contours, const cv::Mat src, const char* winName)
{
    cv::Mat contourImg;
    cv::cvtColor(src, contourImg, cv::COLOR_GRAY2BGR);
    cv::drawContours(contourImg, contours, -1, cv::Scalar(0, 0, 255), 2);
    SHOW_IMG(contourImg, winName);
}
#endif


FourierTransform::DftComponents FourierTransform::getPSD(const cv::Mat& src)
{
    cv::Mat dft = getDFT(src);
    DftComponents dftComps = getDftComponents(dft);
    
    //compute PSD
    dftComps.powerSpectrum = dftComps.magnitude.clone();
    normalize(dftComps.powerSpectrum, -1.0f, 1.0f);
    dftComps.powerSpectrum = cv::abs(dftComps.powerSpectrum);
    dftComps.powerSpectrum = 1 - dftComps.powerSpectrum;
    cv::pow(dftComps.powerSpectrum, 2, dftComps.powerSpectrum);
    log(dftComps.powerSpectrum);
    normalize(dftComps.powerSpectrum, 0, 255);

#ifdef DEBUG_FFT
    fftShift(dftComps.powerSpectrum);
    SHOW_IMG(dftComps.powerSpectrum , "PSD");
    fftShift(dftComps.powerSpectrum);
#endif // DEBUG_FFT

    return dftComps;
}

cv::Mat FourierTransform::getPeaks(const cv::Mat& psd)
{
    //threshold peaks
    cv::Mat threshPSD;
    psd.convertTo(threshPSD, CV_8UC1);
    double thresh = cv::threshold(threshPSD, threshPSD, 7, 255, cv::ThresholdTypes::THRESH_OTSU);

#ifdef DEBUG_FFT
    fftShift(threshPSD);
    SHOW_IMG(threshPSD, "PSD Binary Threshold");
    fftShift(threshPSD);
#endif // DEBUG_FFT

    return threshPSD;
}

void FourierTransform::filterMagnitude(cv::Mat& peaks, cv::Mat& magnitude)
{
    //prepare mask in peaks mat
    fftShift(peaks);
    peaks.convertTo(peaks, CV_8UC1);
    cv::circle(peaks, centerPoint, 5, 0, -1);

#ifdef DEBUG_FFT
    SHOW_IMG(peaks, "Magnitude mask");
#endif // DEBUG_FFT

    fftShift(peaks);

    //remove peaks in magnitude with mask
    magnitude = magnitude.setTo(0, peaks);
}


cv::Mat FourierTransform::getIFT(DftComponents dftComps)
{
    cv::Mat dft, ift;
    cv::Mat planes[] = { cv::Mat(), cv::Mat() }; //planes[0] = real, planes[1] = imag

    //recover the complex dft from the magnitude and phase
    cv::polarToCart(dftComps.magnitude, dftComps.phase, planes[0], planes[1]);
    cv::merge(planes, 2, dft);

    //inverse the dft to reconstruct the image 
    cv::dft(dft, ift, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);
    ift.convertTo(ift, CV_8U, 255.0); // Back to 8-bits
    return ift;
}

cv::Mat FourierTransform::getDFT(const cv::Mat& src)
{
    cv::Mat padded;
    int m = cv::getOptimalDFTSize(src.rows);
    int n = cv::getOptimalDFTSize(src.cols); // on the border add zero values
    cv::copyMakeBorder(src, padded, 0, m - src.rows, 0, n - src.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

    //TODO:: needed if using original frame but not if it's the luminance frame
    padded.convertTo(padded, CV_32F, 1.0 / 255.0); //this allows proper image reconstruction

    // Add to the expanded another plane with zeros this way the result may fit in the source matrix
    cv::Mat planes[] = { cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F) };
    cv::Mat dft;
    cv::merge(planes, 2, dft);
    cv::dft(dft, dft, cv::DFT_SCALE | cv::DFT_COMPLEX_OUTPUT);
    return dft;
}

FourierTransform::DftComponents FourierTransform::getDftComponents(cv::Mat& dft)
{
    DftComponents dftComps(dft);
    cv::Mat planes[2]; //plane[0] = real, plane[1] = imag
    cv::split(dft, planes);
    cv::cartToPolar(planes[0], planes[1], dftComps.magnitude, dftComps.phase);
    return dftComps;
}

void FourierTransform::fftShift(cv::Mat& src)
{
    // crop the spectrum, if it has an odd number of rows or columns
    cv::Mat aux = src(cv::Rect(0, 0, src.cols & -2, src.rows & -2));

    // rearrange the quadrants of Fourier image so that the origin is at the image center
    int cx = aux.cols / 2;
    int cy = aux.rows / 2;

    cv::Mat q0(aux, cv::Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    cv::Mat q1(aux, cv::Rect(cx, 0, cx, cy));  // Top-Right
    cv::Mat q2(aux, cv::Rect(0, cy, cx, cy));  // Bottom-Left
    cv::Mat q3(aux, cv::Rect(cx, cy, cx, cy)); // Bottom-Right

    // swap quadrants (Top-Left with Bottom-Right)
    cv::Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    // swap quadrant (Top-Right with Bottom-Left)
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);

    //Assing the aut to the src, keeping the odd column/row as it was
    src(cv::Rect(0, 0, src.cols & -2, src.rows & -2)) = aux;
}

void FourierTransform::log(cv::Mat& src)
{
    // switch to logarithmic scale
    src += cv::Scalar::all(1);
    cv::log(src, src);
}

void FourierTransform::normalize(cv::Mat& src, float min, float max)
{
    // viewable image form (float between values 0 and 1) transform the matrix with float values into a 
    cv::normalize(src, src, min, max, cv::NORM_MINMAX);
}



} //namespace iris

