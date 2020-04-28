#include <filesystem>
#include <lodepng.h>

#include "common/bilinear_upscale.hpp"
#include "common/open_gl.hpp"
#include "scale_force.hpp"

int main(int argc, const char* argv[]) {
    std::vector<std::string_view> args{argv, argv + argc};
    args.push_back("lisia.png");
    std::vector<std::filesystem::path> paths{
        std::remove_if(args.begin(), args.end(),
                       [](std::string_view x) { return std::filesystem::exists(x); }),
        args.end()};

    OpenGL::InitOffscreenContext();
    ScaleForce scale_force{};

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

            constexpr unsigned scale = 4;
            auto bilinear_image = BilinearUpscale(image_data, w, h, scale);
            lodepng::encode(fmt::format("{}_bilinear.png", name), bilinear_image, w * scale,
                            h * scale);

            auto filter_image = scale_force.Scale(image_data, w, h, scale);
            lodepng::encode(fmt::format("{}_gradient_pull.png", name), filter_image, w * scale,
                            h * scale);
        }
    }
}
