//
// Created by ana on 5/13/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <Semaphore.hpp>
#include <engine/core/Controller.hpp>
#include <glm/vec3.hpp>

namespace app {

struct DirLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight {
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

class MainController : public engine::core::Controller {
    void initialize() override;

    bool loop() override;

    void poll_events() override;

    void update_camera();

    void update() override;

    void draw_car();

    void draw_traffic_light();

    void draw_asphalt();

    void after_draw();

    void shadow_pass();

    void before_draw();

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

public:
    std::string_view name() const override { return "app::MainController"; }

private:
    DirLight m_directional_light;
    Semaphore m_semaphore;

    // za framebuffer
    unsigned int m_fbo;
    unsigned int m_texture;
    unsigned int m_quadVAO;

    // za senke
    unsigned int m_depth_map_fbo;
    unsigned int m_depth_cubemap;
    float m_far_plane = 25.0;
    float m_near_plane = 1.0f;
};

}// app

#endif //MAINCONTROLLER_HPP
