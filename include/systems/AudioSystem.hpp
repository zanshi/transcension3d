//
// Created by Johan Nordin on 26/05/15.
//

#pragma once


#include <entityx/entityx.h>
#include <SDL_mixer.h>
#include <events/AudioEvent.hpp>
#include <events/JumpEvent.hpp>


namespace ex = entityx;

namespace sw {

    class AudioSystem : public ex::System<AudioSystem>, public ex::Receiver<AudioSystem>  {


    public:
        AudioSystem();

        ~AudioSystem();

        void backgroundMusic();

        void loadSounds();

        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override { }

        void configure(ex::EventManager &events) override;

        void receive(const JumpEvent &sound);
        void receive(const AudioEvent &sound);

    private:
        std::string relative_path_to_sound_folder_;

        //The music that will be played
        Mix_Music *gMusic = NULL;

        //The sound effects that will be used
        Mix_Chunk *gJump = NULL;
        Mix_Chunk *gScratch = NULL;



    };
}
