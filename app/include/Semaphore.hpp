//
// Created by ana on 5/29/25.
//

#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP
#include <chrono>
#include <glm/vec3.hpp>

struct PointLight {
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

namespace app {

enum class SemaphoreState {
    RED,
    TRANSITIONING_TO_GREEN,
    GREEN,
    BLINKING_YELLOW
};

class Semaphore {

public:
    void initialize_lights();

    void on_key_pressed(char key);

    void update();

    PointLight red_light;
    PointLight yellow_light;
    PointLight green_light;

    // state for shader
    int color_state = 0;

private:
    void turn_on(PointLight &light, int color);

    void turn_off(PointLight &light);

    void set_state(SemaphoreState new_state);

    void start_transition_to_green();

    SemaphoreState current_state = SemaphoreState::RED;

    float transition_timer = 0.0f;
    bool yellow_on = false;
    std::chrono::steady_clock::time_point last_toggle_time;

    bool yellow_blink_state = false;

};
}
#endif //SEMAPHORE_HPP
