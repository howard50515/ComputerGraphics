cmake_minimum_required(VERSION 3.16)

project(HW1_Renderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE HW1_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(HW1
	main.cpp
	${HW1_SOURCES}
)

target_include_directories(HW1 PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(MSVC)
	target_compile_options(HW1 PRIVATE /W4)
else()
	target_compile_options(HW1 PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(WIN32)
	target_link_libraries(HW1 PRIVATE ws2_32)
endif()
