cmake_minimum_required(VERSION 3.10)

project(fireflybot VERSION 1.0)

find_package(realsense2 REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(include)

add_executable(fireflybot src/main.cpp src/Camera.cpp src/Blink.cpp src/Sync.cpp)

find_library(WIRINGPI_LIBRARIES NAMES wiringPi)

target_link_libraries(${PROJECT_NAME} ${realsense2_LIBRARY} ${OpenCV_LIBS} ${WIRINGPI_LIBRARIES})