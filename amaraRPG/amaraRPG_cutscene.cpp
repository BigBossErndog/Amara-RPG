#pragma once
#ifndef AMARA_CUTSCENE
#define AMARA_CUTSCENE

#include "amaraRPG.h"

namespace Amara {
    class Cutscene: public Amara::CutsceneBase {
        public:
            Amara::RPGScene* rpgScene = nullptr;
            Amara::ActorGroup* group = nullptr;

			Camera* cam;

            Amara::Cutscene* chainedCutscene = nullptr;

            virtual void init(Amara::GameProperties* gameProperties) {
                Amara::CutsceneBase::init(gameProperties);
                rpgScene = (Amara::RPGScene*)scene;
                if (group != nullptr) {
                    delete group;
                }
                rpgScene->recite(group = new ActorGroup());
				cam = scene->mainCamera;
            }

            virtual Amara::Actor* add(Amara::Actor* actor) {
                group->add(true, actor);
                return actor;
            }

            void finish() {
                Amara::CutsceneBase::finish();
                if (chainedCutscene) {
                    rpgScene->startCutscene(chainedCutscene);
                }
                onEnd();

                group->destroyActors();
                delete group;
                group = nullptr;
            }

            Amara::Cutscene* chain(Amara::Cutscene* gCutscene) {
                if (chainedCutscene) {
                    chainedCutscene->chain(gCutscene);
                }
                else {
                    chainedCutscene = gCutscene;
                }
                return gCutscene;
            }

            Amara::Cutscene* unchain() {
                Amara::Cutscene* rec = chainedCutscene;
                chainedCutscene = nullptr;
                return rec;
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

            bool run(Amara::Walker* walker, Amara::Direction dir) {
                if (once()) {
                    walker->run(dir);
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

            bool run(Amara::Walker* walker, int tx, int ty) {
                if (evt()) {
                    if (walker->runTo(tx, ty)) {
                        nextEvt();
                    }
                    return true;
                }
                return false;
            }
    };
}

#endif
