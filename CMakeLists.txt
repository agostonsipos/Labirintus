cmake_minimum_required(VERSION 3.5)

add_executable(Labirintus
src/main.cpp
src/Mesh_OGL3.cpp
src/MyApp.cpp
src/ObjParser_OGL3.cpp
src/Mesh_OGL3.h
src/MyApp.h
src/ObjParser_OGL3.h
src/GLUtils.hpp
)

set(tarver "x86")
set(CMAKE_CXX_FLAGS "-std=c++11 -O2")

if(UNIX)
	set(libdir "/usr/lib/")
	include_directories(/usr/include/SDL2)
	find_library(GLEW_LIB REQUIRED GLEW HINTS ${libdir})
	find_library(GLU_LIB REQUIRED GLU HINTS ${libdir})
	find_library(OGL_LIB REQUIRED GL HINTS ${libdir})
endif()
if(WIN32)
	set(libdir "OGLPack/lib/${tarver}")
	include_directories(OGLPack/include)
	find_library(GLEW_LIB NAME glew32s HINTS ${libdir})
	find_library(GLU_LIB NAME GlU32 HINTS ${libdir})
	find_library(OGL_LIB NAME OPENGL32 HINTS ${libdir})
endif()

find_library(SDL2main_LIB NAME SDL2main HINTS ${libdir})
find_library(SDL2_LIB NAME SDL2 HINTS ${libdir})
find_library(SDL2_image_LIB NAME SDL2_image HINTS ${libdir})

target_link_libraries(Labirintus PUBLIC ${GLEW_LIB})
target_link_libraries(Labirintus PUBLIC ${GLU_LIB})
target_link_libraries(Labirintus PUBLIC ${OGL_LIB})
target_link_libraries(Labirintus PUBLIC ${SDL2main_LIB})
target_link_libraries(Labirintus PUBLIC ${SDL2_LIB})
target_link_libraries(Labirintus PUBLIC ${SDL2_image_LIB})
