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

	void VideoAnalyser::Init(const short& fps, const cv::Size& size, const std::string& videoName, bool flagJson)
	{
		m_flashDetection = new FlashDetection(m_configuration, fps, size);
		m_photosensitivityDetector.push_back(m_flashDetection);
		m_frameSrgbConverter = new EA::EACC::Utils::FrameConverter(m_configuration->GetFrameSrgbConverterParams());
		m_patternDetection = new PatternDetection(m_configuration, fps, size);

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

		LOG_CORE_INFO("Video analysis FPS: {}", fps);

		const char* luminanceType = m_configuration->GetLuminanceType() == Configuration::LuminanceType::CD ? "CD" : "RELATIVE";
		LOG_CORE_INFO("Luminance Type: {0}", luminanceType);
		
		LOG_CORE_INFO("Pattern Detection: {0}", m_configuration->PatternDetectionEnabled());
		
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

		if (VideoIsOpen(video, videoName.c_str()))
		{
			Init((short)round(video.get(cv::CAP_PROP_FPS)), cv::Size(video.get(cv::CAP_PROP_FRAME_WIDTH), video.get(cv::CAP_PROP_FRAME_HEIGHT)), videoName, flagJson);
			cv::Mat frame;
			video.read(frame);

			int numFrames = 0;
			int lastPercentage = 0;

			LOG_DATA_INFO(FrameData().CsvColumns());
			LOG_CORE_INFO("Video analysis started");
			
			auto start = std::chrono::steady_clock::now();

			FrameDataJson lineGraphData;
			FrameDataJson nonPassData;

			if (flagJson) 
			{ 
				lineGraphData.reserveLineGraphData((int)video.get(cv::CAP_PROP_FRAME_COUNT));
				nonPassData.reserve((int)(video.get(cv::CAP_PROP_FRAME_COUNT) * 0.25)); //reserve at least one quarter of frames
			}

			while (!frame.empty())
			{
				FrameData data(numFrames + 1, 1000.0 * (double)numFrames / video.get(cv::CAP_PROP_FPS));
				AnalyseFrame(frame, numFrames, data);
				
				video.read(frame); //obtain new frame

				UpdateProgress(numFrames, video.get(cv::CAP_PROP_FRAME_COUNT), lastPercentage);
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
			int elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
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
				result.VideoLen = (int)(video.get(cv::CAP_PROP_FRAME_COUNT) / video.get(cv::CAP_PROP_FPS) * 1000);
				result.AnalysisTime = elapsedTime;
				result.TotalFrame = (int)video.get(cv::CAP_PROP_FRAME_COUNT);
				SerializeResults(result, lineGraphData, nonPassData);
			}

			DeInit();
		}

		video.release();
	}

	void VideoAnalyser::AnalyseFrame(cv::Mat& frame, int& frameIndex, FrameData& data)
	{
		IrisFrame irisFrame(&(frame), m_frameSrgbConverter->Convert(frame), data);

		m_flashDetection->setLuminance(irisFrame);
		for (auto detector : m_photosensitivityDetector)
		{
			detector->checkFrame(irisFrame, frameIndex, data);
		}

		irisFrame.Release();
	}

	bool VideoAnalyser::VideoIsOpen(cv::VideoCapture& video, const char* videoName)
	{
		if (video.isOpened())
		{
			LOG_CORE_INFO("Video: {0} opened successful", videoName);
			LOG_CORE_INFO("Total frames: {0}", video.get(cv::CAP_PROP_FRAME_COUNT)); 
			LOG_CORE_INFO("FPS: {0}", video.get(cv::CAP_PROP_FPS));
			LOG_CORE_INFO("Duration: {0}s", video.get(cv::CAP_PROP_FRAME_COUNT) / video.get(cv::CAP_PROP_FPS));
			LOG_CORE_INFO("Video resolution: {0}x{1}", video.get(cv::CAP_PROP_FRAME_WIDTH), video.get(cv::CAP_PROP_FRAME_HEIGHT));
			return true;
		}

		LOG_CORE_ERROR("Video: {0} could not be opened\nInformation is missing or corrupt", videoName);
		throw std::runtime_error("Video: "+ std::string(videoName) +"could not be opened\nInformation is missing or corrupt");
		return false;
	}

	void VideoAnalyser::UpdateProgress(int& numFrames, const long& totalFrames, int& lastPercentage)
	{
		int progress = numFrames / (float)totalFrames * 100.0f;

		if (progress != 0 && progress % 10 == 0 && lastPercentage != progress) //display progress
		{
			lastPercentage = progress;
			LOG_CORE_DEBUG("Analysed {0}%", progress);
		}
	}

	void VideoAnalyser::SerializeResults(const Result& result, const FrameDataJson& lineGraphData, const FrameDataJson& nonPassData)
	{
		EA::EACC::Utils::JsonWrapper resultJson;

		resultJson.SetParam("FrameDataResult", "FrameCsvRepositoryKey", "00000000-0000-0000-0000-000000000000"); //Value initialized in Lambda
		resultJson.SetParam("FrameDataResult", "FrameResultJsonRepositoryKey", "00000000-0000-0000-0000-000000000000");
		resultJson.SetParam("FrameDataResult", "FrameDataJsonRepositoryKey", "00000000-0000-0000-0000-000000000000");
		resultJson.SetParam("TotalFrame", result.TotalFrame);
		resultJson.SetParam("AnalyzeTimeString", msToTimeSpan(result.AnalysisTime));
		resultJson.SetParam("VideoLenString", msToTimeSpan(result.VideoLen));
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