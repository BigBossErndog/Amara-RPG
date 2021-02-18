#pragma once
#include "amaraRPG.h"

namespace Amara {
    class EventTrigger: public Script {
        public:
            RPGScene* rpgScene;

            string dayRecordAddKey;
            json dayRecordAddValue = nullptr;

            string recordAddKey;
            json recordAddValue = nullptr;

            bool finishOnTrigger = true;

            virtual void init(Amara::GameProperties* gameProperties) {
                Amara::Script::init(gameProperties);
                rpgScene = (RPGScene*)scene;

                dayRecordAddKey.clear();
                recordAddKey.clear();
            }

            void finish() {
                Script::finish();
                if (!dayRecordAddKey.empty()) {
                    data_dayRecordAdd(this, dayRecordAddKey, dayRecordAddValue);
                }
                if (!recordAddKey.empty()) {
                    data_recordAdd(this, recordAddKey, recordAddValue);
                }
            }

            void dayRecordAdd(string item, json entry) {
                dayRecordAddKey = item;
                dayRecordAddValue = entry;
            }
            void recordAdd(string item, json entry) {
                recordAddKey = item;
                recordAddValue = entry;
            }
    };

    class EventTriggerXY: public EventTrigger {
        public:
            Walker* walker;
            Cutscene* cutscene;

            int x;
            int y;

            EventTriggerXY(Walker* gWalker, int gx, int gy, Cutscene* gScene) {
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

    class EventTriggerX: public EventTrigger {
        public:
            Walker* walker;
            Cutscene* cutscene;

            int x;

            EventTriggerX(Walker* gWalker, int gx, Cutscene* gScene) {
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

            ~EventTriggerX() {
                if (!finished) {
                    if (cutscene) {
                        delete cutscene;
                    }
                }
            }
    };

    class EventTriggerY: public EventTrigger {
        public:
            Walker* walker;
            Cutscene* cutscene;

            int y;

            EventTriggerY(Walker* gWalker, int gy, Cutscene* gScene) {
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

            ~EventTriggerY() {
                if (!finished) {
                    if (cutscene) {
                        delete cutscene;
                    }
                }
            }
    };
}