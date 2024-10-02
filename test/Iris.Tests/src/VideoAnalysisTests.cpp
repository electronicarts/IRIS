//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include "IrisLibTest.h"
#include "iris/VideoAnalyser.h"
#include <opencv2/videoio.hpp>
#include <fstream>
#include <string>
#include "FrameData.h"

namespace iris::Tests
{
	class VideoAnalysisTests : public IrisLibTest {
	protected:
		void SetUp() override {
			configuration.SetLuminanceType(Configuration::LuminanceType::RELATIVE);
			IrisLibTest::SetUp();
		}

		void TestVideoAnalysis(VideoAnalyser& videoAnalyser, cv::VideoCapture& video, const char* sourceLog)
		{
			std::ifstream logFile;
			logFile.open(sourceLog);

			std::string line;
			std::getline(logFile, line); //discard first line
			std::string testVideo = "testVideo";
			videoAnalyser.Init(testVideo);
			cv::Mat frame;
			video.read(frame);
			unsigned int numFrames = 0;

			while (!frame.empty())
			{
				FrameData data(numFrames + 1, 1000.0 * (double)numFrames / video.get(cv::CAP_PROP_FPS));
				//FrameData data(numFrames + 1, video.get(cv::CAP_PROP_FRAME_COUNT));
				videoAnalyser.AnalyseFrame(frame, numFrames, data);

				video.read(frame); //obtain new frame
				numFrames++;

				//check framedata
				std::getline(logFile, line);
				CheckFrameData(line, data);
			}

			logFile.close();
			videoAnalyser.DeInit();
		}
	
		void CheckFrameData(std::string& line, FrameData& data)
		{
			std::vector<std::string> logFrameData;
			std::stringstream ss(line);
			while (ss.good())
			{
				std::string substr;
				getline(ss, substr, ',');
				logFrameData.push_back(substr);
			}

            EXPECT_EQ(std::stoi(logFrameData[0]), data.Frame) << "Frame: " << data.Frame << " Expected: " << std::stoi(logFrameData[0]);

            EXPECT_TRUE(CompareFloat(std::stof(logFrameData[2]), data.LuminanceAverage)) 
				<< "Frame: " << data.Frame << " Expected: " << std::stof(logFrameData[2]) << " Actual: " << data.LuminanceAverage;
            std::string str = data.proportionToPercentage(std::stof(logFrameData[3]));
            EXPECT_EQ(str, data.LuminanceFlashArea) 
				<< "Frame: " << data.Frame << " Expected: " << str << " Actual: " << data.LuminanceFlashArea;
            EXPECT_TRUE(CompareFloat(std::stof(logFrameData[4]), data.AverageLuminanceDiff)) 
				<< "Frame: " << data.Frame << " Expected: " << std::stof(logFrameData[4]) << " Actual: " << data.AverageLuminanceDiff;
            EXPECT_TRUE(CompareFloat(std::stof(logFrameData[5]), data.AverageLuminanceDiffAcc)) 
				<< "Frame: " << data.Frame << " Expected: " << std::stof(logFrameData[5]) << " Actual: " << data.AverageLuminanceDiffAcc;
			
            EXPECT_TRUE(CompareFloat(std::stof(logFrameData[6]), data.RedAverage)) 
				<< "Frame: " << data.Frame << " Expected: " << std::stof(logFrameData[6]) << " Actual: " << data.RedAverage;
            str = data.proportionToPercentage(std::stof(logFrameData[7]));
            EXPECT_EQ(str, data.RedFlashArea) 
				<< "Frame: " << data.Frame << " Expected: " << str << " Actual: " << data.RedFlashArea;
            EXPECT_TRUE(CompareFloat(std::stof(logFrameData[8]), data.AverageRedDiff)) 
				<< "Frame: " << data.Frame << " Expected: " << std::stof(logFrameData[8]) << " Actual: " << data.AverageRedDiff;
            EXPECT_TRUE(CompareFloat(std::stof(logFrameData[9]), data.AverageRedDiffAcc)) 
				<< "Frame: " << data.Frame << " Expected: " << std::stof(logFrameData[9]) << " Actual: " << data.AverageRedDiffAcc;

            EXPECT_EQ(std::stof(logFrameData[10]), data.PatternRisk) 
				<< "Frame: " << data.Frame << " Expected: " << std::stof(logFrameData[10]) << " Actual: " << data.PatternRisk;

            EXPECT_EQ(std::stoi(logFrameData[11]), data.LuminanceTransitions) 
				<< "Frame: " << data.Frame << " Expected: " << std::stoi(logFrameData[11]) << " Actual: " << data.LuminanceTransitions;
            EXPECT_EQ(std::stoi(logFrameData[12]), data.RedTransitions) 
				<< "Frame: " << data.Frame << " Expected: " << std::stoi(logFrameData[12]) << " Actual: " << data.RedTransitions;
            EXPECT_EQ(std::stoi(logFrameData[13]), data.LuminanceExtendedFailCount) 
				<< "Frame: " << data.Frame << " Expected: " << std::stoi(logFrameData[13]) << " Actual: " << data.LuminanceExtendedFailCount;
            EXPECT_EQ(std::stoi(logFrameData[14]), data.RedExtendedFailCount) 
				<< "Frame: " << data.Frame << " Expected: " << std::stoi(logFrameData[14]) << " Actual: " << data.RedExtendedFailCount;

            EXPECT_EQ(std::stoi(logFrameData[15]), (int)data.luminanceFrameResult) 
				<< "Frame: " << data.Frame << " Expected: " << std::stoi(logFrameData[15]) << " Actual: " << (int)data.luminanceFrameResult;
            EXPECT_EQ(std::stoi(logFrameData[16]), (int)data.redFrameResult) 
				<< "Frame: " << data.Frame << " Expected: " << std::stoi(logFrameData[16]) << " Actual: " << (int)data.redFrameResult;
            EXPECT_EQ(std::stoi(logFrameData[17]), (int)data.patternFrameResult) 
				<< "Frame: " << data.Frame << " Expected: " << std::stoi(logFrameData[17]) << " Actual: " << (int)data.patternFrameResult;
		}
	};

