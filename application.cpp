#include <cmath>
#include <string>
#include <stdexcept>
#include <iostream>
#include <cassert>

#include "tiny_obj_loader.h"

#include "application.hpp"



Application::Application(int initial_width, int initial_height, std::vector<std::string> arguments) : PV112Application(initial_width, initial_height, arguments)
{
    tinyobj::ObjReaderConfig tobj_config;
    tobj_config.mtl_search_path = lecture_folder_path / "objects";
    tobj_config.triangulate = false;

    tinyobj::ObjReader tobj_reader;

    if (!tobj_reader.ParseFromFile(lecture_folder_path / "objects" / "kocka.obj", tobj_config)) {
        if (!tobj_reader.Error().empty()) {
            std::cerr << "tinyobjloader error (ParseFromFile) : " << tobj_reader.Error() << "\n";
        } else {
            std::cerr << "tinyobjloader unknown error (ParseFromFile)\n";
        }

        throw std::runtime_error("tinyobjloader error");
    }

    if (!tobj_reader.Warning().empty()) {
        std::cerr << "tinyobjloader warning : " << tobj_reader.Warning() << "\n";
    }

    // std::vector<tinyobj::shape_t> shapes = tobj_reader.GetShapes();
    // assert(shapes.size() == 1);
    // tinyobj::shape_t shape = shapes[0];
    // assert(shape.lines.indices.size() == 0);
    // assert(shape.points.indices.size() == 0);

    // for (size_t face_index = 0; face_index < shape.mesh.num_face_vertices.size(); face_index++) {
    //     assert(shape.mesh.num_face_vertices[face_index] == 3);

    //     size_t vertex_index = face_index * 3;
    //     tinyobj::index_t index0 = shape.mesh.indices[vertex_index];
    //     tinyobj::index_t index1 = shape.mesh.indices[vertex_index + 1];
    //     tinyobj::index_t index2 = shape.mesh.indices[vertex_index + 2];

    //     std::cout << index0.vertex_index + 1 << "/" << index0.texcoord_index + 1 << "/" << index0.normal_index + 1 << " ";
    //     std::cout << index1.vertex_index + 1 << "/" << index1.texcoord_index + 1 << "/" << index1.normal_index + 1 << " ";
    //     std::cout << index2.vertex_index + 1 << "/" << index2.texcoord_index + 1 << "/" << index2.normal_index + 1 << "\n";
    // }

    tinyobj::attrib_t attribs = tobj_reader.GetAttrib();
    std::vector<tinyobj::shape_t> shapes = tobj_reader.GetShapes();
    assert(shapes.size() == 1);
    tinyobj::shape_t shape = shapes[0];
    assert(shape.lines.indices.size() == 0);
    assert(shape.points.indices.size() == 0);
    tinyobj::mesh_t mesh = shape.mesh;

    shape0_face_count = mesh.num_face_vertices.size();

    std::vector<float> positions;
    positions.reserve(shape0_face_count * 9);
    std::vector<float> normals;
    normals.reserve(shape0_face_count * 9);
    std::vector<float> uvs;
    uvs.reserve(shape0_face_count * 6);

    for (size_t face_index = 0; face_index < shape0_face_count; face_index++) {
        assert(shape.mesh.num_face_vertices[face_index] == 3);

        for (size_t vertex_offset = 0; vertex_offset < 3; vertex_offset++) {
            tinyobj::index_t index = shape.mesh.indices[3 * face_index + vertex_offset];

            positions.push_back(attribs.vertices[3 * index.vertex_index]);
            positions.push_back(attribs.vertices[3 * index.vertex_index + 1]);
            positions.push_back(attribs.vertices[3 * index.vertex_index + 2]);

            normals.push_back(attribs.normals[3 * index.normal_index]);
            normals.push_back(attribs.normals[3 * index.normal_index + 1]);
            normals.push_back(attribs.normals[3 * index.normal_index + 2]);

            uvs.push_back(attribs.texcoords[2 * index.texcoord_index]);
            uvs.push_back(attribs.texcoords[2 * index.texcoord_index + 1]);
        }
    }

    std::cout << positions.size() << "\n";

    program0 = create_program(lecture_shaders_path / "main.vert", lecture_shaders_path / "main.frag");

    glCreateBuffers(1, &shape0_vbo_pos);
    glNamedBufferStorage(shape0_vbo_pos, positions.size() * sizeof(float), positions.data(), GL_NONE);

    glCreateBuffers(1, &shape0_vbo_normals);
    glNamedBufferStorage(shape0_vbo_normals, normals.size() * sizeof(float), normals.data(), GL_NONE);

    glCreateVertexArrays(1, &shape0_vao);
    
    glVertexArrayVertexBuffer(shape0_vao, 0, shape0_vbo_pos, 0, 3 * sizeof(float));
    glEnableVertexArrayAttrib(shape0_vao, 0);
    glVertexArrayAttribFormat(shape0_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(shape0_vao, 0, 0);

    glVertexArrayVertexBuffer(shape0_vao, 1, shape0_vbo_normals, 0, 3 * sizeof(float));
    glEnableVertexArrayAttrib(shape0_vao, 1);
    glVertexArrayAttribFormat(shape0_vao, 1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(shape0_vao, 1, 1);
}

Application::~Application() {}

void Application::update(float delta)
{
    t0 += delta;
    t0 = fmod(t0, 10000.0f);

    float delta_s = delta / 1000.0f;

    bool left_pressed = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
    bool right_pressed = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;

    if (left_pressed && !right_pressed) {
        velx = -15.0f;
    } else if (!left_pressed && right_pressed) {
        velx = 15.0f;
    }

    if (glm::abs(velx) > 0) {
        float sign = glm::sign(velx);
        velx -= 40.0f * delta_s * sign;

        if (glm::sign(velx) * sign <= 0.0f) {
            velx = 0.0f;
        }
    }
    x += velx * delta_s;
}

void Application::render()
{
    glViewport(0, 0, get_width(), get_height());

    // bool pressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    // double mult = pressed ? 10.0 : 0.5;
    // glClearColor(0, (std::sin(glfwGetTime() * mult) + 1) / 2, 0, 1);
    glClearColor(1, 1, 1, 1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(90.0f), static_cast<float>(get_width()) / get_height(), 0.01f, 100.0f);

    glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 8.0f);
    glm::mat4 view = glm::lookAt(camera_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glProgramUniformMatrix4fv(program0, 0, 1, GL_FALSE, glm::value_ptr(projection));
    glProgramUniformMatrix4fv(program0, 1, 1, GL_FALSE, glm::value_ptr(view));

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glUseProgram(program0);

    glm::mat4 shape0_model = glm::translate(glm::vec3(x, -4.0f, 0.0f)) * glm::scale(glm::vec3(1.0f));
    glProgramUniformMatrix4fv(program0, 2, 1, GL_FALSE, glm::value_ptr(shape0_model));

    glBindVertexArray(shape0_vao);
    glDrawArrays(GL_TRIANGLES, 0, shape0_face_count * 3);

    shape0_model = glm::translate(glm::vec3(-x, 2.0f, 0.0f)) * glm::scale(glm::vec3(1.0f));
    glProgramUniformMatrix4fv(program0, 2, 1, GL_FALSE, glm::value_ptr(shape0_model));

    glBindVertexArray(shape0_vao);
    glDrawArrays(GL_TRIANGLES, 0, shape0_face_count * 3);
}

void Application::render_ui() {}

void Application::on_resize(int width, int height)
{
    IApplication::on_resize(width, height);
}

void Application::on_mouse_move(double x, double y) {}

void Application::on_mouse_button(int button, int action, int mods) {}

void Application::on_key_pressed(int key, int scancode, int action, int mods) {}
