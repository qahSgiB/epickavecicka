#pragma once

#include "pv112_application.hpp"



class Application : public PV112Application
{
private:
    GLuint shape0_vao;
    GLuint shape0_vbo_pos;
    GLuint shape0_vbo_normals;

    size_t shape0_face_count;

    GLuint program0;

    float t0 = 0.0f;

    float x = 0.0f;
    float velx = 0.0f;

public:
    Application(int initial_width, int initial_height, std::vector<std::string> arguments = {});

    virtual ~Application();

    void update(float delta) override;
    void render() override;
    void render_ui() override;

    void on_resize(int width, int height) override;
    void on_mouse_move(double x, double y) override;
    void on_mouse_button(int button, int action, int mods) override;
    void on_key_pressed(int key, int scancode, int action, int mods) override;
};
