find_package(lodepng REQUIRED)
find_package(Boost REQUIRED)

add_executable(scaleforce
	scaleforce.hpp
	"main.cpp"
 "scaleforce.cpp")

set(shader_files
	scaleforce.comp
)
foreach (shader_file ${shader_files})
	configure_file(${shader_file} ${shader_file} COPYONLY)
endforeach()

target_link_libraries(scaleforce PRIVATE common lodepng)