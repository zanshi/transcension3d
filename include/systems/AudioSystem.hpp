//
// Created by Johan Nordin on 26/05/15.
//

#pragma once


#include <entityx/entityx.h>
#include <SDL_mixer.h>
#include <events/AudioEvent.hpp>


namespace ex = entityx;

namespace sw {

    class AudioSystem : public ex::System<AudioSystem>, public ex::Receiver<AudioSystem>  {


    public:
        AudioSystem() {
            relative_path_to_sound_folder_ = "../res/sounds/";
        }
        ~AudioSystem() {

            //Free the sound effects
            Mix_FreeChunk( gScratch );
        }

        void backgroundMusic() {
            //Play the music
            Mix_PlayMusic( gMusic, -1 );
        }

        void loadSounds(){
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

        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {

        }

        void configure(ex::EventManager &events) override {
            events.subscribe<JumpEvent>(*this);
            events.subscribe<AudioEvent>(*this);
        }


        void receive(const JumpEvent &sound) {
            // arguments -> channel, sound, repeated?
            Mix_PlayChannel( -1, gScratch, 0 );

        }
        void receive(const AudioEvent &sound) {
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


    private:
        std::string relative_path_to_sound_folder_;

        //The music that will be played
        Mix_Music *gMusic = NULL;

        //The sound effects that will be used
        Mix_Chunk *gJump = NULL;
        Mix_Chunk *gScratch = NULL;



    };
}
