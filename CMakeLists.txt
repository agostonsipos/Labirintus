cmake_minimum_required(VERSION 3.5)

add_executable(Labirintus
src/main.cpp
src/MyApp.hpp
src/MyApp.cpp
src/ObjParser_OGL3.hpp
src/ObjParser_OGL3.cpp
src/Mesh_OGL3.hpp
src/Mesh_OGL3.cpp
src/GLUtils.hpp
src/gShaderProgram.hpp
src/gShaderProgram.cpp
src/GameObjects.hpp
src/GameObjects.cpp
)

set(tarver "x86")
set(CMAKE_CXX_FLAGS "-std=c++11 -O2")

# For creating installable binary on Linux
#add_definitions(-D__INSTALL)

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
find_library(SDL2_ttf_LIB NAME SDL2_ttf HINTS ${libdir})

target_link_libraries(Labirintus PUBLIC ${GLEW_LIB})
target_link_libraries(Labirintus PUBLIC ${GLU_LIB})
target_link_libraries(Labirintus PUBLIC ${OGL_LIB})
target_link_libraries(Labirintus PUBLIC ${SDL2main_LIB})
target_link_libraries(Labirintus PUBLIC ${SDL2_LIB})
target_link_libraries(Labirintus PUBLIC ${SDL2_image_LIB})
target_link_libraries(Labirintus PUBLIC ${SDL2_ttf_LIB})
