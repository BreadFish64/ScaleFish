find_package(fmt CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

add_library(common STATIC
    # ensures that a .lib file is generated on windows
    dummy.cpp

	bilinear_upscale.hpp
	dimensional_array.hpp
	file_util.hpp
	open_gl.hpp
	sdl_helper.hpp
	vector_math.hpp
)

set_target_properties(common PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(common PUBLIC fmt::fmt glad::glad SDL2::SDL2)
