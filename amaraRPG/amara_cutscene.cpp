#ifndef AMARA_CUTSCENE
#define AMARA_CUTSCENE

#include "amaraRPG.h"

namespace Amara {
    class Cutscene: Amara::CutsceneBase {
        public:
            Amara::RPGScene* rpgScene = nullptr;

            virtual void init(Amara::GameProperties* gameProperties) {
                Amara::CutsceneBase::init(gameProperties);
                rpgScene = (Amara::RPGScene*)scene;
            }

            void walk(Amara::Walker* walker, Amara::Direction dir) {
                if (once()) {
                    walker->walk(dir);
                    return true;
                }
                return false;
            }

            void walkTo(Amara::Walker* walker, int tx, int ty) {
                if (evt()) {
                    if (walker->walkTo(tx, ty)) {
                        nextEvt();
                    }
                    return true;
                }
                return false;
            }
    }
}

#endif