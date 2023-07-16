// RTSP_Server.cpp: 定义应用程序的入口点。
//

#include "RTSP_Server.h"

using namespace std;
using namespace cv;




void RTSP_Server::read_source()
{
  do
  {
    // initialize a video capture object
    cv::VideoCapture vid_capture(0);

    // Print error message if the stream is invalid
    if (!vid_capture.isOpened())
    {
      cout << "Error opening video stream or file" << endl;
    }
    else
    {
      // Obtain fps and frame count by get() method and print
      int fps = vid_capture.get(CAP_PROP_FPS);
      cout << "Frames per second :" << fps;
    }


    // Read the frames to the last frame
    while (vid_capture.isOpened())
    {
      // Initialise frame matrix
      Mat frame;

      // Initialize a boolean to check if frames are there or not
      bool isSuccess = vid_capture.read(frame);

      // If frames are present, show it
      if (isSuccess == true)
      {
        std::lock_guard<std::mutex> guard(mutex);

        /* bool flag = this->frame.empty();

         this->frame = frame.clone();
         if (flag) {
           this->streamThread = std::thread(&cvstream::stream_publish, this);
         }*/
      }

      if (isSuccess == false)
      {
        cout << "video end" << endl;
        /*  isWorking = false;
          streamer.stop();*/
        break;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }
    // Release the video capture object
    vid_capture.release();

    return;

  } while (false);

}




void RTSP_Server::run()
{

  rtspsvrThread = std::thread(&RTSP_Server::startRtspServer, this);
  rtspsvrThread.detach();


}

void RTSP_Server::startRtspServer()
{
  std::string suffix = "opt";
  std::string ip = "127.0.0.1";
  std::string port = "1554";
  std::string rtsp_url = "rtsp://" + ip + ":" + port + "/" + suffix;

  std::shared_ptr<xop::EventLoop> event_loop(new xop::EventLoop());
  std::shared_ptr<xop::RtspServer> server = xop::RtspServer::Create(event_loop.get());

  if (!server->Start("0.0.0.0", atoi(port.c_str()))) {
    printf("RTSP Server listen on %s failed.\n", port.c_str());
    return;
  }

#ifdef AUTH_CONFIG
  server->SetAuthConfig("-_-", "admin", "12345");
#endif

  xop::MediaSession* session = xop::MediaSession::CreateNew(suffix);
  session->AddSource(xop::channel_0, xop::JPEGSource::CreateNew());
  //session->StartMulticast(); 
  session->AddNotifyConnectedCallback([](xop::MediaSessionId sessionId, std::string peer_ip, uint16_t peer_port) {
    printf("RTSP client connect, ip=%s, port=%hu \n", peer_ip.c_str(), peer_port);
    });

  session->AddNotifyDisconnectedCallback([](xop::MediaSessionId sessionId, std::string peer_ip, uint16_t peer_port) {
    printf("RTSP client disconnect, ip=%s, port=%hu \n", peer_ip.c_str(), peer_port);
    });

  xop::MediaSessionId session_id = server->AddSession(session);

  this->rtspSendFrameThread = std::thread(&RTSP_Server::sendFrameThread, this, server.get(), session_id);
  this->rtspSendFrameThread.detach();


  while (1) {
    xop::Timer::Sleep(100);
  }


}

void RTSP_Server::sendFrameThread(xop::RtspServer* rtsp_server, xop::MediaSessionId session_id)
{
  while (1)
  {
    xop::AVFrame videoFrame = { 0 };
    videoFrame.type = 0;
    videoFrame.timestamp = xop::JPEGSource::GetTimestamp();

    {
      std::lock_guard<std::mutex> guard(mutex);
      int frame_size = 0;//feed.size();
      if (frame_size > 1000) {
        videoFrame.size = frame_size;

        videoFrame.buffer.reset(new uint8_t[videoFrame.size]);
        //memcpy(videoFrame.buffer.get(), feed.c_str(), videoFrame.size);
      }

      rtsp_server->PushFrame(session_id, xop::channel_0, videoFrame);
    }
    xop::Timer::Sleep(20); /* 实际使用需要根据帧率计算延时! */

  }

}


