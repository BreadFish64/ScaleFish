find_path(LODEPNG_INCLUDE_DIR lodepng.h)
find_library(LODEPNG_LIB lodepng)

add_executable(gradient_pull
	gradient_pull.hpp
	gradient_pull.cpp
)

set(shader_files
	gradient_pull.vert
	gradient_pull.frag
)
foreach (shader_file ${shader_files})
	configure_file(${shader_file} ${shader_file} COPYONLY)
endforeach()

target_link_libraries(gradient_pull PUBLIC common)
target_include_directories(gradient_pull PRIVATE ${LODEPNG_INCLUDE_DIR})
target_link_libraries(gradient_pull PRIVATE ${LODEPNG_LIB})
