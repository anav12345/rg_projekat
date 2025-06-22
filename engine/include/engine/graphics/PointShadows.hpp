//
// Created by ana on 6/22/25.
//

#ifndef POINTSHADOWS_HPP
#define POINTSHADOWS_HPP

#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>

namespace engine::graphics {
    class PointShadows {
    private:
        static const unsigned int SHADOW_WIDTH;
        static const unsigned int SHADOW_HEIGHT;

        static unsigned int create_framebuffer();

        static void bind_framebuffer(unsigned int depthMapFBO);

        static void unbind_framebuffer();

        static unsigned int create_cubemap_texture();

        static void bind_cubemap_texture(unsigned int depthCubemap);

        static void setup_cubemap(unsigned int depthMapFBO, unsigned int depthCubemap);

        static std::vector<glm::mat4> create_transformation_matrices(glm::vec3 light_position, float near_plane,
                                                                     float far_plane);

        static void setup_shader_and_render(resources::Shader *shader, std::vector<glm::mat4> shadowTransforms,
                                            glm::vec3 light_position,
                                            float far_plane,
                                            std::vector<std::pair<engine::resources::Model *, glm::mat4> > models);

    public:
        static void activate_cubemap_texture(unsigned int depthCubemap);

        static void initialize_framebuffer(unsigned int &depthMapFBO, unsigned int &depthCubemap);

        static void shadow_pass(unsigned int depthMapFBO, unsigned int depthCubemap, resources::Shader *depth_shader,
                                glm::vec3 light_position, float near_plane, float far_plane, int scr_width,
                                int scr_height, std::vector<std::pair<engine::resources::Model *, glm::mat4> > models);
    };
}

#endif //POINTSHADOWS_HPP
