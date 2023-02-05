cmake_minimum_required(VERSION 3.11)

project(OpenGL)
add_library(opengl STATIC)

set_target_properties(opengl PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"

	PREFIX "lib"
	OUTPUT_NAME "opengl"
)

if(MSVC)
	target_compile_options(opengl PRIVATE /w)
else()
	target_compile_options(opengl PRIVATE -w)
endif()

target_sources(opengl PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/gl.c")