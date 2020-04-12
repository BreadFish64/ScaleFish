find_path(LODEPNG_INCLUDE_DIR lodepng.h)
find_library(LODEPNG_LIB lodepng)

add_executable(most_used_colors
	most_used_colors.hpp
	most_used_colors_main.cpp
)

target_link_libraries(most_used_colors PUBLIC common)
target_include_directories(most_used_colors PRIVATE ${LODEPNG_INCLUDE_DIR})
target_link_libraries(most_used_colors PRIVATE ${LODEPNG_LIB})
