//
// Created by ana on 7/18/25.
//

#include <SemaphoreController.hpp>
#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformController.hpp>
#include <spdlog/spdlog.h>

namespace app {

void SemaphoreController::initialize() {
    spdlog::info("SemaphoreController initialized!");

    m_red_light.position = glm::vec3(4.5, 4.7, -3.5);
    m_red_light.ambient = glm::vec3(0.4, 0.4, 0.2);
    m_red_light.diffuse = glm::vec3(2.0, 0.0, 0.0);
    m_red_light.specular = glm::vec3(0.5, 0.5, 0.5);
    m_red_light.constant = 1.0f;
    m_red_light.linear = 0.09f;
    m_red_light.quadratic = 0.09f;

    // on startup yellow is turned off
    m_yellow_light.position = glm::vec3(4.5, 4.4, -3.5);
    m_yellow_light.ambient = glm::vec3(0.0, 0.0, 0.0);
    m_yellow_light.diffuse = glm::vec3(0.0, 0.0, 0.0);
    m_yellow_light.specular = glm::vec3(0.0, 0.0, 0.0);
    m_yellow_light.constant = 1.0f;
    m_yellow_light.linear = 0.09f;
    m_yellow_light.quadratic = 0.09f;

    // on startup green is turned off
    m_green_light.position = glm::vec3(4.5, 4.05, -3.5);
    m_green_light.ambient = glm::vec3(0.0, 0.0, 0.0);
    m_green_light.diffuse = glm::vec3(0.0, 0.0, 0.0);
    m_green_light.specular = glm::vec3(0.0, 0.0, 0.0);
    m_green_light.constant = 1.0f;
    m_green_light.linear = 0.09f;
    m_green_light.quadratic = 0.09f;

    m_light_pos = glm::vec3(4.5, 4.7, -3.5);
}

void SemaphoreController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    if (platform->key(engine::platform::KeyId::KEY_G).state() == engine::platform::Key::State::JustPressed) { on_key_pressed('G'); }

    if (platform->key(engine::platform::KeyId::KEY_R).state() == engine::platform::Key::State::JustPressed) { on_key_pressed('R'); }

    if (platform->key(engine::platform::KeyId::KEY_Y).state() == engine::platform::Key::State::JustPressed) { on_key_pressed('Y'); }
}

void SemaphoreController::update() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float dt = platform->dt();

    if (m_current_state == SemaphoreState::TRANSITIONING_TO_GREEN) {
        m_transition_timer += dt;

        if (m_transition_timer >= 1.0f && !m_yellow_on) {
            turn_on(m_yellow_light, 1);
            m_yellow_on = true;
            m_color_state = 3;
        }

        if (m_transition_timer >= 3.0f) {
            set_state(SemaphoreState::GREEN);
            spdlog::info("tranzicija gotova, upaljeno je zeleno.....");
        }
    }

    if (m_current_state == SemaphoreState::BLINKING_YELLOW) {
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_toggle_time).count() > 500) {
            m_yellow_blink_state = !m_yellow_blink_state;
            if (m_yellow_blink_state) {
                turn_on(m_yellow_light, 1);
                m_color_state = 1;
            } else {
                turn_off(m_yellow_light);
                m_color_state = -1;
            }
            m_last_toggle_time = now;
        }
    }
}

PointLight SemaphoreController::get_red_point_light() { return m_red_light; }

PointLight SemaphoreController::get_yellow_point_light() { return m_yellow_light; }

PointLight SemaphoreController::get_green_point_light() { return m_green_light; }

int SemaphoreController::get_color_state() { return m_color_state; }

glm::vec3 SemaphoreController::get_light_pos() { return m_light_pos; }

void SemaphoreController::on_key_pressed(char key) {
    if (m_current_state == SemaphoreState::TRANSITIONING_TO_GREEN) {
        spdlog::info("tranzicija u toku, taster ignorisan....");
        return;
    }

    switch (key) {
        case 'R': set_state(SemaphoreState::RED);
            break;
        case 'G': start_transition_to_green();
            break;
        case 'Y': set_state(SemaphoreState::BLINKING_YELLOW);
            break;
    }
}

void SemaphoreController::turn_on(PointLight &light, int color) {
    // color => 0 - red, 1 - yellow, 2 - green
    light.ambient = glm::vec3(0.4f, 0.4f, 0.2f);
    light.specular = glm::vec3(0.5f, 0.5f, 0.5f);

    if (color == 0) { light.diffuse = glm::vec3(2.0f, 0.0f, 0.0f); } else if (color == 1) { light.diffuse = glm::vec3(2.0f, 2.0f, 0.0f); } else if (color == 2) { light.diffuse = glm::vec3(0.0f, 2.0f, 0.0f); }

}

void SemaphoreController::turn_off(PointLight &light) {
    light.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
    light.diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
    light.specular = glm::vec3(0.0f, 0.0f, 0.0f);
}

void SemaphoreController::set_state(SemaphoreState new_state) {
    m_current_state = new_state;

    turn_off(m_red_light);
    turn_off(m_yellow_light);
    turn_off(m_green_light);

    if (new_state == SemaphoreState::RED) {
        turn_on(m_red_light, 0);
        m_color_state = 0;
    } else if (new_state == SemaphoreState::GREEN) {
        turn_on(m_green_light, 2);
        m_color_state = 2;
    } else if (new_state == SemaphoreState::BLINKING_YELLOW) {
        turn_on(m_yellow_light, 1);
        m_color_state = 1;
        m_yellow_blink_state = true;
        m_last_toggle_time = std::chrono::steady_clock::now();
    }
}

void SemaphoreController::start_transition_to_green() {
    set_state(SemaphoreState::TRANSITIONING_TO_GREEN);
    turn_on(m_red_light, 0);
    m_color_state = 0;
    m_transition_timer = 0.0f;
    m_yellow_on = false;
    spdlog::info("tranzicija pocinje....");
}

}
