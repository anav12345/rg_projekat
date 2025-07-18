//
// Created by ana on 7/18/25.
//

#ifndef SEMAPHORECONTROLLER_HPP
#define SEMAPHORECONTROLLER_HPP
#include <chrono>
#include <engine/core/Controller.hpp>
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

class SemaphoreController : public engine::core::Controller {

    void initialize() override;

    void poll_events() override;

    void update() override;

public:
    std::string_view name() const override { return "app::SemaphoreController"; }

    PointLight get_red_point_light();

    PointLight get_yellow_point_light();

    PointLight get_green_point_light();

    int get_color_state();

    glm::vec3 get_light_pos();

private:
    void on_key_pressed(char key);

    void turn_on(PointLight &light, int color);

    void turn_off(PointLight &light);

    void set_state(SemaphoreState new_state);

    void start_transition_to_green();

    PointLight red_light;
    PointLight yellow_light;
    PointLight green_light;

    // state for shader
    int color_state = 0;

    // active light for shadows
    glm::vec3 light_pos;

    SemaphoreState current_state = SemaphoreState::RED;

    float transition_timer = 0.0f;
    bool yellow_on = false;
    std::chrono::steady_clock::time_point last_toggle_time;

    bool yellow_blink_state = false;

};
}
#endif //SEMAPHORECONTROLLER_HPP
