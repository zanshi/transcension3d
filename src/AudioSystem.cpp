//
// Created by cristoffer on 2015-05-27.
//

#include <systems/AudioSystem.hpp>


sw::AudioSystem::AudioSystem() {
    relative_path_to_sound_folder_ = "../res/sounds/";
}

sw::AudioSystem::~AudioSystem() {

    //Free the sound effects
    Mix_FreeChunk( gScratch );
}
void sw::AudioSystem::backgroundMusic() {
    //Play the music
    Mix_PlayMusic( gMusic, -1 );
}

void sw::AudioSystem::loadSounds(){
    //Load sound effects
    //std:string name = "scratch.wav";
    //filename = relative_path_to_sound_folder_ + name;

    gScratch = Mix_LoadWAV( "../sounds/scratch.wav" ); // filename
    gMusic = Mix_LoadMUS( "../sounds/beat.wav" );

    if( gScratch == NULL || gMusic == NULL )
    {
        std::cout << "Failed to load scratch sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;

    }
}

void sw::AudioSystem::configure(ex::EventManager &events)  {
    events.subscribe<JumpEvent>(*this);
    events.subscribe<AudioEvent>(*this);
}

void sw::AudioSystem::receive(const JumpEvent &sound) {
    // arguments -> channel, sound, repeated?
    Mix_PlayChannel( -1, gScratch, 0 );

}
void sw::AudioSystem::receive(const AudioEvent &sound) {
    // arguments -> sound, repeated?

    //If the music is paused
    if( Mix_PausedMusic() != sound.isPlaying_ )
    {
        //Resume the music
        Mix_ResumeMusic();
    }
        //If the music is playing
    else
    {
        //Pause the music
        Mix_PauseMusic();
    }

}
