//
// Created by ana on 5/13/25.
//

#include <MainController.hpp>
#include <engine/graphics/Framebuffer.hpp>

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <spdlog/spdlog.h>

namespace app {

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    camera->rotate_camera(position.dx, position.dy);
}

void MainController::initialize() {
    spdlog::info("MainController initialized!");
    engine::graphics::OpenGL::enable_depth_testing();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    camera->Position = glm::vec3(0.0f, 3.0f, 0.0f);
    platform->set_enable_cursor(false);

    // dirLight
    m_directional_light.direction = glm::vec3(0.0f, -8.0f, -5.0f);
    m_directional_light.ambient = glm::vec3(0.05f);
    m_directional_light.diffuse = glm::vec3(1.0f, 0.7843f, 0.3921f) * 0.5f;
    m_directional_light.specular = glm::vec3(0.2f, 0.2f, 0.2f) * 0.5f;

    // pointLights
    m_semaphore.initialize_lights();

    // framebuffer
    initialize_framebuffer();

}

void MainController::initialize_framebuffer() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    m_fbo = engine::graphics::Framebuffer::create_framebuffer();
    engine::graphics::Framebuffer::bind_framebuffer(m_fbo);
    m_texture = engine::graphics::Framebuffer::create_texture();
    engine::graphics::Framebuffer::bind_texture(m_texture);
    engine::graphics::Framebuffer::setup_texture_for_framebuffer(platform->window()->width(), platform->window()->height(), m_texture);
    engine::graphics::Framebuffer::create_renderbuffer(platform->window()->width(), platform->window()->height());
    engine::graphics::Framebuffer::unbind_framebuffer();

    m_quadVAO = engine::graphics::Framebuffer::create_quad();
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) { return false; }
    return true;

}

void MainController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    if (platform->key(engine::platform::KeyId::KEY_G).state() == engine::platform::Key::State::JustPressed && !m_semaphore.transition_started) {
        m_semaphore.turn_off(m_semaphore.green_light);
        m_semaphore.turn_off(m_semaphore.yellow_light);
        m_semaphore.turn_on(m_semaphore.red_light, 0);

        m_semaphore.transition_on = true;
        m_semaphore.red_on = false;
        m_semaphore.blinking_yellow = false;
    }
    if (platform->key(engine::platform::KeyId::KEY_R).state() == engine::platform::Key::State::JustPressed && !m_semaphore.transition_started) {
        m_semaphore.turn_off(m_semaphore.green_light);
        m_semaphore.turn_off(m_semaphore.yellow_light);

        m_semaphore.transition_on = false;
        m_semaphore.red_on = true;
        m_semaphore.blinking_yellow = false;
    }

    if (platform->key(engine::platform::KeyId::KEY_Y).state() == engine::platform::Key::State::JustPressed && !m_semaphore.transition_started && !m_semaphore.blinking_yellow) {
        m_semaphore.turn_off(m_semaphore.red_light);
        m_semaphore.turn_off(m_semaphore.green_light);

        m_semaphore.turn_on(m_semaphore.yellow_light, 1);
        m_semaphore.state = 1;

        m_semaphore.transition_on = false;
        m_semaphore.red_on = false;
        m_semaphore.blinking_yellow = true;
        m_semaphore.yellow_on = true;
        m_semaphore.last_toggle_time = std::chrono::steady_clock::now();
    }
}

void MainController::update_camera() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    float dt = platform->dt();

    if (platform->key(engine::platform::KeyId::KEY_W).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt); }
    if (platform->key(engine::platform::KeyId::KEY_S).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt); }
    if (platform->key(engine::platform::KeyId::KEY_A).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt); }
    if (platform->key(engine::platform::KeyId::KEY_D).is_down()) { camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt); }

}

void MainController::update() {
    update_camera();

    m_semaphore.transition_from_red_to_green();
    m_semaphore.set_to_red();
    m_semaphore.set_to_blinking_yellow();

}

