#ifndef DEPTHSCANNER_CAMERA_H_
#define DEPTHSCANNER_CAMERA_H_

#include <boost/signals2.hpp>
#include <DepthSense.hxx>
#include <opencv2/core.hpp>

namespace ds = DepthSense;

class Camera
{
public:

    Camera();
    ~Camera();

    void run();

    boost::signals2::signal<void (cv::Mat)> frame_received;

private:

    void _init();
    void _init_device(ds::Device device);
    void _init_node(ds::Node node);
    void _init_depth_node(ds::DepthNode node);
    void _configure_depth_node(ds::DepthNode node);

    void _on_device_added(ds::Context context,
                          ds::Context::DeviceAddedData data);
    void _on_device_removed(ds::Context context,
                            ds::Context::DeviceRemovedData data);
    void _on_node_added(ds::Device device,
                        ds::Device::NodeAddedData data);
    void _on_node_removed(ds::Device device,
                          ds::Device::NodeRemovedData data);
    void _on_sample_received(ds::DepthNode node,
                             ds::DepthNode::NewSampleReceivedData data);

    ds::Context _context;
    ds::Device _device;
    ds::DepthNode _node;
    bool _device_found;
    int _frame_count;
};

#endif