	TEST_F(VideoAnalysisTests, 2Hz_5s_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/2Hz_5s.mp4";
		VideoAnalyser videoAnalyser(&configuration);
		
		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video, nullptr))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/2Hz_5s_RELATIVE.csv");
		}
	}

	TEST_F(VideoAnalysisTests, 2Hz_6s_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/2Hz_6s.mp4";
		VideoAnalyser videoAnalyser(&configuration);

		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video, nullptr))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/2Hz_6s_RELATIVE.csv");
		}
	}

	TEST_F(VideoAnalysisTests, 3Hz_6s_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/3Hz_6s.mp4";
		VideoAnalyser videoAnalyser(&configuration);

		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video, nullptr))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/3Hz_6s_RELATIVE.csv");
		}
	}

	TEST_F(VideoAnalysisTests, extendedFLONG_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/extendedFLONG.mp4";
		VideoAnalyser videoAnalyser(&configuration);

		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video, nullptr))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/extendedFLONG_RELATIVE.csv");
		}
	}

	TEST_F(VideoAnalysisTests, GradualRedIncrease_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/GradualRedIncrease.mp4";
		VideoAnalyser videoAnalyser(&configuration);

		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video, nullptr))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/GradualRedIncrease_RELATIVE.csv");
		}
	}

	TEST_F(VideoAnalysisTests, gray_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/gray.mp4";
		VideoAnalyser videoAnalyser(&configuration);

		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video, nullptr))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/gray_RELATIVE.csv");
		}
	}

	TEST_F(VideoAnalysisTests, intermitentEF_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/intermitentEF.mp4";
		VideoAnalyser videoAnalyser(&configuration);

		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video, nullptr))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/intermitentEF_RELATIVE.csv");
		}
	}
}