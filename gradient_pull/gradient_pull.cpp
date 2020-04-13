#include <filesystem>
#include <lodepng.h>

#include "common/bilinear_upscale.hpp"
#include "common/open_gl.hpp"
#include "gradient_pull.hpp"

int main(int argc, const char* argv[]) {
    std::vector<std::string_view> args(argc);
    for (std::size_t idx{0}; idx < argc; ++idx)
        args[idx] = argv[idx];
    args.push_back("C:\\Users\\Marshall\\AppData\\Roaming\\Citra\\dump\\textures\\00040000000A0500\\tex1_"
                   "256x256_88081B9B35AEE729_13.png");

    std::vector<std::filesystem::path> paths{
        std::remove_if(args.begin(), args.end(),
                       [](std::string_view x) { return std::filesystem::exists(x); }),
        args.end()};

    OpenGL::InitOffscreenContext();

    for (const auto& path : paths) {
        std::vector<std::filesystem::path> files;
        if (std::filesystem::is_directory(path)) {
            for (const auto& file : std::filesystem::recursive_directory_iterator{path}) {
                if (file.is_regular_file() && file.path().extension() == ".png")
                    files.emplace_back(file);
            }
        } else if (path.extension() == ".png") {
            files.emplace_back(path);
        }

        for (const auto& file : files) {
            auto name = file.stem().string();

            std::vector<std::uint8_t> image_data;
            std::uint32_t w, h;
            lodepng::decode(image_data, w, h, file.string());

            constexpr unsigned scale = 3;
            auto bilinear_image = BilinearUpscale(image_data, w, h, scale);
            lodepng::encode(fmt::format("{}_bilinear.png", name), bilinear_image, w * scale,
                            h * scale);

            auto [filter_image, offset_data] = GradientPull(image_data, w, h, scale);
            lodepng::encode(fmt::format("{}_gradient_pull.png", name), filter_image, w * scale,
                            h * scale);
            lodepng::encode(fmt::format("{}_offset_data.png", name), offset_data, w * scale,
                            h * scale);
        }
    }
}