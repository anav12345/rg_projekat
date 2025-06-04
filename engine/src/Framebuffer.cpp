//
// Created by ana on 6/3/25.
//

#include <engine/graphics/Framebuffer.hpp>
#include <glad/glad.h>

namespace engine::graphics {

unsigned int Framebuffer::create_framebuffer() {
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    return fbo;
}

void Framebuffer::bind_framebuffer(unsigned int fbo) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

unsigned int Framebuffer::create_texture() {
    unsigned int texture;
    glGenTextures(1, &texture);
    return texture;
}

void Framebuffer::bind_texture(unsigned int texture) {
    glBindTexture(GL_TEXTURE_2D, texture);
}

void Framebuffer::setup_texture_for_framebuffer(int scr_width, int scr_height, unsigned int texture) {
    // postavljam texture kao trenutno aktivnu GL_TEXTURE_2D texturu
    bind_texture(texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, scr_width, scr_height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // za trenutno aktivan framebuffer vezuje teksturu kao color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
}

void Framebuffer::create_renderbuffer(int scr_width, int scr_height) {
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, scr_width, scr_height);

    // za trenutno aktivan framebuffer vezuje renderbuffer kao depth i stencil attachment
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
}

void Framebuffer::unbind_framebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int Framebuffer::create_quad() {
    float quadVertices[] = {
            // positions   // texCoords
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
    return quadVAO;
}

void Framebuffer::draw_quad(unsigned int quadVAO, unsigned int texture) {
    glBindVertexArray(quadVAO);
    //glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Framebuffer::before_scene_draw(unsigned int fbo) {
    // preusmeravam crtanje u moj framebuffer umesto podrazumevanog
    bind_framebuffer(fbo);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Framebuffer::after_scene_draw() {
    // vracam na podrazumevani framebuffer kako bi se slika prikazala na ekranu
    unbind_framebuffer();
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Framebuffer::activate_texture(unsigned int texture) {
    // moram da aktiviram napravljenu teksturu pre nego sto nacrtam quad
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
}
}
