find_package(fmt CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

add_library(common STATIC
    alignment.hpp
	bilinear_upscale.hpp
	bilinear_upscale.cpp
	file_util.hpp
	file_util.cpp
	open_gl.hpp
	open_gl.cpp
	sdl_helper.hpp
	sdl_helper.cpp
 )

target_include_directories(common PUBLIC .)
target_link_libraries(common PUBLIC fmt::fmt glad::glad SDL2::SDL2)
