//
// Created by ana on 5/13/25.
//

#include <MainController.hpp>

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

    m_red_point_light.position = glm::vec3(4.5, 4.7, -3.5);
    m_red_point_light.ambient = glm::vec3(0.4, 0.4, 0.2);
    m_red_point_light.diffuse = glm::vec3(2.0, 0.0, 0.0);
    m_red_point_light.specular = glm::vec3(0.5, 0.5, 0.5);
    m_red_point_light.constant = 1.0f;
    m_red_point_light.linear = 0.09f;
    m_red_point_light.quadratic = 0.09f;

    // on startup yellow is turned off
    m_yellow_point_light.position = glm::vec3(4.5, 4.4, -3.5);
    m_yellow_point_light.ambient = glm::vec3(0.0, 0.0, 0.0);
    m_yellow_point_light.diffuse = glm::vec3(0.0, 0.0, 0.0);
    m_yellow_point_light.specular = glm::vec3(0.0, 0.0, 0.0);
    m_yellow_point_light.constant = 1.0f;
    m_yellow_point_light.linear = 0.09f;
    m_yellow_point_light.quadratic = 0.09f;

    // on startup green is turned off
    m_green_point_light.position = glm::vec3(4.5, 4.05, -3.5);
    m_green_point_light.ambient = glm::vec3(0.0, 0.0, 0.0);
    m_green_point_light.diffuse = glm::vec3(0.0, 0.0, 0.0);
    m_green_point_light.specular = glm::vec3(0.0, 0.0, 0.0);
    m_green_point_light.constant = 1.0f;
    m_green_point_light.linear = 0.09f;
    m_green_point_light.quadratic = 0.09f;

}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) { return false; }
    return true;

}

void MainController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    if (platform->key(engine::platform::KeyId::KEY_G).state() == engine::platform::Key::State::JustPressed && !transition_started) {
        turn_off(m_green_point_light);
        turn_off(m_yellow_point_light);
        turn_on(m_red_point_light, 0);

        transition_on = true;
        red_on = false;
        blinking_yellow = false;
    }
    if (platform->key(engine::platform::KeyId::KEY_R).state() == engine::platform::Key::State::JustPressed && !transition_started) {
        turn_off(m_green_point_light);
        turn_off(m_yellow_point_light);

        transition_on = false;
        red_on = true;
        blinking_yellow = false;
    }

    if (platform->key(engine::platform::KeyId::KEY_Y).state() == engine::platform::Key::State::JustPressed && !transition_started && !blinking_yellow) {
        turn_off(m_red_point_light);
        turn_off(m_green_point_light);
        turn_on(m_yellow_point_light, 1);
        state = 1;

        transition_on = false;
        red_on = false;
        blinking_yellow = true;
        yellow_on = true;
        last_toggle_time = std::chrono::steady_clock::now();
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

void MainController::update_lights_go() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float dt = platform->dt();

    if (transition_on) {
        if (!transition_started) {
            transition_started = true;
            time_since_transition = 0.0f;
            turn_on(m_red_point_light, 0);
            state = 0;
            //turn_on(yellowPointLight, 1);
            spdlog::info("red on ,wait for 3 second......");
        }

        if (transition_started) {
            time_since_transition += dt;

            if (time_since_transition >= 1.0f) {
                turn_on(m_yellow_point_light, 1);
                state = 3;
            }

            if (time_since_transition >= 3.0f) {
                transition_started = false;
                turn_off(m_red_point_light);
                turn_off(m_yellow_point_light);
                turn_on(m_green_point_light, 2);
                state = 2;
                spdlog::info("3 seconds passed, green on....");
                transition_on = false;
            }
        }
    }
}

void MainController::update_lights_red() {
    if (red_on) {
        turn_on(m_red_point_light, 0);
        state = 0;
    }
}

void MainController::update_blinking_yellow() {
    if (blinking_yellow) {
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_toggle_time).count() > 500) {
            yellow_on = !yellow_on;
            if (yellow_on) {
                turn_on(m_yellow_point_light, 1);
                state = 1;
            } else {
                turn_off(m_yellow_point_light);
                state = -1;
            }
            last_toggle_time = now;
        }
    }
}

