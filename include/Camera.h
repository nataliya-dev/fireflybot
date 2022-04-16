#ifndef FIREFLYBOT_CAMERA_H
#define FIREFLYBOT_CAMERA_H

#include <chrono>
#include <librealsense2/rs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace fireflybot {
class Camera {
 public:
  Camera(){};
  ~Camera(){};

  bool initialize();
  bool is_flash_detected(long int& detect_tm_ms);

  void test_camera();

  void set_sim_mode(bool is_sim);

 private:
  int num_light_frames_ = 0;
  double prev_sum_ = 0.0;
  const double DETECT_THRESH = 4000;

  bool is_sim_ = false;
  const int SIM_DETECT_PERIOD_MS = 1000;
  std::chrono::time_point<std::chrono::high_resolution_clock> sim_detect_tm_ =
      std::chrono::high_resolution_clock::now();

  rs2::pipeline pipeline_;

  void set_config(rs2::config& config);
  cv::Mat convert_to_opencv(const rs2::video_frame& color_frame);
  void save_image(const cv::Mat& cv_img, std::string name);
  bool detect_blob(const cv::Mat& img);
  bool is_light_on(const cv::Mat& img);
  bool is_sim_flash();
};

}  // namespace fireflybot

#endif /* FIREFLYBOT_CAMERA_H */