void MainController::draw_car() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *car = resources->model("car");

    engine::resources::Shader *shader = resources->shader("car");

    shader->use();

    shader->set_vec3("directional_light.direction", m_directional_light.direction);
    shader->set_vec3("directional_light.ambient", m_directional_light.ambient);
    shader->set_vec3("directional_light.diffuse", m_directional_light.diffuse);
    shader->set_vec3("directional_light.specular", m_directional_light.specular);

    shader->set_float("material_shininess", 64.0f);
    shader->set_vec3("viewPosition", graphics->camera()->Position);

    shader->set_vec3("red_point_light.position", m_semaphore.red_light.position);
    shader->set_vec3("red_point_light.ambient", m_semaphore.red_light.ambient);
    shader->set_vec3("red_point_light.diffuse", m_semaphore.red_light.diffuse);
    shader->set_vec3("red_point_light.specular", m_semaphore.red_light.specular);
    shader->set_float("red_point_light.constant", m_semaphore.red_light.constant);
    shader->set_float("red_point_light.linear", m_semaphore.red_light.linear);
    shader->set_float("red_point_light.quadratic", m_semaphore.red_light.quadratic);

    shader->set_vec3("yellow_point_light.position", m_semaphore.yellow_light.position);
    shader->set_vec3("yellow_point_light.ambient", m_semaphore.yellow_light.ambient);
    shader->set_vec3("yellow_point_light.diffuse", m_semaphore.yellow_light.diffuse);
    shader->set_vec3("yellow_point_light.specular", m_semaphore.yellow_light.specular);
    shader->set_float("yellow_point_light.constant", m_semaphore.yellow_light.constant);
    shader->set_float("yellow_point_light.linear", m_semaphore.yellow_light.linear);
    shader->set_float("yellow_point_light.quadratic", m_semaphore.yellow_light.quadratic);

    shader->set_vec3("green_point_light.position", m_semaphore.green_light.position);
    shader->set_vec3("green_point_light.ambient", m_semaphore.green_light.ambient);
    shader->set_vec3("green_point_light.diffuse", m_semaphore.green_light.diffuse);
    shader->set_vec3("green_point_light.specular", m_semaphore.green_light.specular);
    shader->set_float("green_point_light.constant", m_semaphore.green_light.constant);
    shader->set_float("green_point_light.linear", m_semaphore.green_light.linear);
    shader->set_float("green_point_light.quadratic", m_semaphore.green_light.quadratic);


    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.1f, -5.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    shader->set_mat4("model", model);

    car->draw(shader);
}

void MainController::draw_traffic_light() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *traffic_light = resources->model("traffic_light");

    engine::resources::Shader *shader = resources->shader("traffic_light");

    shader->use();

    shader->set_vec3("directional_light.direction", m_directional_light.direction);
    shader->set_vec3("directional_light.ambient", m_directional_light.ambient);
    shader->set_vec3("directional_light.diffuse", m_directional_light.diffuse);
    shader->set_vec3("directional_light.specular", m_directional_light.specular);

    shader->set_float("material_shininess", 64.0f);
    shader->set_vec3("viewPosition", graphics->camera()->Position);

    shader->set_vec3("red_point_light.position", m_semaphore.red_light.position);
    shader->set_vec3("red_point_light.ambient", m_semaphore.red_light.ambient);
    shader->set_vec3("red_point_light.diffuse", m_semaphore.red_light.diffuse);
    shader->set_vec3("red_point_light.specular", m_semaphore.red_light.specular);
    shader->set_float("red_point_light.constant", m_semaphore.red_light.constant);
    shader->set_float("red_point_light.linear", m_semaphore.red_light.linear);
    shader->set_float("red_point_light.quadratic", m_semaphore.red_light.quadratic);

    shader->set_vec3("yellow_point_light.position", m_semaphore.yellow_light.position);
    shader->set_vec3("yellow_point_light.ambient", m_semaphore.yellow_light.ambient);
    shader->set_vec3("yellow_point_light.diffuse", m_semaphore.yellow_light.diffuse);
    shader->set_vec3("yellow_point_light.specular", m_semaphore.yellow_light.specular);
    shader->set_float("yellow_point_light.constant", m_semaphore.yellow_light.constant);
    shader->set_float("yellow_point_light.linear", m_semaphore.yellow_light.linear);
    shader->set_float("yellow_point_light.quadratic", m_semaphore.yellow_light.quadratic);

    shader->set_vec3("green_point_light.position", m_semaphore.green_light.position);
    shader->set_vec3("green_point_light.ambient", m_semaphore.green_light.ambient);
    shader->set_vec3("green_point_light.diffuse", m_semaphore.green_light.diffuse);
    shader->set_vec3("green_point_light.specular", m_semaphore.green_light.specular);
    shader->set_float("green_point_light.constant", m_semaphore.green_light.constant);
    shader->set_float("green_point_light.linear", m_semaphore.green_light.linear);
    shader->set_float("green_point_light.quadratic", m_semaphore.green_light.quadratic);

    shader->set_int("state", m_semaphore.state);


    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(4.5f, 4.1f, -3.5f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.017f));
    shader->set_mat4("model", model);

    traffic_light->draw(shader);
}

