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

  /**
   * Initialize the operational parameters of the camera.
   *
   * @return bool value is True if initialization is a success, False on
   * initialization failure.
   */
  bool initialize();

  /**
   * A non-blocking call to process a frame and check whether or not the frame
   * contains a firefly flash.
   *
   * @param detect_tm_ms[out] Stores the amount of time it took to process the
   * frame.
   *
   * @return bool True if a flash has been detected.
   */
  bool is_flash_detected(long int& detect_tm_ms);

  /**
   * Processes frames in an open loop and prints out whether a light has been
   * detected. Used for testing and debugging.
   */
  void test_camera();

  /**
   * Set simulation mode for the Camera class. No processing occurs, but the
   * camera does return that a "simulated" flash has been detected. Mainly used
   * for testing and debugging of the Sync class because it does not require a
   * functional hardware camera.
   */
  void set_sim_mode(bool is_sim);

 private:
  /**
   * TUNING PARAMETER: The threshold that is used to understand whether or not a
   * camera frame contains a flash. If we expect the firefly to be very bright
   * and contain many pixels in the frame then this value can be larger. If we
   * run into an issue where the firefly is not detected because it is dim, then
   * this value might need to be reduced.
   */
  const double DETECT_THRESH = 1200;

  /**
   * Number of consecutive flashes detected in consecutive frames.
   */
  int num_light_frames_ = 0;

  /**
   *  current frame
   */
  int _frame = 0;

  /**
   * Detection value of the previous frame
   */ 
  bool previous_frame_ = false;
  
  /**
   * Detection value of the current frame
   */
  bool current_frame_ = false;

  /**
   * Keeps track of the sum of light pixels detected on the previous camera
   * frame.
   */
  double prev_sum_ = 0.0;

  /**
   * Set simulation mode for camera operation. No frames will be processed.
   */
  bool is_sim_ = false;

  /**
   * The rate at which the simulated camera will return that a simulated flash
   * has been detected.
   */
  const int SIM_DETECT_PERIOD_MS = 700;

  /**
   * Whether or not to have a pop-up window which displays the images that the
   * camera is seeing. Note this is only used for debugging and only works when
   * the raspberry pi or the camera is connected to a monitor. Cannot be used
   * with ssh.
   */
  const bool visualize_frames_ = false;

  /**
   * Whether or not to save the latest processed frame in the directory. Also
   * used for debugging.
   */
  const bool save_frames_ = true;

  /**
   * Keeps track of the time that a simulated flash has been reported.
   */
  std::chrono::time_point<std::chrono::high_resolution_clock> sim_detect_tm_ =
      std::chrono::high_resolution_clock::now();

  /**
   * Connection to the internal parameters of the realsense camera.
   */
  rs2::pipeline pipeline_;

  /**
   * Configure the realsense camera according to the given parameters.
   *
   * @param[in] config Settings used to set camera parameters.
   */
  void set_config(rs2::config& config);

  /**
   * Convert a realsense camera frame datatype to an opencv frame datatype.
   *
   * @param[in] color_frame The realsense frame capture by the camera.
   * @return The corresponding opencv frame which will be used for further
   * processing.
   */
  cv::Mat convert_to_opencv(const rs2::video_frame& color_frame);

  /**
   * Save an image to the working memory. Used to test and debug how the frames
   * are beeing processed and what the camera is seeing.
   *
   * @param[in] cv_img The image that will be saved.
   * @param[in] name The name of the saved image file.
   */
  void save_image(const cv::Mat& cv_img, std::string name, int frame);

  /**
   * Attempt to detect a light in the frame based on a threshold. If there are
   * more light pixels in the frame than the threshold then the function returns
   * True.
   *
   * @param[in] img The frame used to detect a flash.
   * @return Whether or not a light has been detected.
   */
  bool is_light_on(const cv::Mat& img);

  /**
   * Check whether a flash detection signal should be sent by the simulated
   * camera.
   *
   * @return Whether or not to send a flash.
   */
  bool is_sim_flash();
};

}  // namespace fireflybot

#endif /* FIREFLYBOT_CAMERA_H */
