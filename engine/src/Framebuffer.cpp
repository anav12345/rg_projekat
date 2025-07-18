//
// Created by ana on 6/3/25.
//

#include <engine/graphics/Framebuffer.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <glad/glad.h>

namespace engine::graphics {

void Framebuffer::initialize_framebuffer(int scr_width, int scr_height) {
    fbo = create_framebuffer();
    bind_framebuffer();

    texture = create_texture();
    bind_texture();
    setup_texture_for_framebuffer(scr_width, scr_height);

    create_renderbuffer(scr_width, scr_height);

    unbind_framebuffer();

    quadVAO = create_quad();
}

void Framebuffer::before_draw() { redirect_to_my_framebuffer(); }

void Framebuffer::after_draw(resources::Shader *shader) {
    redirect_to_default_framebuffer();

    shader->use();
    activate_texture();
    shader->set_int("screenTexture", 0);

    draw_quad(quadVAO);
}

unsigned int Framebuffer::create_framebuffer() {
    unsigned int fbo;
    CHECKED_GL_CALL(glGenFramebuffers, 1, &fbo);
    return fbo;
}

void Framebuffer::bind_framebuffer() { CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, fbo); }

unsigned int Framebuffer::create_texture() {
    unsigned int texture;
    CHECKED_GL_CALL(glGenTextures, 1, &texture);
    return texture;
}

void Framebuffer::bind_texture() { CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, texture); }

void Framebuffer::setup_texture_for_framebuffer(int scr_width, int scr_height) {
    // postavljam texture kao trenutno aktivnu GL_TEXTURE_2D texturu
    bind_texture();

    CHECKED_GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGB16F, scr_width, scr_height, 0, GL_RGB, GL_FLOAT, nullptr);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    CHECKED_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // za trenutno aktivan framebuffer vezuje teksturu kao color attachment
    CHECKED_GL_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
}

void Framebuffer::create_renderbuffer(int scr_width, int scr_height) {
    unsigned int rbo;
    CHECKED_GL_CALL(glGenRenderbuffers, 1, &rbo);

    CHECKED_GL_CALL(glBindRenderbuffer, GL_RENDERBUFFER, rbo);
    CHECKED_GL_CALL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, scr_width, scr_height);

    // za trenutno aktivan framebuffer vezuje renderbuffer kao depth i stencil attachment
    CHECKED_GL_CALL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
}

void Framebuffer::unbind_framebuffer() { CHECKED_GL_CALL(glBindFramebuffer, GL_FRAMEBUFFER, 0); }

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
    CHECKED_GL_CALL(glGenVertexArrays, 1, &quadVAO);
    CHECKED_GL_CALL(glGenBuffers, 1, &quadVBO);
    CHECKED_GL_CALL(glBindVertexArray, quadVAO);
    CHECKED_GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, quadVBO);
    CHECKED_GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    CHECKED_GL_CALL(glEnableVertexAttribArray, 0);
    CHECKED_GL_CALL(glVertexAttribPointer, 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    CHECKED_GL_CALL(glEnableVertexAttribArray, 1);
    CHECKED_GL_CALL(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
    return quadVAO;
}

void Framebuffer::draw_quad(unsigned int quadVAO) {
    CHECKED_GL_CALL(glBindVertexArray, quadVAO);
    //glBindTexture(GL_TEXTURE_2D, texture);
    CHECKED_GL_CALL(glDrawArrays, GL_TRIANGLES, 0, 6);
    CHECKED_GL_CALL(glBindVertexArray, 0);
}

void Framebuffer::redirect_to_my_framebuffer() {
    // preusmeravam crtanje u moj framebuffer umesto podrazumevanog
    bind_framebuffer();
    CHECKED_GL_CALL(glEnable, GL_DEPTH_TEST);
    CHECKED_GL_CALL(glClearColor, 0.1f, 0.1f, 0.1f, 1.0f);
    CHECKED_GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Framebuffer::redirect_to_default_framebuffer() {
    // vracam na podrazumevani framebuffer kako bi se slika prikazala na ekranu
    unbind_framebuffer();
    CHECKED_GL_CALL(glDisable, GL_DEPTH_TEST);
    CHECKED_GL_CALL(glClearColor, 1.0f, 1.0f, 1.0f, 1.0f);
    CHECKED_GL_CALL(glClear, GL_COLOR_BUFFER_BIT);
}

void Framebuffer::activate_texture() {
    // moram da aktiviram napravljenu teksturu pre nego sto nacrtam quad
    CHECKED_GL_CALL(glActiveTexture, GL_TEXTURE0);
    CHECKED_GL_CALL(glBindTexture, GL_TEXTURE_2D, texture);
}

}