void MainController::update() {
    update_camera();
    update_lights_go();
    update_lights_red();
    update_blinking_yellow();
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

    shader->set_vec3("red_point_light.position", m_red_point_light.position);
    shader->set_vec3("red_point_light.ambient", m_red_point_light.ambient);
    shader->set_vec3("red_point_light.diffuse", m_red_point_light.diffuse);
    shader->set_vec3("red_point_light.specular", m_red_point_light.specular);
    shader->set_float("red_point_light.constant", m_red_point_light.constant);
    shader->set_float("red_point_light.linear", m_red_point_light.linear);
    shader->set_float("red_point_light.quadratic", m_red_point_light.quadratic);

    shader->set_vec3("yellow_point_light.position", m_yellow_point_light.position);
    shader->set_vec3("yellow_point_light.ambient", m_yellow_point_light.ambient);
    shader->set_vec3("yellow_point_light.diffuse", m_yellow_point_light.diffuse);
    shader->set_vec3("yellow_point_light.specular", m_yellow_point_light.specular);
    shader->set_float("yellow_point_light.constant", m_yellow_point_light.constant);
    shader->set_float("yellow_point_light.linear", m_yellow_point_light.linear);
    shader->set_float("yellow_point_light.quadratic", m_yellow_point_light.quadratic);

    shader->set_vec3("green_point_light.position", m_green_point_light.position);
    shader->set_vec3("green_point_light.ambient", m_green_point_light.ambient);
    shader->set_vec3("green_point_light.diffuse", m_green_point_light.diffuse);
    shader->set_vec3("green_point_light.specular", m_green_point_light.specular);
    shader->set_float("green_point_light.constant", m_green_point_light.constant);
    shader->set_float("green_point_light.linear", m_green_point_light.linear);
    shader->set_float("green_point_light.quadratic", m_green_point_light.quadratic);


    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
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

    shader->set_vec3("red_point_light.position", m_red_point_light.position);
    shader->set_vec3("red_point_light.ambient", m_red_point_light.ambient);
    shader->set_vec3("red_point_light.diffuse", m_red_point_light.diffuse);
    shader->set_vec3("red_point_light.specular", m_red_point_light.specular);
    shader->set_float("red_point_light.constant", m_red_point_light.constant);
    shader->set_float("red_point_light.linear", m_red_point_light.linear);
    shader->set_float("red_point_light.quadratic", m_red_point_light.quadratic);

    shader->set_vec3("yellow_point_light.position", m_yellow_point_light.position);
    shader->set_vec3("yellow_point_light.ambient", m_yellow_point_light.ambient);
    shader->set_vec3("yellow_point_light.diffuse", m_yellow_point_light.diffuse);
    shader->set_vec3("yellow_point_light.specular", m_yellow_point_light.specular);
    shader->set_float("yellow_point_light.constant", m_yellow_point_light.constant);
    shader->set_float("yellow_point_light.linear", m_yellow_point_light.linear);
    shader->set_float("yellow_point_light.quadratic", m_yellow_point_light.quadratic);

    shader->set_vec3("green_point_light.position", m_green_point_light.position);
    shader->set_vec3("green_point_light.ambient", m_green_point_light.ambient);
    shader->set_vec3("green_point_light.diffuse", m_green_point_light.diffuse);
    shader->set_vec3("green_point_light.specular", m_green_point_light.specular);
    shader->set_float("green_point_light.constant", m_green_point_light.constant);
    shader->set_float("green_point_light.linear", m_green_point_light.linear);
    shader->set_float("green_point_light.quadratic", m_green_point_light.quadratic);

    shader->set_int("state", state);


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

void MainController::begin_draw() { engine::graphics::OpenGL::clear_buffers(); }

void MainController::draw() {
    draw_car();
    draw_traffic_light();
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

void MainController::turn_off(PointLight &light) {
    light.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
    light.diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
    light.specular = glm::vec3(0.0f, 0.0f, 0.0f);
}

void MainController::turn_on(PointLight &light, int color) {
    // color => 0 - red, 1 - yellow, 2 - green
    light.ambient = glm::vec3(0.4f, 0.4f, 0.2f);
    light.specular = glm::vec3(0.5f, 0.5f, 0.5f);

    if (color == 0) { light.diffuse = glm::vec3(2.0f, 0.0f, 0.0f); } else if (color == 1) { light.diffuse = glm::vec3(2.0f, 2.0f, 0.0f); } else if (color == 2) { light.diffuse = glm::vec3(0.0f, 2.0f, 0.0f); }

}
}// app