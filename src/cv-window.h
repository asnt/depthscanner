#include <mutex>
#include <string>

#include <opencv2/core.hpp>

class CvWindow
{
public:
    CvWindow(const std::string& name);
    ~CvWindow();

    void update_image(cv::Mat image);
    void run();

private:
    std::string _name;
    std::mutex _mutex;
    cv::Mat _image;
};

