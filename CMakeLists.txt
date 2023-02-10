cmake_minimum_required(VERSION 3.11)

project(RayTracer VERSION 1.0.0)
add_executable(raytracer)

if(NOT DEFINED SFML_PATH)
	set(SFML_PATH "${THIRD_PARTY_DIRECTORY}/SFML/lib/cmake/SFML")
endif()

if(NOT DEFINED SFML_VERSION)
	set(SFML_VERSION 2.5)
endif()

set(SOURCES_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(THIRD_PARTY_DIRECTORY "${SOURCES_DIRECTORY}/3rdparty")
set(OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set_target_properties(raytracer PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO

	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}/debug"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}/release"

	OUTPUT_NAME "rt"
)

if(MSVC)
	target_compile_options(raytracer PRIVATE /W4)
else()
	target_compile_options(raytracer PRIVATE -Wall -Wextra -Wpedantic)
endif()

set(SFML_DIR ${SFML_PATH})
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML ${SFML_VERSION} COMPONENTS window system REQUIRED)
target_link_libraries(raytracer sfml-window sfml-system)

add_subdirectory("${THIRD_PARTY_DIRECTORY}/opengl")
target_link_libraries(raytracer opengl)

target_include_directories(raytracer PRIVATE ${SOURCES_DIRECTORY})

target_sources(raytracer
	PRIVATE ${SOURCES_DIRECTORY}/math/Matrix.cpp
	PRIVATE ${SOURCES_DIRECTORY}/system/FileWork.cpp

	PRIVATE ${SOURCES_DIRECTORY}/main.cpp
)