#pragma once

#include <array>

#include "common/file_util.hpp"
#include "common/open_gl.hpp"

std::pair<std::vector<std::uint8_t>, std::vector<std::uint8_t>> GradientPull(
    const std::vector<std::uint8_t>& image_data,
                                          std::uint32_t width, std::uint32_t height,
                                          std::uint8_t scale_factor) {
    OpenGL::Framebuffer draw_fbo(true);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_fbo.handle);

    OpenGL::Texture read_tex(true), draw_tex(true), offset_tex(true);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, read_tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE,
                    image_data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, draw_tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width * scale_factor, height * scale_factor);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, draw_tex, 0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, offset_tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width * scale_factor, height * scale_factor);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, offset_tex, 0);

    std::array<GLenum, 2> buffers{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, buffers.data());

    OpenGL::VertexArray vao(true);
    glBindVertexArray(vao);

    OpenGL::Shader vertex_shader{StringFromFile("gradient_pull.vert"), GL_VERTEX_SHADER};
    OpenGL::Shader fragment_shader{StringFromFile("gradient_pull.frag"), GL_FRAGMENT_SHADER};
    OpenGL::Program program{vertex_shader, fragment_shader};
    glUseProgram(program);

    glViewport(0, 0, width * scale_factor, height * scale_factor);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glActiveTexture(GL_TEXTURE1);
    std::vector<std::uint8_t> filter_output(image_data.size() * scale_factor * scale_factor);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, filter_output.data());

    glActiveTexture(GL_TEXTURE2);
    std::vector<std::uint8_t> offset_output(image_data.size() * scale_factor * scale_factor);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, offset_output.data());

    return {filter_output, offset_output};
}
