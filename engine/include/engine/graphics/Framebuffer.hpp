//
// Created by ana on 6/3/25.
//

#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP
#include <engine/resources/Shader.hpp>

namespace engine::graphics {
    class Framebuffer {
    public:
        void initialize_framebuffer(int scr_width, int scr_height);

        void before_draw();

        void after_draw(resources::Shader *shader);

    private:
        unsigned int fbo;
        unsigned int texture;
        unsigned int quadVAO;

        unsigned int create_framebuffer();

        void bind_framebuffer();

        unsigned int create_texture();

        void bind_texture();

        void setup_texture_for_framebuffer(int scr_width, int scr_height);

        void create_renderbuffer(int scr_width, int scr_height);

        void unbind_framebuffer();

        unsigned int create_quad();

        void draw_quad(unsigned int quadVAO);

        void redirect_to_my_framebuffer();

        void redirect_to_default_framebuffer();

        void activate_texture();
    };
}
#endif //FRAMEBUFFER_HPP
