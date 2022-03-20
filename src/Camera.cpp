#include <iostream>
#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>

void initialize_config(rs2::config& config) {
  config.disable_all_streams();  // save processing power
  auto color_stream = RS2_STREAM_COLOR;
  int width = 640;
  int height = 480;
  auto format = RS2_FORMAT_BGR8;
  int framerate = 0;  // 0 = any
  config.enable_stream(color_stream, width, height, format, framerate);
}

void print_frame_info(const rs2::video_frame& color) {
  std::cout << "Received frame!" << std::endl;
  int width = color.get_width();
  int height = color.get_height();

  std::cout << "width: " << width << std::endl;
  std::cout << "height: " << height << std::endl;
}

void save_image(const rs2::video_frame& color_frame) {
  int width = color_frame.get_width();
  int height = color_frame.get_height();

  cv::Mat cv_img(cv::Size(width, height), CV_8UC3,
                 (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
  bool check = cv::imwrite("img.jpg", cv_img);

  if (check == false) {
    std::cout << "FAILED to save image" << std::endl;
  }
}

int main() {
  std::cout << "Starting fireflybot!" << std::endl;

  rs2::config config;
  initialize_config(config);

  rs2::pipeline pipeline;
  pipeline.start(config);

  rs2::frameset frames = pipeline.wait_for_frames();
  rs2::video_frame color_frame = frames.get_color_frame();
  print_frame_info(color_frame);
  save_image(color_frame);

  return 0;
}
