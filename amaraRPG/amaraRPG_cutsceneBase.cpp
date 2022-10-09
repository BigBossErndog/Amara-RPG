#pragma once
#ifndef AMARA_CUTSCENEBASE
#define AMARA_CUTSCENEBASE

#include "amaraRPG.h"

namespace Amara {
    class RPGCutsceneBase: public Amara::Script {
        public:
            RPGCutsceneBase* chainedCutscene = nullptr;

            bool fadeIn(Amara::Entity* actor, float speed, float targetAlpha) {
                if (evt()) {
                    actor->alpha += speed;
                    if (actor->alpha >= targetAlpha) {
                        actor->alpha = targetAlpha;
                        nextEvt();
                    }
                }
                return false;
            }
            bool fadeIn(Amara::Entity* actor, float speed) {
                return fadeIn(actor, speed, 1);
            }

            bool fadeOut(Amara::Entity* actor, float speed, float targetAlpha) {
                if (evt()) {
                    actor->alpha -= speed;
                    if (actor->alpha <= targetAlpha) {
                        actor->alpha = targetAlpha;
                        nextEvt();
                    }
                }
                return false;
            }
            bool fadeOut(Amara::Entity* actor, float speed) {
                return fadeOut(actor, speed, 0);
            }

            Amara::RPGCutsceneBase* chain(Amara::RPGCutsceneBase* cutscene) {
                chainedCutscene = cutscene;
                return cutscene;
            }

            ~RPGCutsceneBase() {
                if (chainedCutscene != nullptr && !chainedCutscene->initiated) {
                    delete chainedCutscene;
                    chainedCutscene = nullptr;
                }
            }
    };
}

#endif