#include <fmt/color.h>
#include <fmt/ostream.h>

#include <glad/glad.h>

#include <lodepng.h>

#include "common/bilinear_upscale.hpp"
#include "most_used_colors.hpp"

int main() {
    OpenGL::InitOffscreenContext();

    std::vector<std::uint8_t> image_data;
    std::uint32_t w, h;
    lodepng::decode(image_data, w, h,
                    "C:\\Users\\Marshall\\OneDrive\\Pictures\\textures\\tex1_128x128_DBD259CACE906D59_13.png");

    auto histogram = std::make_unique<Histogram>();
    GenerateHistogram(*histogram, image_data);
    for (std::size_t layer_idx{0}; layer_idx != histogram->size(); ++layer_idx) {
        lodepng::encode(fmt::format("red_layer_{}.png", layer_idx),
                        reinterpret_cast<std::uint8_t*>((*histogram)[layer_idx].data()),
                        (*histogram)[0][0].size(), (*histogram)[0].size(), LodePNGColorType::LCT_GREY);
    }

    std::vector<FloatColor> peak_colors;
    GetPeakColors(peak_colors, *histogram);
    for (const auto& color : peak_colors) {
        fmt::print(/*fmt::fg(fmt::rgb{color.r(), color.g(), color.b()}),*/
                   "red: {}, green: {}, blue: {}\n", color.r(), color.g(), color.b());
    }

    PushColors(image_data, peak_colors);
    lodepng::encode("result.png", image_data, w, h);

    auto filter_image = BilinearUpscale(image_data, w, h, 2);
    lodepng::encode("bilinear.png", filter_image, w * 2, h * 2);
    PushColors(filter_image, peak_colors);
    lodepng::encode("upscaled_result.png", filter_image, w * 2, h * 2);
    
    return 0;
}
