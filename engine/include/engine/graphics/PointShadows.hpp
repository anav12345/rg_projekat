//
// Created by ana on 6/22/25.
//

#ifndef POINTSHADOWS_HPP
#define POINTSHADOWS_HPP

#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>

namespace engine::graphics {
    class PointShadows {
    public:
        float far_plane = 25.0;

        void activate_cubemap_texture();

        void initialize_framebuffer();

        void shadow_pass(resources::Shader *depth_shader, glm::vec3 light_position, int scr_width, int scr_height,
                         std::vector<std::pair<engine::resources::Model *, glm::mat4> > models);

    private:
        unsigned int m_depth_map_fbo;
        unsigned int m_depth_cubemap;
        float m_near_plane                 = 1.0f;
        const unsigned int m_shadow_width  = 1024;
        const unsigned int m_shadow_height = 1024;

        unsigned int create_framebuffer();

        void bind_framebuffer();

        void unbind_framebuffer();

        unsigned int create_cubemap_texture();

        void bind_cubemap_texture();

        void setup_cubemap();

        std::vector<glm::mat4> create_transformation_matrices(glm::vec3 light_position);

        void setup_shader_and_render(resources::Shader *shader, std::vector<glm::mat4> shadow_transforms,
                                     glm::vec3 light_position,
                                     std::vector<std::pair<engine::resources::Model *, glm::mat4> > models);
    };
}

#endif //POINTSHADOWS_HPP
