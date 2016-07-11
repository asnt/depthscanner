#include <algorithm>
#include <type_traits>

#include <DepthSense.hxx>
#include <opencv2/core.hpp>

#include "ds-image.h"

namespace ds = DepthSense;

cv::Mat Image::make_depth(int width, int height, const Image::DepthMap& depth)
{
    if (depth == nullptr)
        return cv::Mat();

    const int16_t* vertices = &depth[0];

    cv::Mat image(height, width, CV_32FC1);

    int i = 0;
    std::for_each(vertices, vertices + depth.size(),
            [&i, &image](int16_t value) {
                image.at<float>(i) = (float)value;
                i++;
            });

    return image;
}
