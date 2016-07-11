#ifndef DEPTHSCANNER_DS_IMAGE_H_
#define DEPTHSCANNER_DS_IMAGE_H_

#include <DepthSense.hxx>
#include <opencv2/core.hpp>

namespace ds = DepthSense;

class Image
{
public:


    typedef ds::Pointer<int16_t> DepthMap;

    static cv::Mat make_depth(int width, int height,
                              const Image::DepthMap& depth);
};

#endif
