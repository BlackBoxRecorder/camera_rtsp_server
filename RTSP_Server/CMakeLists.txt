# CMakeList.txt: RTSP_Server 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/rtsp-server)

find_package(OpenCV REQUIRED)

# 设置 include 目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/rtsp-server
    ${OpenCV_INCLUDE_DIRS}
)

# 将源代码添加到此项目的可执行文件。
add_executable (RTSP_Server "rtsp_server.cpp" "rtsp_server.h" "main.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE
    rtsp-server
    ${OpenCV_LIBRARIES}
)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RTSP_Server PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
