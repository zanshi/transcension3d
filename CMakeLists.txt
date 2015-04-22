cmake_minimum_required(VERSION 2.8.8)
set(PROJECT_NAME_STR swag3d)
project(${PROJECT_NAME_STR} C CXX)



set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

#Set up the includes
include(ExternalProject)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_CXX_DIR ${PROJECT_SOURCE_DIR}/src)
set(PROJECT_LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)
set(PROJECT_EXT_DIR ${PROJECT_SOURCE_DIR}/external)


if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
  message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
  message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


if(CMAKE_COMPILER_IS_GNUCXX)
add_definitions(
  -D__NO_INLINE__
  -DTW_STATIC
  -DTW_NO_LIB_PRAGMA
  -DTW_NO_DIRECT3D
  -DGLEW_STATIC
  -D_CRT_SECURE_NO_WARNINGS
)

endif()


find_package(OpenGL REQUIRED)

if (MINGW)
SET(CMAKE_EXE_LINKER_FLAGS "-static")
set(SDL2_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/external/SDL2/include/)
set(SDL2MAIN_LIBRARY "${PROJECT_SOURCE_DIR}/external/SDL2/lib/libSDL2main.a")
set(SDL2_LIBRARY_TEMP  "${PROJECT_SOURCE_DIR}/external/SDL2/lib/libSDL2.a")
set(SDL2_LIBRARY ${SDL2MAIN_LIBRARY} ${SDL2_LIBRARY_TEMP})
set(ZLIB_LIBRARY "${PROJECT_SOURCE_DIR}/external/zlib/libzlibstatic.a")
set(ASSIMP_LIBRARY "${PROJECT_SOURCE_DIR}/external/assimp/lib32/libassimp.a")

else()
	find_package(SDL2 REQUIRED)
	set(ASSIMP_LIBRARY assimp)
endif(MINGW)


if(MINGW)
set(OPENGL_LIBRARY
    ${OPENGL_LIBRARY}
    -mwindows
    -mconsole
    -lmingw32 -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid
    ${CMAKE_DL_LIBS}
)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
set(OPENGL_LIBRARY
	${OPENGL_LIBRARY}
#	-lGL -lGLU -lXrandr -lXext -lX11 -lrt
	${CMAKE_DL_LIBS}
)
add_definitions(-D_UNIX -DSDL2=ON)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

SET(EXTRA_LIBS "-framework Cocoa -framework OpenGL -framework IOKit -framework Foundation -framework AppKit")

if(CMAKE_GENERATOR STREQUAL Xcode)
        add_definitions(-D_MACOSX -ObjC++)
endif()

endif(MINGW)


set(GLEW_SOURCE
	${PROJECT_EXT_DIR}/glew/src/glew.c
)
add_library(GLEW STATIC
	${GLEW_SOURCE}
	${GLEW_INCLUDE}
)

## Assimp

if(UNIX)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_SAMPLES OFF)

include(BuildAssimp)
endif(UNIX)

target_link_libraries(GLEW
	${OPENGL_LIBRARY}
	${EXTRA_LIBS}
)

add_subdirectory(
  ${PROJECT_EXT_DIR}/entityx
)


set(ext_libs
  ${CMAKE_SOURCE_DIR}/external/glew/include/
  ${CMAKE_SOURCE_DIR}/external/entityx/
  ${CMAKE_SOURCE_DIR}/external/assimp/include
)


include_directories(
        ${ext_libs}
        ${SDL2_INCLUDE_DIR}
        ${PROJECT_INCLUDE_DIR}
)

set(ALL_LIBS
  ${OPENGL_LIBRARY}
  GLEW
  INT_LIBS
  entityx
  ${SDL2_LIBRARY}
  ${ASSIMP_LIBRARY}
  ${ZLIB_LIBRARY}
)





set(EXT_DEPS
    entityx
    ${ASSIMP_LIBRARY}
)


file(GLOB PROJECT_LIBS_FILES ${PROJECT_LIBS_DIR}/*.cpp)
add_library(INT_LIBS 
  ${PROJECT_LIBS_FILES}
)

add_dependencies(INT_LIBS
  ${EXT_DEPS}
)


file(GLOB PROJECT_CXX_FILES ${PROJECT_CXX_DIR}/*.cpp)
add_executable(${PROJECT_NAME_STR}
  ${PROJECT_CXX_FILES}
)


target_link_libraries(${PROJECT_NAME_STR}
  ${ALL_LIBS}
)





