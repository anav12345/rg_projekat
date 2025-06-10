//
// Created by ana on 6/10/25.
//

#include <engine/graphics/PointShadow.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <spdlog/spdlog.h>

namespace engine::graphics {

float PointShadow::near_plane = 1.0f;
float PointShadow::far_plane = 25.0f;

unsigned int PointShadow::SHADOW_WIDTH = 1024;
unsigned int PointShadow::SHADOW_HEIGHT = 1024;

unsigned int PointShadow::create_fbo() {
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    return depthMapFBO;
}

unsigned int PointShadow::create_cubeMap() {
    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);
    return depthCubemap;
}

void PointShadow::initialize(unsigned int fbo, unsigned int cubemap) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) spdlog::info("PointShadow FBO not complete!");
}

std::vector<glm::mat4> PointShadow::transformation_matrices(glm::vec3 light_position) {
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float) SHADOW_WIDTH / (float) SHADOW_HEIGHT, near_plane, far_plane);
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light_position, light_position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

    return shadowTransforms;
}

void PointShadow::render_depth_buffer(unsigned int fbo, unsigned int cubemap, glm::vec3 light_position, std::vector<glm::mat4> shadowTransforms, resources::Shader *shader) {
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    shader->use();
    for (unsigned int i = 0; i < 6; ++i) shader->set_mat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
    shader->set_float("far_plane", far_plane);
    shader->set_vec3("lightPos", light_position);


    //ovo ispod moram u main
    //renderScene(simpleDepthShader);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PointShadow::unbind_fbo() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PointShadow::after_render_depth_buffer(int scr_width, int scr_height) {

    unbind_fbo();
    glViewport(0, 0, scr_width, scr_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PointShadow::clear_buffers() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PointShadow::activate_depth_map(unsigned int cubemap) {
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
}
}
