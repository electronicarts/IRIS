//Copyright (C) 2023 Electronic Arts, Inc.  All rights reserved.

#include "IrisLibTest.h"
#include "iris/VideoAnalyser.h"
#include <opencv2/videoio.hpp>
#include <fstream>
#include <string>
#include "iris/FrameData.h"

namespace iris::Tests
{
	class VideoAnalysisTests : public IrisLibTest {
	protected:
		void SetUp() override 
		{
			IrisLibTest::SetUp();
		}

		void TestVideoAnalysis(VideoAnalyser& videoAnalyser, cv::VideoCapture& video, const char* sourceLog, bool timeAnalysis = false)
		{
			std::ifstream logFile;
			logFile.open(sourceLog);

			std::string line;
			std::getline(logFile, line); //discard first line
			std::string testVideo = "./testVideo.mp4";
			
			if (timeAnalysis)
			{
				cv::Size size = videoAnalyser.GetVideoInfo().frameSize;
				videoAnalyser.RealTimeInit(size);
			}
			else
			{
				videoAnalyser.Init(testVideo);
			}
			cv::Mat frame;
			video.read(frame);
			unsigned int numFrames = 0;

			while (!frame.empty())
			{
				FrameData data(numFrames + 1, 1000.0 * (double)numFrames / video.get(cv::CAP_PROP_FPS));
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

			EXPECT_EQ(std::stoi(logFrameData[0]), data.Frame) << "Frame: " << data.Frame << '\n';

			EXPECT_TRUE(CompareFloat(std::stof(logFrameData[2]), data.LuminanceAverage)) << "Frame: " << data.Frame << '\n';
			std::string str = data.proportionToPercentage(std::stof(logFrameData[3]));
			EXPECT_EQ(str, data.LuminanceFlashArea) << "Frame: " << data.Frame << '\n';
			EXPECT_TRUE(CompareFloat(std::stof(logFrameData[4]), data.AverageLuminanceDiff)) << "Frame: " << data.Frame << '\n';
			EXPECT_TRUE(CompareFloat(std::stof(logFrameData[5]), data.AverageLuminanceDiffAcc)) << "Frame: " << data.Frame << '\n';
			
			EXPECT_TRUE(CompareFloat(std::stof(logFrameData[6]), data.RedAverage)) << "Frame: " << data.Frame << '\n';
			str = data.proportionToPercentage(std::stof(logFrameData[7]));
			EXPECT_EQ(str, data.RedFlashArea) << "Frame: " << data.Frame << '\n';
			EXPECT_TRUE(CompareFloat(std::stof(logFrameData[8]), data.AverageRedDiff)) << "Frame: " << data.Frame << '\n';
			EXPECT_TRUE(CompareFloat(std::stof(logFrameData[9]), data.AverageRedDiffAcc)) << "Frame: " << data.Frame << '\n';

			EXPECT_EQ(std::stof(logFrameData[10]), data.PatternRisk) << "Frame: " << data.Frame << '\n';

			EXPECT_EQ(std::stoi(logFrameData[11]), data.LuminanceTransitions) << "Frame: " << data.Frame << '\n';
			EXPECT_EQ(std::stoi(logFrameData[12]), data.RedTransitions) << "Frame: " << data.Frame << '\n';
			EXPECT_EQ(std::stoi(logFrameData[13]), data.LuminanceExtendedFailCount) << "Frame: " << data.Frame << '\n';
			EXPECT_EQ(std::stoi(logFrameData[14]), data.RedExtendedFailCount) << "Frame: " << data.Frame << '\n';

			EXPECT_EQ(std::stoi(logFrameData[15]), (int)data.luminanceFrameResult) << "Frame: " << data.Frame << '\n';
			EXPECT_EQ(std::stoi(logFrameData[16]), (int)data.redFrameResult) << "Frame: " << data.Frame << '\n';
			EXPECT_EQ(std::stoi(logFrameData[17]), (int)data.patternFrameResult) << "Frame: " << data.Frame << '\n';
		}
	};

	TEST_F(VideoAnalysisTests, 2Hz_5s_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/2Hz_5s.mp4";
		VideoAnalyser videoAnalyser(&configuration);
		
		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/2Hz_5s_RELATIVE.csv");
		}
	}

	TEST_F(VideoAnalysisTests, 2Hz_6s_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/2Hz_6s.mp4";
		VideoAnalyser videoAnalyser(&configuration);

		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/2Hz_6s_RELATIVE.csv");
		}
	}

	TEST_F(VideoAnalysisTests, 3Hz_6s_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/3Hz_6s.mp4";
		VideoAnalyser videoAnalyser(&configuration);

		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/3Hz_6s_RELATIVE.csv");
		}
	}

	TEST_F(VideoAnalysisTests, extendedFLONG_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/extendedFLONG.mp4";
		VideoAnalyser videoAnalyser(&configuration);

		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/extendedFLONG_RELATIVE.csv");
		}
	}

	TEST_F(VideoAnalysisTests, GradualRedIncrease_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/GradualRedIncrease.mp4";
		VideoAnalyser videoAnalyser(&configuration);

		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/GradualRedIncrease_RELATIVE.csv");
		}
	}

	TEST_F(VideoAnalysisTests, gray_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/gray.mp4";
		VideoAnalyser videoAnalyser(&configuration);

		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/gray_RELATIVE.csv");
		}
	}

	TEST_F(VideoAnalysisTests, intermitentEF_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/intermitentEF.mp4";
		VideoAnalyser videoAnalyser(&configuration);

		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/intermitentEF_RELATIVE.csv");
		}
	}

	TEST_F(VideoAnalysisTests, RealTime_2Hz_5s_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/2Hz_5s.mp4";
		VideoAnalyser videoAnalyser(&configuration);

		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/2Hz_5s_RELATIVE.csv", true);
		}
	}

	TEST_F(VideoAnalysisTests, RealTime_2Hz_6s_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/2Hz_6s.mp4";
		VideoAnalyser videoAnalyser(&configuration);

		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/2Hz_6s_RELATIVE.csv", true);
		}
	}

	TEST_F(VideoAnalysisTests, RealTime_3Hz_6s_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/3Hz_6s.mp4";
		VideoAnalyser videoAnalyser(&configuration);

		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/3Hz_6s_RELATIVE.csv", true);
		}
	}

	TEST_F(VideoAnalysisTests, RealTime_extendedFLONG_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/extendedFLONG.mp4";
		VideoAnalyser videoAnalyser(&configuration);

		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/extendedFLONG_RELATIVE.csv", true);
		}
	}

	TEST_F(VideoAnalysisTests, RealTime_GradualRedIncrease_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/GradualRedIncrease.mp4";
		VideoAnalyser videoAnalyser(&configuration);

		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/GradualRedIncrease_RELATIVE.csv", true);
		}
	}

	TEST_F(VideoAnalysisTests, RealTime_gray_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/gray.mp4";
		VideoAnalyser videoAnalyser(&configuration);

		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/gray_RELATIVE.csv", true);
		}
	}

	TEST_F(VideoAnalysisTests, RealTime_intermitentEF_Video_Test)
	{
		const char* sourceVideo = "data/TestVideos/intermitentEF.mp4";
		VideoAnalyser videoAnalyser(&configuration);

		cv::VideoCapture video(sourceVideo);
		if (videoAnalyser.VideoIsOpen(sourceVideo, video))
		{
			TestVideoAnalysis(videoAnalyser, video, "data/ExpectedVideoLogFiles/intermitentEF_RELATIVE.csv", true);
		}
	}
}