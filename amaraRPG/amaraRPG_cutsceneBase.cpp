#pragma once
#ifndef AMARA_CUTSCENEBASE
#define AMARA_CUTSCENEBASE

#include "amaraRPG.h"

namespace Amara {
    class CutsceneBase: public Amara::Script {
        public:
            bool fadeIn(Amara::Actor* actor, float speed, float targetAlpha) {
                if (evt()) {
                    actor->alpha += speed;
                    if (actor->alpha >= targetAlpha) {
                        actor->alpha = targetAlpha;
                        nextEvt();
                    }
                }
                return false;
            }
            bool fadeIn(Amara::Actor* actor, float speed) {
                return fadeIn(actor, speed, 1);
            }

            bool fadeOut(Amara::Actor* actor, float speed, float targetAlpha) {
                if (evt()) {
                    actor->alpha -= speed;
                    if (actor->alpha <= targetAlpha) {
                        actor->alpha = targetAlpha;
                        nextEvt();
                    }
                }
                return false;
            }
            bool fadeOut(Amara::Actor* actor, float speed) {
                return fadeOut(actor, speed, 0);
            }
    };
}

#endif