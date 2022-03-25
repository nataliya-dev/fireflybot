#ifndef FIREFLYBOT_CAMERA_H  // include guard
#define FIREFLYBOT_CAMERA_H

#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>

namespace fireflybot {
class Camera {
 public:
  Camera(){};
  ~Camera(){};

  bool initialize();
  bool is_flash_detected();

 private:
  rs2::pipeline pipeline_;

  void set_config(rs2::config& config);
  cv::Mat convert_to_opencv(const rs2::video_frame& color_frame);
  void save_image(const cv::Mat& cv_img, std::string name);
  bool detect_blob(const cv::Mat& img);
};

}  // namespace fireflybot

#endif /* FIREFLYBOT_CAMERA_H */