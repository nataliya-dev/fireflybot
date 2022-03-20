#include "Camera.h"

#include <iostream>

namespace fireflybot {
bool Camera::initialize() {
  std::cout << "Initializing Camera" << std::endl;
  rs2::config config;
  set_config(config);
  pipeline_.start(config);

  return true;
}

void Camera::set_config(rs2::config& config) {
  std::cout << "Setting camera config" << std::endl;
  config.disable_all_streams();  // save processing power
  auto color_stream = RS2_STREAM_COLOR;
  int width = 640;
  int height = 480;
  auto format = RS2_FORMAT_BGR8;
  int framerate = 0;  // 0 = any
  config.enable_stream(color_stream, width, height, format, framerate);
}

cv::Mat Camera::convert_to_opencv(const rs2::video_frame& color_frame) {
  int width = color_frame.get_width();
  int height = color_frame.get_height();
  cv::Mat cv_img(cv::Size(width, height), CV_8UC3,
                 (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
  return cv_img;
}

void Camera::save_image(const cv::Mat& cv_img) {
  bool check = cv::imwrite("img.jpg", cv_img);
  if (check == false) {
    std::cout << "FAILED to save image" << std::endl;
  }
}

void Camera::wait_for_flash() {
  std::cout << "Waiting to detect firefly flash" << std::endl;

  rs2::frameset frames = pipeline_.wait_for_frames();
  rs2::video_frame color_frame = frames.get_color_frame();

  cv::Mat cv_img = convert_to_opencv(color_frame);
  save_image(cv_img);

  // do some image processing here

  return;
}

}  // namespace fireflybot