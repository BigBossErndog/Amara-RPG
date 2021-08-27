#pragma once
#include "amaraRPG.h"

namespace Amara {
    class EventTrigger: public Script {
        public:
            RPGScene* rpgScene;

            bool finishOnTrigger = true;

            virtual void init(Amara::GameProperties* gameProperties) {
                Amara::Script::init(gameProperties);
                rpgScene = (RPGScene*)scene;
            }

            void finish() {
                Script::finish();
            }
    };

    class EventTriggerXY: public EventTrigger {
        public:
            Walker* walker;
            RPGCutscene* cutscene;

            int x;
            int y;

            EventTriggerXY(Walker* gWalker, int gx, int gy, RPGCutscene* gScene) {
                walker = gWalker;
                x = gx;
                y = gy;
                cutscene = gScene;
            }

            void script() {
                if (rpgScene->sm.inState("duration") && walker->isOn(x, y)) {
                    rpgScene->startCutscene(cutscene);
                    walker->stopWalking();
                    if (finishOnTrigger) finish();
                    else cutscene->deleteOnFinish = false;
                }
            }

            ~EventTriggerXY() {
                if (!finished) {
                    if (cutscene) {
                        delete cutscene;
                    }
                }
            }
    };

    class EventTriggerTileX: public EventTrigger {
        public:
            Walker* walker;
            RPGCutscene* cutscene;

            int x;

            EventTriggerTileX(Walker* gWalker, int gx, RPGCutscene* gScene) {
                walker = gWalker;
                x = gx;
                cutscene = gScene;
            }

            void script() {
                if (rpgScene->sm.inState("duration") && walker->isOnX(x)) {
                    rpgScene->startCutscene(cutscene);
                    walker->stopWalking();
                    if (finishOnTrigger) finish();
                    else cutscene->deleteOnFinish = false;
                }
            }

            ~EventTriggerTileX() {
                if (!finished) {
                    if (cutscene) {
                        delete cutscene;
                    }
                }
            }
    };

    class EventTriggerTileY: public EventTrigger {
        public:
            Walker* walker;
            RPGCutscene* cutscene;

            int y;

            EventTriggerTileY(Walker* gWalker, int gy, RPGCutscene* gScene) {
                walker = gWalker;
                y = gy;
                cutscene = gScene;
            }

            void script() {
                if (rpgScene->sm.inState("duration") && walker->isOnY(y)) {
                    rpgScene->startCutscene(cutscene);
                    walker->stopWalking();
                    if (finishOnTrigger) finish();
                    else cutscene->deleteOnFinish = false;
                }
            }

            ~EventTriggerTileY() {
                if (!finished) {
                    if (cutscene) {
                        delete cutscene;
                    }
                }
            }
    };
}