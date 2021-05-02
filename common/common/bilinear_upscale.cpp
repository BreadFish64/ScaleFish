#include "bilinear_upscale.hpp"

#include "open_gl.hpp"

std::vector<std::uint8_t> BilinearUpscale(const std::vector<std::uint8_t>& image_data,
                                          std::uint32_t width, std::uint32_t height,
                                          std::uint8_t scale_factor) {
    OpenGL::Framebuffer read_fbo(true), draw_fbo(true);
    OpenGL::Texture read_tex(true), draw_tex(true);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, read_fbo.handle);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_fbo.handle);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, read_tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE,
                    image_data.data());
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, read_tex, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, draw_tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width * scale_factor, height * scale_factor);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, draw_tex, 0);

    glBlitFramebuffer(0, 0, width, height, 0, 0, width * scale_factor, height * scale_factor,
                      GL_COLOR_BUFFER_BIT, GL_LINEAR);

    std::vector<std::uint8_t> filter_output(image_data.size() * scale_factor * scale_factor);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, filter_output.data());
    return filter_output;
}
