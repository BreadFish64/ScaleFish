#pragma once

#include <array>
#include <chrono>

#include <fmt/chrono.h>

#include "common/file_util.hpp"
#include "common/open_gl.hpp"

class ScaleForce {
    OpenGL::Framebuffer draw_fbo{true};
    OpenGL::Shader vertex_shader{StringFromFile("scale_force.vert"), GL_VERTEX_SHADER};
    OpenGL::Shader fragment_shader{StringFromFile("scale_force.frag"), GL_FRAGMENT_SHADER};
    OpenGL::VertexArray vao{true};
    OpenGL::Program program{vertex_shader, fragment_shader};

public:
    std::vector<std::uint8_t> Scale(
        const std::vector<std::uint8_t>& image_data, std::uint32_t width, std::uint32_t height,
        std::uint8_t scale_factor) {
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
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, draw_tex,
                               0);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, offset_tex);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width * scale_factor, height * scale_factor);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, offset_tex,
                               0);

        std::array<GLenum, 2> buffers{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, buffers.data());
        glBindVertexArray(vao);
        glUseProgram(program);
        glViewport(0, 0, width * scale_factor, height * scale_factor);

        OpenGL::Query render_time_query{true};
        glBeginQuery(GL_TIME_ELAPSED, render_time_query);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glEndQuery(GL_TIME_ELAPSED);

        glActiveTexture(GL_TEXTURE1);
        std::vector<std::uint8_t> filter_output(image_data.size() * scale_factor * scale_factor);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, filter_output.data());

        GLuint64 render_time;
        glGetQueryObjectui64v(render_time_query, GL_QUERY_RESULT, &render_time);
        fmt::print(std::cout, "ScaleForce completed {}x{} in {}",
                   width, height, std::chrono::duration<double, std::milli>{std::chrono::nanoseconds{render_time}});

        return filter_output;
    }
};
