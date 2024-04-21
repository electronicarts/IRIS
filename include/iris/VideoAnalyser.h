//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

/////////////////////////////////////////////////////////////////////////////
// VideoAnalyser is the main object to with which to run a photosensitivity
// compliance check on a video, it analizes frames of a video running both 
// flash and pattern detection to check the guidelines for compliance issues.
// When calling AnalyseVideo, initialization, video analysis, logging and 
// deinitialization is managed by VideoAnalyser.
// If not, all these steps will have to be manually managed.
/////////////////////////////////////////////////////////////////////////////

//#if defined BUILD_SHARED_LIBS && WIN32
//#define IRIS_API __declspec(dllexport)
//#else
//#define IRIS_API __declspec(dllimport)
//#endif


#pragma once
#include <opencv2/core/types.hpp>

namespace cv
{
	class VideoCapture;
	class Mat;
}

namespace EA::EACC::Utils
{
	class FrameConverter;
}

namespace iris
{
	class Configuration;
	class FlashDetection;
	class PatternDetection;
	class PhotosensitivityDetector;
	class FrameData;
	struct FrameDataJson;
	struct Result;

	class VideoAnalyser
	{
	public:
		VideoAnalyser(Configuration* configuration);
		~VideoAnalyser();

		/// <summary>
		/// Initializes FlashDetection and PatternDetection
		/// </summary>
		void Init(const std::string& videoName, bool flagJson = false);

		/// <summary>
		/// Release FlashDetection and PatternDection
		/// </summary>
		void DeInit();

		/// <summary>
		/// Checks if the video file can be opened and read
		/// </summary>
		bool VideoIsOpen(const char* sourceVideo, cv::VideoCapture& video, const char* videoName);

		/// <summary>
		/// Anlyses video to check photosensitivity
		/// </summary>
		/// <param name="flagJson"> If true, saves Result to json file</param>
		/// <param name="sourceVideo"> video file path</param>
		void AnalyseVideo(bool flagJson, const char* sourceVideo);

		/// <summary>
		/// Frame analysis for checking for photosensitivity for tracked issues (flashes/patterns)
		/// </summary>
		void AnalyseFrame(cv::Mat& frame, unsigned int& frameIndex, FrameData& data);

		/// <summary>
		/// Set optimal number of threads depending on the frame resolution
		/// </summary>
		void SetOptimalCvThreads(cv::Size size);

		
		struct VideoInfo
		{
			int fps; //video frames per second
			int frameCount; //number of frames in video
			float duration; //duration in seconds
			cv::Size frameSize; //video resolution
		};

		inline VideoInfo GetVideoInfo() { return m_videoInfo; }
		
	private:

		/// <summary>
		/// Obtains the avg_frame_rate of the video
		/// </summary>
		/// <param name="sourceVideo">Video file path</param>
		/// <returns>frames per second in video</returns>
		int GetVideoFps(const char* sourceVideo);

		void SerializeResults(const Result& result, const FrameDataJson& lineGraphData, const FrameDataJson& nonPassData);

		/// <summary>
		/// Updates the current analysis progress
		/// </summary>
		void UpdateProgress(unsigned int& numFrames, const unsigned long& totalFrames, unsigned int& lastPercentage);

		Configuration* m_configuration = nullptr;
		FlashDetection* m_flashDetection = nullptr;
		PatternDetection* m_patternDetection = nullptr;
		std::vector<PhotosensitivityDetector*> m_photosensitivityDetector;

		EA::EACC::Utils::FrameConverter* m_frameSrgbConverter = nullptr;

		std::string m_resultJsonPath;
		std::string m_frameDataJsonPath;

		VideoInfo m_videoInfo;
	};
}