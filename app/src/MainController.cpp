//
// Created by ana on 5/13/25.
//

#include <MainController.hpp>
#include <engine/graphics/Framebuffer.hpp>
#include <engine/graphics/PointShadow.hpp>

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
    engine::graphics::Framebuffer::initialize_framebuffer(m_fbo, m_texture, m_quadVAO, platform->window()->width(), platform->window()->height());

    // senke
    m_depth_fbo = engine::graphics::PointShadow::create_fbo();
    m_depth_cubemap = engine::graphics::PointShadow::create_cubeMap();
    engine::graphics::PointShadow::initialize(m_depth_fbo, m_depth_cubemap);

}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) { return false; }
    return true;

}

void MainController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    if (platform->key(engine::platform::KeyId::KEY_G).state() == engine::platform::Key::State::JustPressed) { m_semaphore.on_key_pressed('G'); }

    if (platform->key(engine::platform::KeyId::KEY_R).state() == engine::platform::Key::State::JustPressed) { m_semaphore.on_key_pressed('R'); }

    if (platform->key(engine::platform::KeyId::KEY_Y).state() == engine::platform::Key::State::JustPressed) { m_semaphore.on_key_pressed('Y'); }

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

    m_semaphore.update();
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

    shader->set_int("state", m_semaphore.color_state);


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
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    engine::resources::Shader *shader = resources->shader("post_processing");
    engine::graphics::Framebuffer::after_draw(m_texture, m_quadVAO, shader);

}

void MainController::begin_draw() {
    engine::graphics::PointShadow::clear_buffers();
    // neka samo sveylo ide iz crvenog za pocetak
    shadowTransforms = engine::graphics::PointShadow::transformation_matrices(m_semaphore.red_light.position);
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    engine::resources::Shader *depth_shader = resources->shader("depth_shader");
    engine::graphics::PointShadow::render_depth_buffer(m_depth_fbo, m_depth_cubemap, m_semaphore.red_light.position, shadowTransforms, depth_shader);

    // ovde ide renderovanje
    // prvo modeli
    engine::resources::Model *car = resources->model("car");
    engine::resources::Model *traffic_light = resources->model("traffic_light");
    engine::resources::Model *asphalt = resources->model("asphalt");

    // model matrica za auto
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.1f, -5.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    depth_shader->set_mat4("model", model);
    car->draw(depth_shader);

    // model matirca za semafor
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(4.5f, 4.1f, -3.5f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.017f));
    depth_shader->set_mat4("model", model);
    traffic_light->draw(depth_shader);

    // model matrica za asfalt
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 9.4f, -5.0f));
    model = glm::scale(model, glm::vec3(8.0f));
    depth_shader->set_mat4("model", model);
    asphalt->draw(depth_shader);


    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    engine::graphics::PointShadow::after_render_depth_buffer(platform->window()->width(), platform->window()->height());
    // posle ide standardno crtanje sa racunanjem senki


    // framebuffer ide posle
    engine::graphics::Framebuffer::before_draw(m_fbo);
}

void MainController::draw_car_shadows() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *car = resources->model("car");

    engine::resources::Shader *shader = resources->shader("car_shadows");

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

    // idu uniform za senke
    shader->set_vec3("lightPos", m_semaphore.red_light.position);
    shader->set_float("far_plane", engine::graphics::PointShadow::far_plane);

    engine::graphics::PointShadow::activate_depth_map(m_depth_cubemap);
    shader->set_int("depthMap", 3);

    car->draw(shader);
}

void MainController::draw_traffic_light_shadows() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *traffic_light = resources->model("traffic_light");

    engine::resources::Shader *shader = resources->shader("traffic_light_shadows");

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

    shader->set_int("state", m_semaphore.color_state);


    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(4.5f, 4.1f, -3.5f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.017f));
    shader->set_mat4("model", model);

    shader->set_vec3("lightPos", m_semaphore.red_light.position);
    shader->set_float("far_plane", engine::graphics::PointShadow::far_plane);

    engine::graphics::PointShadow::activate_depth_map(m_depth_cubemap);
    shader->set_int("depthMap", 3);

    traffic_light->draw(shader);
}

void MainController::draw_asphalt_shadows() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Model *asphalt = resources->model("asphalt");

    engine::resources::Shader *shader = resources->shader("car_shadows");

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

    shader->set_vec3("lightPos", m_semaphore.red_light.position);
    shader->set_float("far_plane", engine::graphics::PointShadow::far_plane);

    engine::graphics::PointShadow::activate_depth_map(m_depth_cubemap);
    shader->set_int("depthMap", 3);

    asphalt->draw(shader);
}

void MainController::draw() {

    draw_car_shadows();
    draw_traffic_light_shadows();
    draw_asphalt_shadows();

    //        draw_car();
    //    draw_traffic_light();
    //    draw_asphalt();

    after_draw();
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

}// app