void MainController::draw_asphalt() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Model *asphalt = resources->model("asphalt");

    engine::resources::Shader *shader = resources->shader("car");

    shader->use();

    shader->set_vec3("directional_light.direction", m_directional_light.direction);
    shader->set_vec3("directional_light.ambient", m_directional_light.ambient);
    shader->set_vec3("directional_light.diffuse", m_directional_light.diffuse);
    shader->set_vec3("directional_light.specular", m_directional_light.specular);

    shader->set_float("material_shininess", 32.0f);
    shader->set_vec3("viewPosition", graphics->camera()->Position);

    shader->set_vec3("red_point_light.position", m_semaphore.red_light.position);
    shader->set_vec3("red_point_light.ambient", m_semaphore.red_light.ambient);
    shader->set_vec3("red_point_light.diffuse", m_semaphore.red_light.diffuse);
    shader->set_vec3("red_point_light.specular", m_semaphore.red_light.specular);
    shader->set_float("red_point_light.constant", m_semaphore.red_light.constant);
    shader->set_float("red_point_light.linear", m_semaphore.red_light.linear);
    shader->set_float("red_point_light.quadratic", m_semaphore.red_light.quadratic);

    shader->set_vec3("yellow_point_light.position", m_semaphore.yellow_light.position);
    shader->set_vec3("yellow_point_light.ambient", m_semaphore.yellow_light.ambient);
    shader->set_vec3("yellow_point_light.diffuse", m_semaphore.yellow_light.diffuse);
    shader->set_vec3("yellow_point_light.specular", m_semaphore.yellow_light.specular);
    shader->set_float("yellow_point_light.constant", m_semaphore.yellow_light.constant);
    shader->set_float("yellow_point_light.linear", m_semaphore.yellow_light.linear);
    shader->set_float("yellow_point_light.quadratic", m_semaphore.yellow_light.quadratic);

    shader->set_vec3("green_point_light.position", m_semaphore.green_light.position);
    shader->set_vec3("green_point_light.ambient", m_semaphore.green_light.ambient);
    shader->set_vec3("green_point_light.diffuse", m_semaphore.green_light.diffuse);
    shader->set_vec3("green_point_light.specular", m_semaphore.green_light.specular);
    shader->set_float("green_point_light.constant", m_semaphore.green_light.constant);
    shader->set_float("green_point_light.linear", m_semaphore.green_light.linear);
    shader->set_float("green_point_light.quadratic", m_semaphore.green_light.quadratic);


    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 9.4f, -5.0f));
    model = glm::scale(model, glm::vec3(8.0f));
    shader->set_mat4("model", model);

    asphalt->draw(shader);
}

void MainController::after_draw() {
    engine::graphics::Framebuffer::after_scene_draw();

    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    engine::resources::Shader *shader = resources->shader("post_processing");
    shader->use();
    engine::graphics::Framebuffer::activate_texture(m_texture);
    shader->set_int("screenTexture", 0);
    engine::graphics::Framebuffer::draw_quad(m_quadVAO, m_texture);
}

void MainController::begin_draw() { engine::graphics::Framebuffer::before_scene_draw(m_fbo); }

void MainController::draw() {
    draw_car();
    draw_traffic_light();
    draw_asphalt();

    after_draw();
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

}// app