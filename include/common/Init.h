#pragma once

#include <string>

class Init {
private:
    bool glewInitialized = false;
public:

    void glew() {
        // Initialize GLEW if not on apple
        // Apple already has such context-info, and thus, this isn't needed.
#ifndef __APPLE__        
            if(!glewInitialized){
                glewExperimental=GL_TRUE;
                GLenum err = glewInit();
                if (err != GLEW_OK) {
                    fprintf(stderr, "Failed to initialize GLEW\n");
                    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
                    return;
                }
                fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
                glewInitialized = true;
            }
#endif
    }

};

