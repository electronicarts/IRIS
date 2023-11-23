//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include "PhotosensitivityDetector.h"
#include <opencv2/core.hpp>

namespace iris
{
	class FrameData;
	class Configuration;
	struct IrisFrame;
	struct Result;
	struct PatternDetectionParams;

#ifdef _DEBUG
//#define DEBUG_PATTERN_DETECTION
#endif // _DEBUG

#ifdef DEBUG_PATTERN_DETECTION
	static void showImg(const cv::Mat& src, const char* wName = "");
	static void showContours(const std::vector<std::vector<cv::Point>>& contours, const cv::Mat src, const char* winName = "Contours");
#define SHOW_IMG(mat, wName)	showImg(mat, wName)	
#define SHOW_CONTOURS(contours, mat, wName)	showContours(contours, mat, wName)	
#define DEBUG_FFT
#define DEBUG_IFT
#define DEBUG_PATTERN_REGION
#else
#define	SHOW_IMG(mat, wName)
#define SHOW_CONTOURS(contours, mat, wName)
#endif // !DEBUG_PATTERN_DETECTION


class PatternDetection : public  PhotosensitivityDetector
{
public:
	PatternDetection(Configuration* configuration, const short& fps, const cv::Size& frameSize);

	//Checks a video frame for harmful patterns
	void checkFrame(const IrisFrame& irisFrame, const int& framePos, FrameData& data) override;

	//returns true if the video is not compliant to the pattern guidelines
	bool isFail() override;

	//sets the results of the pattern detection
	void setResult(Result& result) override;

private:

	struct Pattern
	{
		int area;
		int nComponents; //stripes, circles, rectangles, etc
		float avgDarkLuminance; //average luminance of pattern dark components
		float avgLightLuminance; //average luminance of pattern light components
	};

	void checkFrameCount(FrameData& data);
	//detects a pattern in a video frame and returns the pattern info
	Pattern detectPattern(const IrisFrame& irisFrame);

	//determines whether a video frame has a pattern or not
	bool hasPattern(const cv::Mat& luminanceFrame, cv::Mat& ift);
	
	//applies operations on the inverse Fourier transform to highlight the pattern area
	cv::Mat highlightPatternArea(cv::Mat& ift, const cv::Mat& luminanceFrame);
	
	//obtains the pattern region mask and the number of pattern components
	std::tuple<cv::Mat, int> getPatternRegion(cv::Mat& otsu, cv::Mat& luminanceFrame);
	
	//calculates the luminance of the pattern components
	void setPatternLuminance(Pattern& pattern, cv::Mat& patternRegion, const cv::Mat& luminance8UC, const cv::Mat& luminanceFrame);

	//obtains the contours of matrix
	std::vector<std::vector<cv::Point>> getContours(const cv::Mat& src);

	//contours that are bigger than the area threshold are moved to another Mat
	//as to avoid having pixels of the smaller contours
	cv::Mat moveBiggerContours(std::vector<std::vector<cv::Point>>& contours, cv::Mat& src);

	//returns the contour with the biggest area
	std::vector<cv::Point> getBiggestContour(const std::vector<std::vector<cv::Point>>& contours);

#ifndef DEBUG_PATTERN_REGION
	//obtains the contour of the pattern region and the number pattern components
	std::tuple<std::vector<cv::Point>, int> getPatternContour(const std::vector<std::vector<cv::Point>>& contours);

	//groups contours by shape
	std::tuple<std::vector<cv::Point>, int> getSimilarContours(const std::vector<std::vector<cv::Point>>& contours);
#else
	//same methods but the contourMat img is passed to show debug images
	std::tuple<std::vector<cv::Point>, int> getPatternContour(const std::vector<std::vector<cv::Point>>& contours,
		const cv::Mat& contourMat);
	std::tuple<std::vector<cv::Point>, int> getSimilarContours(const std::vector<std::vector<cv::Point>>& contours,
		const cv::Mat& contourMat);
#endif // DEBUG_PATTERN_REGION true



	struct Counter
	{
		void updateCurrent(bool add)
		{
			if (add)
			{
				count.emplace_back(count.back() + 1);
			}
			else
			{
				count.emplace_back(count.back());
			}

			current = count.back() - passed;
		}

		void updatePassed()
		{
			passed = count.front();
			count.erase(count.begin());
		}

		std::vector<int> count;
		int passed = 0;
		int current = 0;
	};

	PatternDetectionParams* m_params;


	Counter m_patternFrameCount;

	int m_frameTimeThresh;
	int m_patternFailFrames;

	short m_fps;
	int m_safeArea;
	int m_frameSize;
	int m_thresholdArea; //20% of the frame size to check if there is a possible pattern
	bool m_isFail;
	int areaThresh;
	int m_contourThreshArea;

	cv::Mat m_dilationElement;
	cv::Mat m_erosionElement;
	cv::Point centerPoint;
	cv::Size scaleSize; //downscale the video frame to this size if the resolutions is high enough
};

class FourierTransform
{
public:
	struct DftComponents;
	struct Peak;

	FourierTransform(cv::Point& center) : centerPoint(center) {};
		
	/// <summary>
	/// 
	/// </summary>
	/// <param name="src"></param>
	/// <returns></returns>
	DftComponents getPSD(const cv::Mat& psd);

	cv::Mat getPeaks(const cv::Mat& psd);

	void filterMagnitude(cv::Mat& peaks, cv::Mat& magnitude);

	cv::Mat getIFT(DftComponents dftComps);


	struct DftComponents
	{
		DftComponents(const cv::Mat& src)
		{
			magnitude = cv::Mat(src.rows, src.cols, CV_32FC1);
			phase = cv::Mat(src.rows, src.cols, CV_32FC1);
			powerSpectrum = cv::Mat(src.rows, src.cols, CV_32FC1);
		};

		cv::Mat phase, magnitude, powerSpectrum;
	};

private:

	cv::Mat getDFT(const cv::Mat& src);
	DftComponents getDftComponents(cv::Mat& dft);
	void fftShift(cv::Mat& src);
	void log(cv::Mat& src);
	void normalize(cv::Mat& src, float min, float max);
	const cv::Point& centerPoint;
};

}