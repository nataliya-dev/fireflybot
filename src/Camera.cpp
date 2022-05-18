#include "Camera.h"

#include <iostream>

namespace fireflybot {
bool Camera::initialize() {
  std::cout << "Initializing Camera" << std::endl;
  if (is_sim_) {
    return true;
  }

  rs2::config config;
  set_config(config);
  pipeline_.start(config);
  std::cout << "Leaving camera config" << std::endl;

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

void Camera::save_image(const cv::Mat& cv_img, std::string name, int frame) {
  name = name + "_" + std::to_string(frame) + ".jpg" ;
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

  // cv::imshow("img", img);
  // cv::waitKey(0);
  // save_image(img, "raw");
  // save_image(im_with_keypoints, "keypoints");

  if (keypoints.size() == 0) {
    return false;
  }

  return true;
}

bool Camera::is_light_on(const cv::Mat& img) {
  cv::Mat greyMat;
  cv::Mat getThresh;
  cv::Mat th_1;
  cv::Mat th_2;

  // convert to grayscale
  cv::cvtColor(img, greyMat, cv::COLOR_BGR2GRAY);

  // set threshold
  cv::threshold(greyMat, getThresh, 80, 255, 0);

  int rect = 0;
  int k_size = 1;

  // calculate kernel for erosion & dilation
  cv::Mat element =
      cv::getStructuringElement(rect, cv::Size(2 * k_size + 1, 2 * k_size + 1),
                                cv::Point(k_size, k_size));

  // processing
  cv::erode(getThresh, th_1, element);
  cv::dilate(th_1, th_1, element);

  if (img.empty()) {
    std::cout << "image not loaded" << std::endl;
  } else if (visualize_frames_) {
    cv::imshow("processed", th_1);
    cv::imshow("unprocessed", getThresh);
    cv::imshow("raw", img);
    cv::waitKey(0);
  }

  // sum over entire processed matrix, looks like around 35000 when detected
  double current_sum = cv::sum(th_1)[0];

  // setting the threshold around 4000, based on outputting
  // sum over filtered matrix and observing sums generated from flashes

  // std::cout << "current_sum: " << current_sum << std::endl;

  // if two frames in a row detect a light sum larger than threshold
  // then led is detected, else ignore and return false
  bool retval = false;
  frame += 1;
  current_frame_ = current_sum > DETECT_THRESH;
  if (!previous_frame_ && current_frame_) {
    std::cout << "\nFlash detected!" << std::endl;
    retval = true;
  }
  if (retval && save_frames_) {
    save_image(th_1, "th_1", frame);
    save_image(getThresh,"unprocessed", frame);
    save_image(img,"raw", frame);
  }
  previous_frame_ = current_frame_;
  return retval;
}

bool Camera::is_sim_flash() {
  bool is_detected = false;

  auto now_tm = std::chrono::high_resolution_clock::now();
  long int sim_since_flash_detect_ms =
      std::chrono::duration<double, std::milli>(now_tm - sim_detect_tm_)
          .count();
  if (sim_since_flash_detect_ms > SIM_DETECT_PERIOD_MS) {
    std::cout << "\nFlash detected!" << std::endl;
    sim_detect_tm_ = std::chrono::high_resolution_clock::now();
    is_detected = true;
  } else {
    is_detected = false;
  }
  return is_detected;
}

bool Camera::is_flash_detected(long int& detect_tm_ms) {
  auto start_detect_tm = std::chrono::high_resolution_clock::now();
  bool is_detected = false;

  if (is_sim_) {
    is_detected = is_sim_flash();
  } else {
    rs2::frameset frames = pipeline_.wait_for_frames();
    rs2::video_frame color_frame = frames.get_color_frame();
    cv::Mat cv_img = convert_to_opencv(color_frame);
    is_detected = is_light_on(cv_img);
  }

  auto end_detect_tm = std::chrono::high_resolution_clock::now();
  detect_tm_ms =
      std::chrono::duration<double, std::milli>(end_detect_tm - start_detect_tm)
          .count();
  return is_detected;
}

// Just update when a change is detected
void Camera::test_camera() {
  bool detected = false;
  while (true) {
    long int detect_tm_ms = 0;
    bool is_detected = is_flash_detected(detect_tm_ms);
    if (is_detected) {
      std::cout << "is_detected: " << is_detected << std::endl;
      std::cout << "detect_tm_ms: " << detect_tm_ms << std::endl;
    }
  }
}

void Camera::set_sim_mode(bool is_sim) { is_sim_ = is_sim; }

}  // namespace fireflybot
