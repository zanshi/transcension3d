//
// Created by Johan Nordin on 26/05/15.
//

#pragma once

namespace sw {

    struct AudioEvent {
        AudioEvent(bool isPlaying ) : isPlaying_(isPlaying) {}

        bool isPlaying_;
    };
}