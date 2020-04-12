#pragma once

#include <cstdint>
#include <vector>
#include <set>

#include "common/dimensional_array.hpp"
#include "common/open_gl.hpp"
#include "common/vector_math.hpp"

constexpr std::uint_fast8_t in_color_depth = 8;
constexpr std::uint_fast8_t out_color_depth = 5;
constexpr std::uint_fast8_t color_shift = in_color_depth - out_color_depth;
constexpr std::int32_t values_per_color = 1 << out_color_depth;
constexpr std::int32_t max_color_value = values_per_color - 1;

using Histogram = CubeArray<std::uint16_t, values_per_color>;
using Color = VecBase<std::uint8_t, 3>;
using FloatColor = VecBase<float, 3>;

[[gnu::noinline]] void GenerateHistogram(Histogram& histogram,
                                         const std::vector<std::uint8_t>& image_data) {
    if (image_data.size() % 4 != 0)
        throw std::invalid_argument("Picture data is not 4 bpp");

    for (auto iter = image_data.begin(); iter != image_data.end(); iter += 2) {
        std::size_t red, green, blue;
        red = *(iter) >> color_shift;
        green = *(++iter) >> color_shift;
        blue = *(++iter) >> color_shift;
        ++histogram[red][green][blue];
    }
}

[[gnu::noinline]] void GetPeakColors(std::vector<FloatColor>& peak_data,
                                     const Histogram& histogram) {
    for (std::int32_t red{20}; red != values_per_color; ++red) {
        for (std::int32_t green{20}; green != values_per_color; ++green) {
            for (std::int32_t blue{20}; blue != values_per_color; ++blue) {
                std::array<std::uint32_t, 6> surrounding;
                std::uint32_t count;
                surrounding[0] = histogram[std::max(red - 1, 0)][green][blue];
                surrounding[1] = histogram[red][std::max(green - 1, 0)][blue];
                surrounding[2] = histogram[red][green][std::max(blue - 1, 0)];
                count = histogram[red][green][blue];
                surrounding[3] = histogram[red][green][std::min(blue + 1, max_color_value)];
                surrounding[4] = histogram[red][std::min(green + 1, max_color_value)][blue];
                surrounding[5] = histogram[std::min(red + 1, max_color_value)][green][blue];
                // TODO: handle equal cases better
                if (count > *std::max_element(surrounding.begin(), surrounding.end()))
                    peak_data.emplace_back(static_cast<FloatColor>(VecBase<std::int32_t, 3>{red, green, blue}));
            }
        }
    }
}

[[gnu::noinline]] void PushColors(std::vector<std::uint8_t>& image_data,
                                  std::vector<FloatColor>& peak_data) {
    for (auto iter = image_data.begin(); iter != image_data.end(); iter += 2) {
        FloatColor color;
        color.r() = *(iter) >> color_shift;
        color.g() = *(++iter) >> color_shift;
        color.b() = *(++iter) >> color_shift;
        iter -= 2;
        Color out_color = static_cast<Color>(*std::min_element(
            peak_data.begin(), peak_data.end(), [&color](const FloatColor& a, const FloatColor& b) {
                return color.distance(a) < color.distance(b);
            }));
        *(iter) = out_color.r() << color_shift;
        *(++iter) = out_color.g() << color_shift;
        *(++iter) = out_color.b() << color_shift;
    }
}

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
