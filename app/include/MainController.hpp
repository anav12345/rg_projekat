//
// Created by ana on 5/13/25.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <engine/graphics/Framebuffer.hpp>
#include <engine/graphics/PointShadows.hpp>
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

    void shadow_pass();

    void before_draw();

    void after_draw();

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

public:
    std::string_view name() const override { return "app::MainController"; }

private:
    DirLight m_directional_light;

    // framebuffer za post-processing
    engine::graphics::Framebuffer m_framebuffer;

    // za senke
    engine::graphics::PointShadows m_point_shadows;
};

}// app

#endif //MAINCONTROLLER_HPP
