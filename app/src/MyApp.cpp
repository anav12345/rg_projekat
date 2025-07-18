//
// Created by ana on 5/13/25.
//

#include <MyApp.hpp>

#include <MainController.hpp>
#include <SemaphoreController.hpp>
#include <spdlog/spdlog.h>

namespace app {
void MyApp::app_setup() {
    spdlog::info("App setup completed!");
    auto main_controller = register_controller<app::MainController>();
    auto semaphore_controller = register_controller<app::SemaphoreController>();
    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    semaphore_controller->after(main_controller);
}
}// app