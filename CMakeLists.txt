cmake_minimum_required(VERSION 2.8.8)
set(PROJECT_NAME_STR Opengl-Bootstrap)
project(${PROJECT_NAME_STR} C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

#Set up the includes
include(ExternalProject)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build")

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

if (MINGW OR UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")
elseif (MSVC)
	if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
endif()


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

if(MSVC)
#vc 2012 fix for vararg templates
set(MSVC_COMPILER_DEFS "-D_VARIADIC_MAX=10")

set(CMAKE_C_FLAGS_DEBUG_INIT "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
set(CMAKE_C_FLAGS_MINSIZEREL_INIT     "/MT /O1 /Ob1 /D NDEBUG")
set(CMAKE_C_FLAGS_RELEASE_INIT        "/MT /O2 /Ob2 /D NDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "/MT /Zi /O2 /Ob1 /D NDEBUG")

add_definitions(
  ${MSVC_COMPILER_DEFS}
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

elseif (MSVC)
set(SDL2_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/external/SDL2/include/)

# check if 64 bit
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
set(HAVE_64_BIT 1)
else()
set(HAVE_64_BIT 0)
endif()

if(HAVE_64_BIT)
	set(SDL2MAIN_LIBRARY "${PROJECT_SOURCE_DIR}/external/SDL2/visualc/64/SDL2main.lib")
	set(SDL2_LIBRARY_TEMP  "${PROJECT_SOURCE_DIR}/external/SDL2/visualc/64/SDL2.lib")
	set(SDL2_DLL_INCLUDE_PATH ${PROJECT_SOURCE_DIR}external/SDL2/visualc/64/SDL2.dll)
else()
	set(SDL2MAIN_LIBRARY "${PROJECT_SOURCE_DIR}/external/SDL2/visualc/32/SDL2main.lib")
	set(SDL2_LIBRARY_TEMP  "${PROJECT_SOURCE_DIR}/external/SDL2/visualc/32/SDL2.lib")
	set(SDL2_DLL_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/external/SDL2/visualc/32/SDL2.dll)
endif()
set(SDL2_LIBRARY ${SDL2MAIN_LIBRARY} ${SDL2_LIBRARY_TEMP})
else()
	find_package(SDL2 REQUIRED)
endif(MINGW)


#if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
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
#endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")


set(GLEW_SOURCE
	${PROJECT_EXT_DIR}/glew/src/glew.c
)
add_library(GLEW STATIC
	${GLEW_SOURCE}
	${GLEW_INCLUDE}
)


target_link_libraries(GLEW
	${OPENGL_LIBRARY}
	${EXTRA_LIBS}
)

#Get glm
add_subdirectory(
  ${PROJECT_EXT_DIR}/glm
)

set(ext_libs
  ${CMAKE_SOURCE_DIR}/external/glew/include/
  ${GLM_INCLUDE_DIR}
)


include_directories(
        ${ext_libs}
        ${SDL2_INCLUDE_DIR}
        ${PROJECT_INCLUDE_DIR}
)


set(EXT_DEPS
    glm
)

file(GLOB PROJECT_LIBS_FILES ${PROJECT_LIBS_DIR}/*.cpp)
add_library(INT_LIBS 
  ${PROJECT_LIBS_FILES}
)

add_dependencies(INT_LIBS
  ${EXT_DEPS}
)

set(ALL_LIBS
  ${OPENGL_LIBRARY}
  GLEW
  INT_LIBS
  ${SDL2_LIBRARY}
)


file(GLOB PROJECT_CXX_FILES ${PROJECT_CXX_DIR}/*.cpp)
add_executable(${PROJECT_NAME_STR}
  ${PROJECT_CXX_FILES}
)



add_dependencies(${PROJECT_NAME_STR} ${EXT_DEPS})

add_custom_command(TARGET ${PROJECT_NAME_STR}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
                            ${PROJECT_SOURCE_DIR}/copy_dir $<TARGET_FILE_DIR:${PROJECT_NAME_STR}>)

if(MSVC)
add_custom_command(TARGET ${PROJECT_NAME_STR}_MSVC
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
                            ${SDL2_DLL_INCLUDE_PATH} $<TARGET_FILE_DIR:${PROJECT_NAME_STR}>)
endif()

target_link_libraries(${PROJECT_NAME_STR}
  ${ALL_LIBS}
)





