#pragma once
#ifndef AMARA_CUTSCENE
#define AMARA_CUTSCENE

#include "amaraRPG.h"

namespace Amara {
    class RPGCutscene: public Amara::RPGCutsceneBase {
        public:
            Amara::RPGScene* rpgScene = nullptr;
            Amara::ActorGroup* group = nullptr;

			Camera* cam;

            Amara::RPGCutscene* chainedCutscene = nullptr;

            virtual void init(Amara::GameProperties* gameProperties) {
                Amara::RPGCutsceneBase::init(gameProperties);
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
                Amara::RPGCutsceneBase::finish();
                if (chainedCutscene) {
                    rpgScene->startCutscene(chainedCutscene);
                }
                onEnd();

                group->destroyActors();
                delete group;
                group = nullptr;
            }

            Amara::RPGCutscene* chain(Amara::RPGCutscene* gCutscene) {
                if (chainedCutscene) {
                    chainedCutscene->chain(gCutscene);
                }
                else {
                    chainedCutscene = gCutscene;
                }
                return gCutscene;
            }

            Amara::RPGCutscene* unchain() {
                Amara::RPGCutscene* rec = chainedCutscene;
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
