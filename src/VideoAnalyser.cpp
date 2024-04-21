//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include "iris/VideoAnalyser.h"
#include "iris/Configuration.h"
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include "IrisFrame.h"
#include "FlashDetection.h"
#include "PatternDetection.h"
#include "FrameRgbConverter.h"
#include "utils/FrameConverter.h"
#include "iris/Log.h"
#include <filesystem>
#include <chrono>
#include "iris/Result.h"
#include <string>
#include "utils/JsonWrapper.h"
#include <opencv2/imgproc.hpp>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}


namespace iris
{
	VideoAnalyser::VideoAnalyser(Configuration* configuration)
	{
		LOG_CORE_WARNING("This output report is for informational purposes only and should not be used as certification or validation of compliance with any legal, regulatory or other requirements");

		cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_ERROR);
		m_configuration = configuration;
#ifdef _DEBUG
		LOG_CORE_DEBUG(cv::getBuildInformation()); //OpenCV build info
#endif // DEBUG
	}

	VideoAnalyser::~VideoAnalyser()
	{
	}

	void VideoAnalyser::Init(const std::string& videoName, bool flagJson)
	{
		if (m_configuration->FrameResizeEnabled())
		{
			float resizedFrameProportion;
			resizedFrameProportion = m_configuration->GetFrameResizeProportion();
			LOG_CORE_INFO("Resizing frames at: {0}%", resizedFrameProportion * 100);
			m_videoInfo.frameSize = cv::Size(m_videoInfo.frameSize.width * resizedFrameProportion, m_videoInfo.frameSize.height * resizedFrameProportion);
		}

		m_flashDetection = new FlashDetection(m_configuration, m_videoInfo.fps, m_videoInfo.frameSize);
		m_photosensitivityDetector.push_back(m_flashDetection);
		m_frameSrgbConverter = new EA::EACC::Utils::FrameConverter(m_configuration->GetFrameSrgbConverterParams());
		m_patternDetection = new PatternDetection(m_configuration, m_videoInfo.fps, m_videoInfo.frameSize);

		if (m_configuration->PatternDetectionEnabled())
		{
			m_photosensitivityDetector.push_back(m_patternDetection);
		}

		std::string frameDataFile = m_configuration->GetResultsPath() + videoName + "/framedata.csv";
		Log::SetDataLoggerFile(frameDataFile.c_str());

		if (flagJson)
		{
			m_resultJsonPath = m_configuration->GetResultsPath() + videoName + "/" + "result" + ".json";
			m_frameDataJsonPath = m_configuration->GetResultsPath() + videoName + "/" + "frameData" + ".json";
		}

		const char* luminanceType = m_configuration->GetLuminanceType() == Configuration::LuminanceType::CD ? "CD" : "RELATIVE";
		LOG_CORE_INFO("Luminance Type: {0}", luminanceType);
		
		LOG_CORE_INFO("Pattern Detection: {0}", m_configuration->PatternDetectionEnabled());

		LOG_CORE_INFO("Frame Resize: {0}", m_configuration->FrameResizeEnabled());
		
		LOG_CORE_INFO("Safe Area Proportion: {0}", m_configuration->GetSafeAreaProportion());

		LOG_CORE_INFO("Write json file: {0}", flagJson);
	}

	void VideoAnalyser::DeInit()
	{
		if (m_flashDetection != nullptr)
		{
			delete m_flashDetection; m_flashDetection = nullptr;
		}
		if (m_patternDetection != nullptr)
		{
			delete m_patternDetection; m_patternDetection = nullptr;
		}
		if (m_frameSrgbConverter != nullptr)
		{
			delete m_frameSrgbConverter; m_frameSrgbConverter = nullptr;
		}

		m_photosensitivityDetector.clear();
	}

	void VideoAnalyser::AnalyseVideo(bool flagJson, const char* sourceVideo)
	{
		cv::VideoCapture video(sourceVideo);
		std::string videoName = std::filesystem::path(sourceVideo).filename().string();

		if (VideoIsOpen(sourceVideo, video, videoName.c_str()))
		{
			Init(videoName, flagJson);
			SetOptimalCvThreads(m_videoInfo.frameSize);
			cv::Mat frame;
			video.read(frame);

			unsigned int numFrames = 0;
			unsigned int lastPercentage = 0;

			LOG_DATA_INFO(FrameData().CsvColumns());
			LOG_CORE_INFO("Video analysis started");
			
			auto start = std::chrono::steady_clock::now();

			FrameDataJson lineGraphData;
			FrameDataJson nonPassData;

			if (flagJson) 
			{ 
				lineGraphData.reserveLineGraphData(m_videoInfo.frameCount);
				nonPassData.reserve(m_videoInfo.frameCount * 0.25); //reserve at least one quarter of frames
			}

			while (!frame.empty())
			{
				FrameData data(numFrames + 1, 1000.0 * (double)numFrames / m_videoInfo.fps);
				if (m_configuration->FrameResizeEnabled())
				{
					cv::resize(frame, frame, m_videoInfo.frameSize);
				}
				AnalyseFrame(frame, numFrames, data);
				
				video.read(frame); //obtain new frame

				UpdateProgress(numFrames, m_videoInfo.frameCount, lastPercentage);
				numFrames++;

				LOG_DATA_INFO(data.ToCSV());

				if (flagJson)
				{
					lineGraphData.push_back_lineGraphData(data);
					//save non-pass frame result frame data in json
					if ((int)data.luminanceFrameResult > 0 || (int)data.redFrameResult > 0 || (int)data.patternFrameResult > 0)
					{
						nonPassData.push_back(data);
					}
				}
			}

			auto end = std::chrono::steady_clock::now();
			LOG_CORE_INFO("Video analysis ended");
			unsigned int elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			LOG_CORE_INFO("Elapsed time: {0} ms", elapsedTime);

			if (m_flashDetection->isFail() || m_patternDetection->isFail())
			{
				LOG_CORE_INFO("Video Result: FAIL");
			}
			else
			{
				LOG_CORE_INFO("Video Result: PASS");
			}

			if (flagJson)
			{
				Result result;
				m_flashDetection->setResult(result);
				if (m_patternDetection != nullptr) { m_patternDetection->setResult(result); }
				result.VideoLen = m_videoInfo.duration * 1000;
				result.AnalysisTime = elapsedTime;
				result.TotalFrame = m_videoInfo.frameCount;
				SerializeResults(result, lineGraphData, nonPassData);
			}

			DeInit();
		}

		video.release();
	}

	void VideoAnalyser::AnalyseFrame(cv::Mat& frame, unsigned int& frameIndex, FrameData& data)
	{
		IrisFrame irisFrame(&(frame), m_frameSrgbConverter->Convert(frame), data);

		m_flashDetection->setLuminance(irisFrame);
		for (auto detector : m_photosensitivityDetector)
		{
			detector->checkFrame(irisFrame, frameIndex, data);
		}

		irisFrame.Release();
	}

	void VideoAnalyser::SetOptimalCvThreads(cv::Size size)
	{
		int num_threads = 1;

		//Detection of max num of threads available
		int max_num_threads = std::thread::hardware_concurrency();	

		//Depending of the size of the frame, different number of threads will be used
		//If the resolution is less or more than 1080, 6 threads will be used 
		//If not more than 6 threads will be used with higher resolutions like 2K. 
		//The research concluded that the optimal number of threads for a 1080p resolution 
		//is 6 and for a 4K resolution is 10. 
		//The operation (1 / 270) * (size.height - 1080) + 6 is a rule of three that 
		//ensures these results.
		num_threads = std::round(size.height <= 1080)				
			? 6 : (1 / 270) * (size.height - 1080) + 6;				

		//If the PC does not have the num of threads that the program requires,
		//we will take the max num of threads that are available in the machine.
		int threads_used = max_num_threads < num_threads && max_num_threads != 0 ? max_num_threads : num_threads;	

		//Setting number of used threads
		cv::setNumThreads(threads_used);							
		LOG_CORE_INFO("Number of threads used: {0}", threads_used);
	}

	bool VideoAnalyser::VideoIsOpen(const char* sourceVideo, cv::VideoCapture& video, const char* videoName)
	{
		if (video.isOpened())
		{
			m_videoInfo.fps = GetVideoFps(sourceVideo);
			if (m_videoInfo.fps == -1)
			{
				LOG_CORE_ERROR("Video FPS extraction with FFmpeg failed, attempting with OpenCV");
				m_videoInfo.fps = video.get(cv::CAP_PROP_FPS);
			}

			m_videoInfo.frameCount = video.get(cv::CAP_PROP_FRAME_COUNT);
			m_videoInfo.frameSize = cv::Size(video.get(cv::CAP_PROP_FRAME_WIDTH), video.get(cv::CAP_PROP_FRAME_HEIGHT));
			m_videoInfo.duration = m_videoInfo.frameCount / (float)m_videoInfo.fps;

			LOG_CORE_INFO("Video: {0} opened successful", videoName);
			LOG_CORE_INFO("Video FPS: {}", m_videoInfo.fps);
			LOG_CORE_INFO("Total frames: {0}", m_videoInfo.frameCount); 
			LOG_CORE_INFO("Video resolution: {0}x{1}", m_videoInfo.frameSize.width, m_videoInfo.frameSize.height);
			LOG_CORE_INFO("Duration: {0}s", m_videoInfo.duration);
			return true;
		}

		LOG_CORE_ERROR("Video: {0} could not be opened\nInformation is missing or corrupt", videoName);
		throw std::runtime_error("Video: "+ std::string(videoName) +"could not be opened\nInformation is missing or corrupt");
		return false;
	}

	void VideoAnalyser::UpdateProgress(unsigned int& numFrames, const unsigned long& totalFrames, unsigned int& lastPercentage)
	{
		unsigned int progress = numFrames / (float)totalFrames * 100.0f;

		if (progress != 0 && progress % 10 == 0 && lastPercentage != progress) //display progress
		{
			lastPercentage = progress;
			LOG_CORE_DEBUG("Analysed {0}%", progress);
		}
	}

	int VideoAnalyser::GetVideoFps(const char* filePath)
	{
		AVFormatContext* pFormatContext = avformat_alloc_context();

		if (avformat_open_input(&pFormatContext, filePath, NULL, NULL) != 0) {
			LOG_CORE_ERROR("Unable to open file {}", filePath);
			return -1;
		}

		// Retrieve stream information
		if (avformat_find_stream_info(pFormatContext, NULL) < 0) {
			LOG_CORE_ERROR("Failed to retrieve stream info");
			return -1;
		}

		// Get the first video stream
		int videoStreamIndex = -1;
		for (int i = 0; i < pFormatContext->nb_streams; i++) {
			if (pFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
				videoStreamIndex = i;
				break;
			}
		}

		if (videoStreamIndex == -1) {
			LOG_CORE_ERROR("No video stream found");
			return -1;
		}

		// Get the frame rates
		AVRational r_frame_rate = pFormatContext->streams[videoStreamIndex]->r_frame_rate;
		AVRational avg_frame_rate = pFormatContext->streams[videoStreamIndex]->avg_frame_rate;

		LOG_CORE_DEBUG("r_frame_rate: {}/{}", r_frame_rate.num, r_frame_rate.den);
		LOG_CORE_DEBUG("avg_frame_rate: {}/{}", avg_frame_rate.num, avg_frame_rate.den);

		int fps = round(avg_frame_rate.num / (float)avg_frame_rate.den);

		// Close the video file
		avformat_close_input(&pFormatContext);
		avformat_free_context(pFormatContext);
		return fps;
	}

	void VideoAnalyser::SerializeResults(const Result& result, const FrameDataJson& lineGraphData, const FrameDataJson& nonPassData)
	{
		EA::EACC::Utils::JsonWrapper resultJson;

		resultJson.SetParam("FrameDataResult", "FrameCsvRepositoryKey", "00000000-0000-0000-0000-000000000000"); //Value initialized in Lambda
		resultJson.SetParam("FrameDataResult", "FrameResultJsonRepositoryKey", "00000000-0000-0000-0000-000000000000");
		resultJson.SetParam("FrameDataResult", "FrameDataJsonRepositoryKey", "00000000-0000-0000-0000-000000000000");
		resultJson.SetParam("TotalFrame", result.TotalFrame);
		resultJson.SetParam("AnalyzeTime", msToTimeSpan(result.AnalysisTime));
		resultJson.SetParam("VideoLen", msToTimeSpan(result.VideoLen));
		resultJson.SetParam("OverallResult", result.OverallResult);
		resultJson.SetParam("Results", result.Results);
		resultJson.SetParam("TotalLuminanceIncidents", result.totalLuminanceIncidents);
		resultJson.SetParam("TotalRedIncidents", result.totalRedIncidents);
		resultJson.SetParam("PatternFailFrames", result.patternFailFrames);
		resultJson.WriteFile(m_resultJsonPath.c_str());
		LOG_CORE_INFO("Results Json written to {}", m_resultJsonPath);


		EA::EACC::Utils::JsonWrapper frameDataJson;
		frameDataJson.SetParam("NonPassFrameData", nonPassData);
		frameDataJson.SetParam("LineGraphFrameData", lineGraphData);

		frameDataJson.WriteFile(m_frameDataJsonPath.c_str());
		LOG_CORE_INFO("Non Pass Json written to {}", m_frameDataJsonPath);
	}
}
