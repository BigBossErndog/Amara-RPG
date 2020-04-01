#ifndef AMARA_SCRIPT
#define AMARA_SCRIPT

#include "amara.h"

namespace Amara {
    class Game;
    class Scene;
    class Actor;

    class Script: Amara::StateManager {
        public:
            Amara::GameProperties* properties = nullptr;
            Amara::Actor* parent = nullptr;
            Amara::Scene* scene = nullptr;
            Amara::Game* game = nullptr;
            Amara::InputManager* input = nullptr;
            Amara::ControlScheme* controls = nullptr;
            Amara::AudioGroup* audio = nullptr;

            bool deleteOnFinish = true;

            Script(bool deleteWhenDone): Amara::StateManager() {
                deleteOnFinish = deleteWhenDone;
            }

            Script(): Script(true) {}

            void init(Amara::GameProperties* gameProperties, Amara::Actor* parentActor) {
                properties = gameProperties;
                game = properties->game;
                scene = properties->currentScene;
                input = properties->input;
                controls = properties->controls;
                audio = properties->audio;

                parent = parentActor;
            }
            
            bool finished = false;
            
            virtual void finish() {
                finished = true;
            }
            void finishEvt() {
                if (evt()) {
                    finish();
                }
            }

            virtual void prepare() {}
            virtual void prepare(Amara::Actor* entity) {}

            virtual void script() {}
            virtual void script(Amara::Actor* entity) {}
    };
}

#endif