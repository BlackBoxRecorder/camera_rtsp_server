// RTSP_Server.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <iostream>
#include "xop/RtspServer.h"
#include "xop/JPEGSource.h"
#include <opencv2/opencv.hpp>

class RTSP_Server
{
private:
  std::mutex mutex;
  std::mutex rtspMutex;
  std::string videosource;
  std::atomic_bool isWorking = true;
  cv::Mat frame;
  std::string rtsp_frame_data;



private:
  std::thread capture_thread;
  std::thread camera_frame_encode;

  std::thread rtspsvrThread;
  std::thread rtspSendFrameThread;

public:

  void run();

  void encoding();

  void startRtspServer();

  void sendFrameThread(xop::RtspServer* rtsp_server, xop::MediaSessionId session_id);

  void read_source();
};