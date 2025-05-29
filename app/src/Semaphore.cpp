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

void Semaphore::transition_from_red_to_green() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float dt = platform->dt();

    if (transition_on) {
        if (!transition_started) {
            transition_started = true;
            time_since_transition = 0.0f;
            turn_on(red_light, 0);
            state = 0;
            //turn_on(yellowPointLight, 1);
            spdlog::info("red on ,wait for 3 second......");
        }

        if (transition_started) {
            time_since_transition += dt;

            if (time_since_transition >= 1.0f) {
                turn_on(yellow_light, 1);
                state = 3;
            }

            if (time_since_transition >= 3.0f) {
                transition_started = false;
                turn_off(red_light);
                turn_off(yellow_light);
                turn_on(green_light, 2);
                state = 2;
                spdlog::info("3 seconds passed, green on....");
                transition_on = false;
            }
        }
    }
}

void Semaphore::set_to_red() {
    if (red_on) {
        turn_on(red_light, 0);
        state = 0;
    }
}

void Semaphore::set_to_blinking_yellow() {
    if (blinking_yellow) {
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_toggle_time).count() > 500) {
            yellow_on = !yellow_on;
            if (yellow_on) {
                turn_on(yellow_light, 1);
                state = 1;
            } else {
                turn_off(yellow_light);
                state = -1;
            }
            last_toggle_time = now;
        }
    }
}
}