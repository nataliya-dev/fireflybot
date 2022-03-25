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

void Camera::save_image(const cv::Mat& cv_img, std::string name) {
  name = name + ".jpg";
  bool check = cv::imwrite(name, cv_img);
  if (check == false) {
    std::cout << "FAILED to save image" << std::endl;
  }
}

bool Camera::detect_blob(const cv::Mat& img) {
  // cv::Mat bwImg;
  // cv::cvtColor(img, bwImg, cv::CV_BGR2GRAY);
  // save_image(bwImg, "bwImg");

  // Create a structuring element (SE)
  int morph_size = 1;
  cv::Mat element = cv::getStructuringElement(
      cv::MORPH_RECT, cv::Size(2 * morph_size + 1, 2 * morph_size + 1),
      cv::Point(morph_size, morph_size));
  // For Erosion
  cv::Mat morphedImg;
  cv::erode(img, morphedImg, element, cv::Point(-1, -1), 1);
  save_image(morphedImg, "morphedImg");

  // cv::Mat blurredImg;
  // cv::blur(morphedImg, blurredImg, cv::Size(8, 8), cv::Point(-1, -1));
  // save_image(blurredImg, "blurredImg");

  // cv::Mat bwImg2;
  // cv::cvtColor(morphedImg, bwImg2, CV_BGR2GRAY);
  // save_image(bwImg2, "bwImg2");

  // cv::Mat threshImg;
  // cv::adaptiveThreshold(bwImg2, threshImg, 200, cv::ADAPTIVE_THRESH_MEAN_C,
  //                       cv::THRESH_BINARY, 3, 2);
  // save_image(threshImg, "threshImg");

  // https://learnopencv.com/blob-detection-using-opencv-python-c/
  // Setup SimpleBlobDetector parameters.
  cv::SimpleBlobDetector::Params params;

  // Change thresholds
  params.minThreshold = 0;
  params.maxThreshold = 0;

  // Filter by Area.
  params.filterByArea = false;
  params.minArea = 500;

  // Filter by Circularity
  params.filterByCircularity = false;
  params.minCircularity = 0.3;

  // Filter by Convexity
  params.filterByConvexity = false;
  params.minConvexity = 0.87;

  // Set up detector with params
  cv::Ptr<cv::SimpleBlobDetector> detector =
      cv::SimpleBlobDetector::create(params);

  // detect blobs
  std::vector<cv::KeyPoint> keypoints;
  detector->detect(morphedImg, keypoints);

  // Draw detected blobs as red circles.
  // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle
  // corresponds to the size of blob
  cv::Mat im_with_keypoints;
  cv::drawKeypoints(morphedImg, keypoints, im_with_keypoints,
                    cv::Scalar(0, 0, 255),
                    cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

  // cv::imshow("keypoints", im_with_keypoints);
  // cv::waitKey(0);
  save_image(img, "raw");
  save_image(im_with_keypoints, "keypoints");

  if (keypoints.size() == 0) {
    return false;
  }

  return true;
}

bool Camera::is_flash_detected() {
  std::cout << "Checking for firefly flash" << std::endl;

  rs2::frameset frames = pipeline_.wait_for_frames();
  rs2::video_frame color_frame = frames.get_color_frame();
  cv::Mat cv_img = convert_to_opencv(color_frame);
  return detect_blob(cv_img);
}

}  // namespace fireflybot