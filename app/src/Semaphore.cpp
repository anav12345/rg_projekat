//
// Created by ana on 5/29/25.
//

#include <Semaphore.hpp>
#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformController.hpp>
#include <spdlog/spdlog.h>

namespace app {
void Semaphore::initialize_lights() {
    red_light.position = glm::vec3(4.5, 4.7, -3.5);
    red_light.ambient = glm::vec3(0.4, 0.4, 0.2);
    red_light.diffuse = glm::vec3(2.0, 0.0, 0.0);
    red_light.specular = glm::vec3(0.5, 0.5, 0.5);
    red_light.constant = 1.0f;
    red_light.linear = 0.09f;
    red_light.quadratic = 0.09f;

    // on startup yellow is turned off
    yellow_light.position = glm::vec3(4.5, 4.4, -3.5);
    yellow_light.ambient = glm::vec3(0.0, 0.0, 0.0);
    yellow_light.diffuse = glm::vec3(0.0, 0.0, 0.0);
    yellow_light.specular = glm::vec3(0.0, 0.0, 0.0);
    yellow_light.constant = 1.0f;
    yellow_light.linear = 0.09f;
    yellow_light.quadratic = 0.09f;

    // on startup green is turned off
    green_light.position = glm::vec3(4.5, 4.05, -3.5);
    green_light.ambient = glm::vec3(0.0, 0.0, 0.0);
    green_light.diffuse = glm::vec3(0.0, 0.0, 0.0);
    green_light.specular = glm::vec3(0.0, 0.0, 0.0);
    green_light.constant = 1.0f;
    green_light.linear = 0.09f;
    green_light.quadratic = 0.09f;
}

void Semaphore::on_key_pressed(char key) {
    if (current_state == SemaphoreState::TRANSITIONING_TO_GREEN) {
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

void Semaphore::update() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float dt = platform->dt();

    if (current_state == SemaphoreState::TRANSITIONING_TO_GREEN) {
        transition_timer += dt;

        if (transition_timer >= 1.0f && !yellow_on) {
            turn_on(yellow_light, 1);
            yellow_on = true;
            color_state = 3;
        }

        if (transition_timer >= 3.0f) {
            set_state(SemaphoreState::GREEN);
            spdlog::info("tranzicija gotova, upaljeno je zeleno.....");
        }
    }

    if (current_state == SemaphoreState::BLINKING_YELLOW) {
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_toggle_time).count() > 500) {
            yellow_blink_state = !yellow_blink_state;
            if (yellow_blink_state) {
                turn_on(yellow_light, 1);
                color_state = 1;
            } else {
                turn_off(yellow_light);
                color_state = -1;
            }
            last_toggle_time = now;
        }
    }
}

void Semaphore::turn_on(PointLight &light, int color) {
    // color => 0 - red, 1 - yellow, 2 - green
    light.ambient = glm::vec3(0.4f, 0.4f, 0.2f);
    light.specular = glm::vec3(0.5f, 0.5f, 0.5f);

    if (color == 0) { light.diffuse = glm::vec3(2.0f, 0.0f, 0.0f); } else if (color == 1) { light.diffuse = glm::vec3(2.0f, 2.0f, 0.0f); } else if (color == 2) { light.diffuse = glm::vec3(0.0f, 2.0f, 0.0f); }

}

void Semaphore::turn_off(PointLight &light) {
    light.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
    light.diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
    light.specular = glm::vec3(0.0f, 0.0f, 0.0f);
}

void Semaphore::set_state(SemaphoreState new_state) {
    current_state = new_state;

    turn_off(red_light);
    turn_off(yellow_light);
    turn_off(green_light);

    if (new_state == SemaphoreState::RED) {
        turn_on(red_light, 0);
        color_state = 0;
    } else if (new_state == SemaphoreState::GREEN) {
        turn_on(green_light, 2);
        color_state = 2;
    } else if (new_state == SemaphoreState::BLINKING_YELLOW) {
        turn_on(yellow_light, 1);
        color_state = 1;
        yellow_blink_state = true;
        last_toggle_time = std::chrono::steady_clock::now();
    }
}

void Semaphore::start_transition_to_green() {
    set_state(SemaphoreState::TRANSITIONING_TO_GREEN);
    turn_on(red_light, 0);
    color_state = 0;
    transition_timer = 0.0f;
    yellow_on = false;
    spdlog::info("tranzicija pocinje....");
}

}