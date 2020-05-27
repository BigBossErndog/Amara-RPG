#pragma once
#ifndef AMARA_CUTSCENE
#define AMARA_CUTSCENE

#include "amaraRPG.h"

namespace Amara {
    class Cutscene: public Amara::CutsceneBase {
        public:
            Amara::RPGScene* rpgScene = nullptr;
            Amara::ActorGroup* group = nullptr;

            virtual void init(Amara::GameProperties* gameProperties) {
                Amara::CutsceneBase::init(gameProperties);
                rpgScene = (Amara::RPGScene*)scene;

                if (group != nullptr) {
                    delete group;
                }
                group = new ActorGroup(this);
            }

            virtual Amara::Actor* add(Amara::Actor* actor) {
                scene->add(actor);
                group->add(actor);
                return actor;
            }

            void finish() {
                Amara::CutsceneBase::finish();
                onEnd();

                group->destroyActors();
                delete group;
                group = nullptr;
            }

            virtual void onEnd() {
                
            }

            bool walk(Amara::Walker* walker, Amara::Direction dir) {
                if (once()) {
                    walker->walk(dir);
                    return true;
                }
                return false;
            }

            bool walk(Amara::Walker* walker, int tx, int ty) {
                if (evt()) {
                    if (walker->walkTo(tx, ty)) {
                        nextEvt();
                    }
                    return true;
                }
                return false;
            }
    };
}

#endif