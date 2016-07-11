#include <exception>
#include <iostream>
#include <vector>

#include <boost/signals2.hpp>
#include <DepthSense.hxx>

#include "camera.h"
#include "ds-image.h"

namespace ds = DepthSense;

Camera::Camera() : _device_found(false), _frame_count(0)
{
    _context = ds::Context::create("localhost");
    _context.deviceAddedEvent().connect(this, &Camera::_on_device_added);
    _context.deviceRemovedEvent().connect(this, &Camera::_on_device_removed);
    _init();
}

Camera::~Camera()
{
    if (_node.isSet())
        _context.unregisterNode(_node);
}

void Camera::run()
{
   _context.startNodes();
    _context.run();
   _context.stopNodes();
}

void Camera::_init()
{
    std::vector<ds::Device> devices = _context.getDevices();
    std::cout << "found " << devices.size() << " devices\n";
    if (devices.size() > 0) {
        _init_device(devices[0]);
    }
}

void Camera::_init_device(ds::Device device)
{
    std::cout << "init device\n";

    _device = device;

    _device.nodeAddedEvent().connect(this, &Camera::_on_node_added);
    _device.nodeRemovedEvent().connect(this, &Camera::_on_node_removed);

    for (auto node: device.getNodes()) {
        _init_node(node);
    }
}

void Camera::_init_node(ds::Node node)
{
    std::cout << "init node\n";

    if (!_node.isSet() && node.is<ds::DepthNode>()) {
        _init_depth_node(node.as<ds::DepthNode>());
    }
}

void Camera::_init_depth_node(ds::DepthNode node)
{
    std::cout << "init depth node\n";

    _node = node;
    _configure_depth_node(_node);
    _context.registerNode(_node);
    _frame_count = 0;
}

void Camera::_on_device_added(ds::Context context,
                              ds::Context::DeviceAddedData data)
{
    std::cout << "device added\n";

    if (!_device.isSet()) {
        _init_device(data.device);
    }
}

void Camera::_on_device_removed(ds::Context context,
                                ds::Context::DeviceRemovedData data)
{
    std::cout << "device removed\n";
    if (data.device == _device) {
        std::cout << "removed device\n";
        _device.unset();
    }
}

void Camera::_on_node_added(ds::Device device,
                            ds::Device::NodeAddedData data)
{
    std::cout << "node added\n";

    if (_device.isSet() && (device == _device) && !_node.isSet()) {
        _init_node(data.node);
    }
}

void Camera::_on_node_removed(ds::Device device,
                              ds::Device::NodeRemovedData data)
{
    std::cout << "node removed\n";
    if (data.node == _node) {
        std::cout << "removed depth node\n";
        _node.unset();
    }
}

void Camera::_configure_depth_node(ds::DepthNode node)
{
    std::cout << "configure node\n";

    node.newSampleReceivedEvent().connect(this, &Camera::_on_sample_received);

    node.setEnableDepthMap(true);
    
    ds::DepthNode::Configuration config = node.getConfiguration();
    config.frameFormat = ds::FRAME_FORMAT_QVGA;
    config.framerate = 25;
    config.mode = ds::DepthNode::CAMERA_MODE_CLOSE_MODE;
    config.saturation = false;

    try 
    {
        _context.requestControl(node, 0);

        node.setConfiguration(config);
    }
    catch (ds::ArgumentException& e)
    {
        std::cout << "Argument Exception: " << e.what() << "\n";
    }
    catch (ds::UnauthorizedAccessException& e)
    {
        std::cout << "Unauthorized Access Exception: " << e.what() << "\n";
    }
    catch (ds::IOException& e)
    {
        std::cout << "IO Exception: " << e.what() << "\n";
    }
    catch (ds::InvalidOperationException& e)
    {
        std::cout << "Invalid Operation Exception: " << e.what() << "\n";
    }
    catch (ds::ConfigurationException& e)
    {
        std::cout << "Configuration Exception: " << e.what() << "\n";
    }
    catch (ds::StreamingException& e)
    {
        std::cout << "Streaming Exception: " << e.what() << "\n";
    }
    catch (ds::TimeoutException&)
    {
        std::cout << "TimeoutException\n";
    }
}

void Camera::_on_sample_received(ds::DepthNode node,
                                 ds::DepthNode::NewSampleReceivedData data)
{
    _frame_count++;

    int width;
    int height;
    ds::FrameFormat_toResolution(data.captureConfiguration.frameFormat,
                                 &width, &height);
    cv::Mat image = Image::make_depth(width, height, data.depthMap);
    frame_received(image);
}
