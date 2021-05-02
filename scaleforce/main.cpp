#include "scaleforce.hpp"

#include "common/bilinear_upscale.hpp"
#include "common/open_gl.hpp"

#include <lodepng.h>

#include <algorithm>
#include <charconv>
#include <filesystem>
#include <span>

using namespace std::literals;

int main(int argc, const char* argv[]) {
    std::vector<std::string_view> args{argv, argv + argc};
    std::vector<std::filesystem::path> paths;
    // C++ made this near impossible to do with ranges, and I am severely disappointed
    for (auto arg : args) {
        std::filesystem::path path{arg};
        if (std::filesystem::exists(path)) paths.push_back(std::move(path));
    }

    const bool debug = std::find(args.begin(), args.end(), "--debug"sv) != args.end();

    unsigned scale_factor = 2;
    for (auto arg : args) {
        unsigned maybe_scale;
        if (std::from_chars(arg.data(), arg.data() + arg.size(), maybe_scale).ec !=
            std::errc::invalid_argument) {
            scale_factor = maybe_scale;
        }
    }

    auto [window, context] = OpenGL::InitContext(true, debug);
    ScaleForce scaleforce{};
    scaleforce.write_offsets = debug;

    //for (;;) {
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

                auto bilinear_image = BilinearUpscale(image_data, w, h, scale_factor);
                lodepng::encode(fmt::format("{}_bilinear.png", name), bilinear_image,
                                w * scale_factor, h * scale_factor);

                auto filter_image = scaleforce.Scale(image_data, w, h, scale_factor);
                lodepng::encode(fmt::format("{}_scaleforce.png", name), filter_image.data(),
                                w * scale_factor, h * scale_factor);

                if (scaleforce.write_offsets) {
                    lodepng::encode(fmt::format("{}_offsets.png", name),
                                    filter_image.data() + filter_image.size() / 2, w * scale_factor,
                                    h * scale_factor);
                }
            }
        }
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) std::exit(0);
        }
        SDL_GL_SwapWindow(window.get());
    //}
}
