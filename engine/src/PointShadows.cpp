//
// Created by ana on 6/22/25.
//

#include <engine/graphics/PointShadows.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace engine::graphics {

void PointShadows::activate_cubemap_texture() {
    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE1);
    bind_cubemap_texture();
}

void PointShadows::initialize_framebuffer() {
    CHECKED_GL_CALL(glEnable, GL_DEPTH_TEST);

    m_depth_map_fbo = create_framebuffer();
    m_depth_cubemap = create_cubemap_texture();

    setup_cubemap();
}

void PointShadows::shadow_pass(resources::Shader *depth_shader, glm::vec3 light_position, int scr_width, int scr_height, std::vector<std::pair<engine::resources::Model *, glm::mat4> > models) {
    // enable depth test zbog post-processinga koji radi disable
    CHECKED_GL_CALL(glEnable, GL_DEPTH_TEST);
    CHECKED_GL_CALL(glViewport, 0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    bind_framebuffer();
    CHECKED_GL_CALL(glClear, GL_DEPTH_BUFFER_BIT);

    depth_shader->use();

    std::vector<glm::mat4> shadowTransforms = create_transformation_matrices(light_position);

    setup_shader_and_render(depth_shader, shadowTransforms, light_position, models);

    unbind_framebuffer();
    CHECKED_GL_CALL(glViewport, 0, 0, scr_width, scr_height);
    CHECKED_GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

unsigned int PointShadows::create_framebuffer() {
    unsigned int depthMapFBO;
    CHECKED_GL_CALL(glGenFramebuffers, 1, &depthMapFBO);
    return depthMapFBO;
}

void PointShadows::bind_framebuffer() { CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_depth_map_fbo); }

void PointShadows::unbind_framebuffer() { CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0); }

unsigned int PointShadows::create_cubemap_texture() {
    unsigned int depthCubemap;
    CHECKED_GL_CALL(glGenTextures, 1, &depthCubemap);
    return depthCubemap;
}

void PointShadows::bind_cubemap_texture() { CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_CUBE_MAP, m_depth_cubemap); }

void PointShadows::setup_cubemap() {
    bind_cubemap_texture();

    for (unsigned int i = 0; i < 6; ++i) { CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr); }

    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    bind_framebuffer();
    CHECKED_GL_CALL(glFramebufferTexture, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depth_cubemap, 0);
    CHECKED_GL_CALL(glDrawBuffer, GL_NONE);
    CHECKED_GL_CALL(glReadBuffer, GL_NONE);
    unbind_framebuffer();
}

std::vector<glm::mat4> PointShadows::create_transformation_matrices(glm::vec3 light_position) {
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.reserve(6);

    shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

    return shadowTransforms;
}

void PointShadows::setup_shader_and_render(resources::Shader *shader, std::vector<glm::mat4> shadowTransforms, glm::vec3 light_position, std::vector<std::pair<engine::resources::Model *, glm::mat4> > models) {

    for (unsigned int i = 0; i < 6; ++i) { shader->set_mat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]); }
    shader->set_vec3("lightPos", light_position);
    shader->set_float("far_plane", far_plane);


    for (const auto &[model, model_matrix]: models) {
        shader->set_mat4("model", model_matrix);
        model->draw(shader);
    }
}

}