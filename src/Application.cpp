//
// Created by Benjamin Wiberg on 15-04-23.
//

#include "Application.hpp"

#include <cleanup.h>
#include <SDL_mixer.h>
#include "assimp/Importer.hpp"
#include "scene/SceneImporter.hpp"
#include "components/all_components.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/DebugSystem.hpp"
#include "systems/PhysicsSystem.hpp"
#include "systems/InputSystem.hpp"
#include "systems/PlayerControlSystem.hpp"
#include "systems/AudioSystem.hpp"
#include <chrono>

sw::Application::Application() {
    systems.add<InputSystem>();
    systems.add<AudioSystem>();
    systems.add<RenderSystem>(events);
    systems.add<DebugSystem>(std::cout);
    systems.add<PhysicsSystem>();
    systems.add<PlayerControlSystem>();

    systems.configure();

    events.subscribe<QuitEvent>(*this);
}

void sw::Application::update(ex::TimeDelta dt) {
    systems.update<InputSystem>(dt);
    systems.update<AudioSystem>(dt);
    systems.update<PlayerControlSystem>(dt);
    systems.update<PhysicsSystem>(dt);
    systems.update<RenderSystem>(dt);
    systems.update<DebugSystem>(dt);

}

bool sw::Application::init() {
    //First we need to start up SDL, and make sure it went ok
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    //Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer could not initialize! SDL_mixer Error: %s\n" << Mix_GetError() << std::endl;
        return false;
    }

    /* Request opengl 3.3 context.
     *      * SDL doesn't have the ability to choose which profile at this time of writing,
     *      * but it should default to the core profile */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    /* Turn on double buffering with a 24bit Z buffer.
     * You may need to change this to 16 or 32 for your system */
    //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);


    //Now create a window with title "Hello World" at 100, 100 on the screen with w:640 h:480 and show it
    win = SDL_CreateWindow("Hello Transcension3D!", 100, 100, sw::WINDOW_WIDTH, sw::WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    //Make sure creating our window went ok
    if (win == nullptr) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return false;
    }

    glcontext = SDL_GL_CreateContext(win);
    SDL_GL_MakeCurrent(win, glcontext);

    SDL_GL_SetSwapInterval(0);

    //Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
    }

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    systems.system < InputSystem > ()->setWindowSize(sw::WINDOW_WIDTH, sw::WINDOW_HEIGHT);

    return true;

}

void sw::Application::initScene(std::string input) {

    const std::string filename = input;

    // SceneImporter shell
    SceneImporter::relative_path_to_scene_folder_ = "../res/";
    SceneImporter sceneImporter(filename);

    //Load the sound files
    auto audioSystem = systems.system < AudioSystem > ();
    audioSystem->loadSounds();
    audioSystem->backgroundMusic();

    /* The coast is clear, now we can start loading the scene */
    sceneImporter.populateInternalGraph([&]() { return entities.create(); });

    auto renderSystem = systems.system < RenderSystem > ();
    renderSystem->initShader();

    auto physicsSystem = systems.system < PhysicsSystem > ();
    physicsSystem->populateWorld(entities);

    current_dim_ = sw::STARTING_DIMENSION;
}

void sw::Application::updateFPS(float newFPS) {
    std::string FPS_str = "FPS: " + std::to_string(newFPS);
    SDL_SetWindowTitle(win, FPS_str.c_str());
}

void sw::Application::run(std::string input) {
    initScene(input);

    std::chrono::high_resolution_clock::time_point last = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point current;

    std::chrono::duration<double> second_accumulator;
    int frames_last_second = 0;

    isRunning = true;

    while (isRunning) {
        current = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> dt = std::chrono::duration_cast<std::chrono::duration<double>>(current - last);
        last = current;

        glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_BACK); //TODO: Check

        update(dt.count());

        // TODO: check if this is done correct
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        SDL_GL_SwapWindow(win);

        /* FPS DISPLAY HANDLING */
        second_accumulator += dt;
        if (second_accumulator.count() >= 1.0) {
            float newFPS = static_cast<float>( frames_last_second / second_accumulator.count());
            updateFPS(newFPS);
            frames_last_second = 0;
            second_accumulator = std::chrono::duration<double>(0);
        }

        frames_last_second++;
    } // Exits if a QuitEvent is received

    // Clean up
    SDL_GL_DeleteContext(glcontext);
    cleanup(win);
    //Quit SDL subsystems
    SDL_Quit();
    Mix_Quit();
}

void sw::Application::receive(const QuitEvent &quitEvent) {
    isRunning = false;
}
