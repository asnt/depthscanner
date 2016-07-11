#include <cassert>
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "cv-window.h"

static
cv::Mat get_channel(cv::Mat image, int channel_index);
cv::Mat get_channel(cv::Mat image, int channel_index)
{
    assert(channel_index < image.channels());

    cv::Mat channel(image.rows, image.cols, image.depth());

    const float* src_data = image.ptr<float>();
    float* dst_data = channel.ptr<float>();
    for (unsigned int i = 0; i < image.total() / image.channels(); i++) {
        dst_data[i] = src_data[channel_index + i * 3];
    }

    return channel;
}

CvWindow::CvWindow(const std::string& name) : _name(name)
{
    cv::namedWindow(_name);
}

CvWindow::~CvWindow()
{
}

void CvWindow::update_image(cv::Mat image)
{
    _mutex.lock();
    _image = image;
    _mutex.unlock();
}

void CvWindow::run()
{
    while (true) {
        _mutex.lock();
        cv::Mat image = _image.clone();
        _mutex.unlock();

        if (image.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        cv::Mat depth;
        if (image.channels() > 1) {
            depth = get_channel(image, image.channels() - 1);
        } else {
            depth = image;
        }

        depth = depth / 1000;

        depth.convertTo(depth, CV_8U, 255);
        cv::Mat depth_color;
        cv::cvtColor(depth, depth_color, CV_GRAY2BGR);
        cv::applyColorMap(depth_color, depth_color, cv::COLORMAP_OCEAN);
        cv::imshow(_name, depth_color);

        cv::waitKey(5);
    }
}
