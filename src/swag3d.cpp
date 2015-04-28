// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <chrono>
#include <ctime>


#include "cleanup.h"

// Include GLEW
#ifdef __APPLE_CC__

//#include <OpenGL/gl3.h>
#include <GL/glew.h>

#define GLFW_INCLUDE_GLCOREARB
#else
#include "GL/glew.h"
#endif

#include "SDL.h"

#undef main

// Include GLM
#include "glm/glm.hpp"

#include <common/Init.h>
#include <common/Shader.h>
#include <Application.hpp>

#include "entityx/entityx.h"

//#include "Application.hpp"

namespace ex = entityx;

int main(int argc, char *argv[]) {
    //First we need to start up SDL, and make sure it went ok
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    /* Request opengl 3.3 context.
     *      * SDL doesn't have the ability to choose which profile at this time of writing,
     *      * but it should default to the core profile */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    /* Turn on double buffering with a 24bit Z buffer.
     * You may need to change this to 16 or 32 for your system */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    //Now create a window with title "Hello World" at 100, 100 on the screen with w:640 h:480 and show it
    SDL_Window *win = SDL_CreateWindow("Hello Swag3d!", 100, 100, 640, 480, SDL_WINDOW_OPENGL);
    //Make sure creating our window went ok
    if (win == nullptr) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_GLContext glcontext = SDL_GL_CreateContext(win);
    SDL_GL_MakeCurrent(win, glcontext);

    //Init init;
    //init.glew();

    //Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if( glewError != GLEW_OK )
    {
        printf( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
    }

    // Application initialization
    sw::Application app;
    app.initScene();

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point current, last;

    int counter = 0;
    int counter2 = 0;

    SDL_Event e;
    bool quit = false;

    while (!quit) {

        current = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> dt = std::chrono::duration_cast<std::chrono::duration<double>>(current - last);
        last = current;

        glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        //glCullFace(GL_BACK); //TODO: Check


        app.update(dt.count());

        // Event polling
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // TODO: check if this is done correct
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        SDL_GL_SwapWindow(win);


    } // Check if the ESC key was pressed or the window was closed

    // Clean up
    SDL_GL_DeleteContext(glcontext);
    cleanup(win);
    SDL_Quit();


    return 0;

}
