//
// Created by ana on 5/13/25.
//

#include <MainController.hpp>
#include <SemaphoreController.hpp>
#include <engine/graphics/Framebuffer.hpp>
#include <engine/graphics/PointShadows.hpp>

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
    m_directional_light.direction = glm::vec3(0.0f, -8.0f, 0.0f);
    m_directional_light.ambient = glm::vec3(0.03f);
    m_directional_light.diffuse = glm::vec3(1.0f, 0.7843f, 0.3921f) * 0.7f;
    m_directional_light.specular = glm::vec3(0.2f, 0.2f, 0.2f) * 0.5f;

    // framebuffer za post-processing
    m_framebuffer.initialize_framebuffer(platform->window()->width(), platform->window()->height());

    // framebuffer za senke
    m_point_shadows.initialize_framebuffer();
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) { return false; }
    return true;

}

void MainController::poll_events() {}

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
}

void MainController::draw_car() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto semaphore = engine::core::Controller::get<SemaphoreController>();

    engine::resources::Model *car = resources->model("car");

    engine::resources::Shader *shader = resources->shader("car");

    PointLight red_light = semaphore->get_red_point_light();
    PointLight yellow_light = semaphore->get_yellow_point_light();
    PointLight green_light = semaphore->get_green_point_light();

    shader->use();

    shader->set_vec3("directional_light.direction", m_directional_light.direction);
    shader->set_vec3("directional_light.ambient", m_directional_light.ambient);
    shader->set_vec3("directional_light.diffuse", m_directional_light.diffuse);
    shader->set_vec3("directional_light.specular", m_directional_light.specular);

    shader->set_float("material_shininess", 64.0f);
    shader->set_vec3("viewPosition", graphics->camera()->Position);

    shader->set_vec3("red_point_light.position", red_light.position);
    shader->set_vec3("red_point_light.ambient", red_light.ambient);
    shader->set_vec3("red_point_light.diffuse", red_light.diffuse);
    shader->set_vec3("red_point_light.specular", red_light.specular);
    shader->set_float("red_point_light.constant", red_light.constant);
    shader->set_float("red_point_light.linear", red_light.linear);
    shader->set_float("red_point_light.quadratic", red_light.quadratic);

    shader->set_vec3("yellow_point_light.position", yellow_light.position);
    shader->set_vec3("yellow_point_light.ambient", yellow_light.ambient);
    shader->set_vec3("yellow_point_light.diffuse", yellow_light.diffuse);
    shader->set_vec3("yellow_point_light.specular", yellow_light.specular);
    shader->set_float("yellow_point_light.constant", yellow_light.constant);
    shader->set_float("yellow_point_light.linear", yellow_light.linear);
    shader->set_float("yellow_point_light.quadratic", yellow_light.quadratic);

    shader->set_vec3("green_point_light.position", green_light.position);
    shader->set_vec3("green_point_light.ambient", green_light.ambient);
    shader->set_vec3("green_point_light.diffuse", green_light.diffuse);
    shader->set_vec3("green_point_light.specular", green_light.specular);
    shader->set_float("green_point_light.constant", green_light.constant);
    shader->set_float("green_point_light.linear", green_light.linear);
    shader->set_float("green_point_light.quadratic", green_light.quadratic);


    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.1f, -5.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    shader->set_mat4("model", model);

    // racunanje senki
    m_point_shadows.activate_cubemap_texture();
    shader->set_int("depthMap", 1);
    shader->set_float("far_plane", m_point_shadows.far_plane);

    car->draw(shader);
}

