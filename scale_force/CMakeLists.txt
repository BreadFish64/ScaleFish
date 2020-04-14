find_path(LODEPNG_INCLUDE_DIR lodepng.h)
find_library(LODEPNG_LIB lodepng)

add_executable(scale_force
	scale_force.hpp
	scale_force.cpp
)

set(shader_files
	scale_force.vert
	scale_force.frag
)
foreach (shader_file ${shader_files})
	configure_file(${shader_file} ${shader_file} COPYONLY)
endforeach()

target_link_libraries(scale_force PUBLIC common)
target_include_directories(scale_force PRIVATE ${LODEPNG_INCLUDE_DIR})
target_link_libraries(scale_force PRIVATE ${LODEPNG_LIB})
