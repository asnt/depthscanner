#include <thread>

#include <opencv2/core.hpp>

#include "camera.h"
#include "gl-window.h"

int main()
{
    GlWindow gl_window("depthgl", 320, 240);

    Camera camera;
    camera.frame_received.connect(
            [&](cv::Mat image){ gl_window.update_image(image); });

    std::thread gl_window_thread(&GlWindow::run, &gl_window);
    std::thread camera_thread(&Camera::run, &camera);
    gl_window_thread.join();
    camera_thread.join();

    return 0;
}
