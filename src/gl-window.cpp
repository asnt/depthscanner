#include <cassert>
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include <GL/gl.h>
#include <GL/glu.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "gl-window.h"

GlWindow::GlWindow(const std::string& name, int width, int height)
    : instance(this),
      _name(name),
      _width(width),
      _height(height),
      _last_pos{0, 0}
{
    reset_view();

    cv::namedWindow(_name, cv::WINDOW_OPENGL);
    cv::setOpenGlDrawCallback(_name, &GlWindow::on_draw, this);
    cv::setMouseCallback(_name, &GlWindow::on_mouse, this);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)_width / (float)_height, 0.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

GlWindow::~GlWindow()
{
}

void GlWindow::on_draw(void* param)
{
    GlWindow* window = (GlWindow*)param;
    window->draw();
}

void GlWindow::on_mouse(int event, int x, int y, int flags, void* param)
{
    GlWindow* window = (GlWindow*)param;
    window->handle_mouse(event, x, y, flags);
}

void GlWindow::update_image(cv::Mat image)
{
    _mutex.lock();
    _image = image;
    _mutex.unlock();
}

void GlWindow::draw()
{
    _mutex.lock();
    cv::Mat image = _image.clone();
    _mutex.unlock();
    if (image.empty())
        return;

    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(_pos[0], _pos[1], _pos[2]);
    glRotatef(_rot[0], 1.0, 0.0, 0.0);
    glRotatef(_rot[1], 0.0, 1.0, 0.0);
    glRotatef(_rot[2], 0.0, 0.0, 1.0);

    glBegin(GL_POINTS);
    for (int r = 0; r < image.rows; r++) {
        float* row = image.ptr<float>(r);
        for (int c = 0; c < image.cols; c++) {
            if (row[c] > 10000)
                continue;
            float z = row[c] / 1000;
            float col = (z < 1.0) ? z : 1.0;
            float x = 1.0 - (float)c / image.cols - 0.5;
            float y = 1.0 - (float)r / image.rows - 0.5;
            glColor3f(col, col, 1.0);
            glVertex3f(x, y, z);
        }
    }
    glEnd();
}

void GlWindow::handle_mouse(int event, int x, int y, int flags)
{
    int dx = x - _last_pos[0];
    int dy = y - _last_pos[1];
    bool left_down = flags & cv::EVENT_FLAG_LBUTTON;
    bool right_down = flags & cv::EVENT_FLAG_RBUTTON;
    bool middle_down = flags & cv::EVENT_FLAG_MBUTTON;
    switch (event) {
        case cv::EVENT_MOUSEMOVE:
            if (left_down) {
                rotate(dx, dy);
            } else if (right_down) {
                pan(dx, dy);
            } else if (middle_down) {
                zoom(dy);
            }
            break;
        case cv::EVENT_LBUTTONDBLCLK:
            reset_view();
            break;
        default:
            break;
    }

    _last_pos[0] = x;
    _last_pos[1] = y;
}

void GlWindow::rotate(int dx, int dy)
{
    _rot[0] += dy;
    _rot[1] += dx;
}

void GlWindow::pan(int dx, int dy)
{
    _pos[0] += dx / (_width / 2.0);
    _pos[1] -= dy / (_height / 2.0);
}

void GlWindow::zoom(int dz)
{
    _pos[2] -= dz / (_height / 2.0);
}

void GlWindow::reset_view()
{
    _pos[0] = 0.0;
    _pos[1] = 0.0;
    _pos[2] = -1.0;
    _rot[0] = 0.0;
    _rot[1] = 180.0;
    _rot[2] = 0.0;
}

void GlWindow::run()
{
    while (true) {
        cv::updateWindow(_name);
        cv::waitKey(1);
    }
}