void MainController::draw_traffic_light() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto semaphore = engine::core::Controller::get<SemaphoreController>();

    engine::resources::Model *traffic_light = resources->model("traffic_light");

    engine::resources::Shader *shader = resources->shader("traffic_light");

    PointLight red_light = semaphore->get_red_point_light();
    PointLight yellow_light = semaphore->get_yellow_point_light();
    PointLight green_light = semaphore->get_green_point_light();
    int color_state = semaphore->get_color_state();

    shader->use();

    shader->set_vec3("directional_light.direction", m_directional_light.direction);
    shader->set_vec3("directional_light.ambient", m_directional_light.ambient);
    shader->set_vec3("directional_light.diffuse", m_directional_light.diffuse);
    shader->set_vec3("directional_light.specular", m_directional_light.specular);

    shader->set_float("material_shininess", 64.0f);
    shader->set_vec3("viewPosition", graphics->camera()->Position);

    shader->set_vec3("red_point_light.position", red_light.position);
    shader->set_vec3("red_point_light.ambient", red_light.ambient);
    shader->set_vec3("red_point_light.diffuse", red_light.diffuse);
    shader->set_vec3("red_point_light.specular", red_light.specular);
    shader->set_float("red_point_light.constant", red_light.constant);
    shader->set_float("red_point_light.linear", red_light.linear);
    shader->set_float("red_point_light.quadratic", red_light.quadratic);

    shader->set_vec3("yellow_point_light.position", yellow_light.position);
    shader->set_vec3("yellow_point_light.ambient", yellow_light.ambient);
    shader->set_vec3("yellow_point_light.diffuse", yellow_light.diffuse);
    shader->set_vec3("yellow_point_light.specular", yellow_light.specular);
    shader->set_float("yellow_point_light.constant", yellow_light.constant);
    shader->set_float("yellow_point_light.linear", yellow_light.linear);
    shader->set_float("yellow_point_light.quadratic", yellow_light.quadratic);

    shader->set_vec3("green_point_light.position", green_light.position);
    shader->set_vec3("green_point_light.ambient", green_light.ambient);
    shader->set_vec3("green_point_light.diffuse", green_light.diffuse);
    shader->set_vec3("green_point_light.specular", green_light.specular);
    shader->set_float("green_point_light.constant", green_light.constant);
    shader->set_float("green_point_light.linear", green_light.linear);
    shader->set_float("green_point_light.quadratic", green_light.quadratic);

    shader->set_int("state", color_state);


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
    auto semaphore = engine::core::Controller::get<SemaphoreController>();

    engine::resources::Model *asphalt = resources->model("asphalt");

    engine::resources::Shader *shader = resources->shader("car");

    PointLight red_light = semaphore->get_red_point_light();
    PointLight yellow_light = semaphore->get_yellow_point_light();
    PointLight green_light = semaphore->get_green_point_light();

    shader->use();

    shader->set_vec3("directional_light.direction", m_directional_light.direction);
    shader->set_vec3("directional_light.ambient", m_directional_light.ambient);
    shader->set_vec3("directional_light.diffuse", m_directional_light.diffuse);
    shader->set_vec3("directional_light.specular", m_directional_light.specular);

    shader->set_float("material_shininess", 32.0f);
    shader->set_vec3("viewPosition", graphics->camera()->Position);

    shader->set_vec3("red_point_light.position", red_light.position);
    shader->set_vec3("red_point_light.ambient", red_light.ambient);
    shader->set_vec3("red_point_light.diffuse", red_light.diffuse);
    shader->set_vec3("red_point_light.specular", red_light.specular);
    shader->set_float("red_point_light.constant", red_light.constant);
    shader->set_float("red_point_light.linear", red_light.linear);
    shader->set_float("red_point_light.quadratic", red_light.quadratic);

    shader->set_vec3("yellow_point_light.position", yellow_light.position);
    shader->set_vec3("yellow_point_light.ambient", yellow_light.ambient);
    shader->set_vec3("yellow_point_light.diffuse", yellow_light.diffuse);
    shader->set_vec3("yellow_point_light.specular", yellow_light.specular);
    shader->set_float("yellow_point_light.constant", yellow_light.constant);
    shader->set_float("yellow_point_light.linear", yellow_light.linear);
    shader->set_float("yellow_point_light.quadratic", yellow_light.quadratic);

    shader->set_vec3("green_point_light.position", green_light.position);
    shader->set_vec3("green_point_light.ambient", green_light.ambient);
    shader->set_vec3("green_point_light.diffuse", green_light.diffuse);
    shader->set_vec3("green_point_light.specular", green_light.specular);
    shader->set_float("green_point_light.constant", green_light.constant);
    shader->set_float("green_point_light.linear", green_light.linear);
    shader->set_float("green_point_light.quadratic", green_light.quadratic);


    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 9.4f, -5.0f));
    model = glm::scale(model, glm::vec3(8.0f));
    shader->set_mat4("model", model);

    // racunanje senki
    m_point_shadows.activate_cubemap_texture();
    shader->set_int("depthMap", 1);
    shader->set_float("far_plane", m_point_shadows.far_plane);

    asphalt->draw(shader);
}

void MainController::shadow_pass() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    engine::resources::Shader *depthShader = resources->shader("depth");

    // kreiram vektor za modele i model matrice
    std::vector<std::pair<engine::resources::Model *, glm::mat4> > models;

    // model auta i njegova model matrica
    engine::resources::Model *car = resources->model("car");
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.1f, -5.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    models.emplace_back(car, model);

    // model asfalta i njegova model matrica
    engine::resources::Model *asphalt = resources->model("asphalt");
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 9.4f, -5.0f));
    model = glm::scale(model, glm::vec3(8.0f));
    models.emplace_back(asphalt, model);

    auto semaphore = engine::core::Controller::get<SemaphoreController>();
    glm::vec3 light_position = semaphore->get_light_pos();
    int scr_width = platform->window()->width();
    int scr_height = platform->window()->height();

    m_point_shadows.shadow_pass(depthShader, light_position, scr_width, scr_height, models);
}

void MainController::before_draw() { m_framebuffer.before_draw(); }

void MainController::after_draw() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    engine::resources::Shader *shader = resources->shader("post_processing");
    m_framebuffer.after_draw(shader);

}

void MainController::begin_draw() {
    shadow_pass();
    before_draw();
}

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