#include <mutex>
#include <string>

#include <opencv2/core.hpp>

class GlWindow
{
public:
    GlWindow(const std::string& name, int width, int height);
    ~GlWindow();

    void update_image(cv::Mat image);
    void draw();
    void run();
    void handle_mouse(int event, int x, int y, int flags);
    void rotate(int dx, int dy);
    void pan(int dx, int dy);
    void zoom(int dz);
    void reset_view();

    static void on_draw(void* param);
    static void on_mouse(int event, int x, int y, int flags, void* param);

    GlWindow* instance;

private:
    std::string _name;
    std::mutex _mutex;
    cv::Mat _image;
    int _width;
    int _height;
    int _last_pos[2];
    float _pos[3];
    float _rot[3];
};

