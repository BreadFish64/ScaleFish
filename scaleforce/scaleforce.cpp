#include "scaleforce.hpp"

#include "alignment.hpp"

#include <fmt/chrono.h>

#include <chrono>

std::vector<std::uint8_t> ScaleForce::Scale(const std::vector<std::uint8_t>& image_data,
                                                   std::uint32_t width, std::uint32_t height,
                                                   std::uint8_t scale_factor) {
    const std::uint32_t scaled_width = width * scale_factor, scaled_height = height * scale_factor;
    OpenGL::Texture read_tex(true), draw_tex(true), offset_tex(true);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, read_tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE,
                    image_data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, draw_tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, scaled_width, scaled_height);
    glBindImageTexture(0, draw_tex, 0, false, 0, GL_WRITE_ONLY, GL_RGBA8);
    
    if (write_offsets) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, offset_tex);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, scaled_width, scaled_height);
        glBindImageTexture(1, offset_tex, 0, false, 0, GL_WRITE_ONLY, GL_RGBA8);
    }

    glUseProgram(program);
    glUniform1i(0, write_offsets);

    OpenGL::Query render_time_query{true};
    glBeginQuery(GL_TIME_ELAPSED, render_time_query);
    glDispatchCompute(CeilDiv(scaled_width, block_size), CeilDiv(scaled_height, block_size), 1);
    glEndQuery(GL_TIME_ELAPSED);

    const std::uint32_t scaled_image_size = image_data.size() * scale_factor * scale_factor;
    std::vector<std::uint8_t> filter_output(scaled_image_size * (1 + write_offsets));

    glActiveTexture(GL_TEXTURE1);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, filter_output.data());

    if (write_offsets) {
        glActiveTexture(GL_TEXTURE2);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, filter_output.data() + scaled_image_size);
    }

    GLuint64 render_time;
    glGetQueryObjectui64v(render_time_query, GL_QUERY_RESULT, &render_time);
    fmt::print(std::cout, "ScaleForce completed {}x{} in {}\n", width, height,
               std::chrono::duration<double, std::milli>{std::chrono::nanoseconds{render_time}});

    return filter_output;
}
