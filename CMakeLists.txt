cmake_minimum_required(VERSION 3.10)

project(fireflybot VERSION 1.0)

find_package(realsense2 REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(fireflybot src/camera.cpp)

target_link_libraries(${PROJECT_NAME} ${realsense2_LIBRARY} ${OpenCV_LIBS})