#pragma once

#include "common/file_util.hpp"
#include "common/open_gl.hpp"

class ScaleForce {
    OpenGL::Shader compute_shader{StringFromFile("scaleforce.comp"), GL_COMPUTE_SHADER};
    OpenGL::VertexArray vao{true};
    OpenGL::Program program{compute_shader};

    static constexpr std::uint32_t block_size = 8;

public:
    bool write_offsets = false;
    GLenum wrap_mode = GL_CLAMP_TO_EDGE;

    std::vector<std::uint8_t> Scale(const std::vector<std::uint8_t>& image_data,
                                    std::uint32_t width, std::uint32_t height,
                                    std::uint8_t scale_factor);
};
