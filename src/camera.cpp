#include <librealsense2/rs.hpp>
#include <iostream>

int main(){
	std::cout << "Hello World!" << std::endl;
	rs2::pipeline p;
	p.start();

	rs2::frameset frames = p.wait_for_frames();
	rs2::video_frame color = frames.get_color_frame();

	int width = color.get_width();
	int height = color.get_height();

	std::cout << "width: " << width << std::endl;
	std::cout << "height: " << height << std::endl;
	return 0;

}
