//
// Created by Cristoffer on 10/04/2015.
//

#include <SDL.h>

#ifndef OPENGL_BOOTSTRAP_CONTROLLER_H
#define OPENGL_BOOTSTRAP_CONTROLLER_H



class Controller
{
public:
    Controller();

    ~Controller();

    static void handle_event( SDL_Event* e);



    static bool return_boolean();

private:
    static bool test;

};

#endif //OPENGL_BOOTSTRAP_CONTROLLER_H
