#include <iostream>
#include "rtsp_server.h"


int main()
{
  std::cout << "Hello CMake." << std::endl;


  RTSP_Server svr;
  svr.run();


  return 0;
}