//
// Created by ana on 6/10/25.
//

#ifndef POINTSHADOW_HPP
#define POINTSHADOW_HPP

#include <glm/fwd.hpp>
#include <vector>
#include <engine/resources/Shader.hpp>

namespace engine::graphics {
    class PointShadow {
    public:
        static float near_plane;
        static float far_plane;

        static unsigned int SHADOW_WIDTH;
        static unsigned int SHADOW_HEIGHT;

        static unsigned int create_fbo();

        static unsigned int create_cubeMap();

        static void initialize(unsigned int fbo, unsigned int cubemap);

        static std::vector<glm::mat4> transformation_matrices(glm::vec3 light_position);

        static void render_depth_buffer(unsigned int fbo, unsigned int cubemap, glm::vec3 light_position,
                                        std::vector<glm::mat4> shadowTransforms, resources::Shader *shader);

        // posle gore funkcije moram iz MainController da renderujem sve -> ucitam shader, ucitam modele, postavim model matrice
        // skaliram+transliran+rotiram ispravnim redosledom + onda ide draw sa shaderom + unbind_fbo

        static void unbind_fbo();

        static void after_render_depth_buffer(int scr_width, int scr_height);

        // ide pre prvog renderovanja -> mozda nije potrebno???
        static void clear_buffers();

        static void activate_depth_map(unsigned int cubemap);
    };
}
#endif //POINTSHADOW_HPP
