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
class Semaphore {
private:
    void turn_on(PointLight &light, int color);

    void turn_off(PointLight &light);

public:
    void initialize_lights();

    void transition_from_red_to_green();

    void set_to_red();

    void set_to_blinking_yellow();

    PointLight red_light;
    PointLight yellow_light;
    PointLight green_light;

    bool transition_started = false;
    bool transition_on = false;
    float time_since_transition = 0.0f;

    bool red_on = true;

    bool blinking_yellow = false;
    bool yellow_on = false;
    std::chrono::steady_clock::time_point last_toggle_time;

    int state = 0;

};
}
#endif //SEMAPHORE_HPP
