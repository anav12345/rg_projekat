//
// Created by ana on 6/3/25.
//

#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

namespace engine::graphics {
    class Framebuffer {
    public:
        static unsigned int create_framebuffer();

        static void bind_framebuffer(unsigned int fbo);

        static unsigned int create_texture();

        static void bind_texture(unsigned int texture);

        static void setup_texture_for_framebuffer(int scr_width, int scr_height, unsigned int texture);

        static void create_renderbuffer(int scr_width, int scr_height);

        static void unbind_framebuffer();

        static unsigned int create_quad();

        static void draw_quad(unsigned int quadVAO, unsigned int texture);

        static void before_scene_draw(unsigned int fbo);

        static void after_scene_draw();

        static void activate_texture(unsigned int texture);
    };
}
#endif //FRAMEBUFFER_HPP
