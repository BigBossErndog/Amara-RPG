#pragma once
#include "amaraRPG.h"

namespace Amara {
    class RPGEventTrigger: public Script {
        public:
            RPGScene* rpgScene;
			RPGCutscene* cutscene;

            bool finishOnTrigger = true;

            virtual void init(Amara::GameProperties* gameProperties) {
                Amara::Script::init(gameProperties);
                rpgScene = (RPGScene*)scene;
            }

            void finish() {
                Script::finish();
            }

			~RPGEventTrigger() {
                if (!isFinished) {
                    if (cutscene) {
                        delete cutscene;
                    }
                }
            }
    };

    class RPGEventTriggerTileXY: public RPGEventTrigger {
        public:
            Walker* walker;

            int x;
            int y;

            RPGEventTriggerTileXY(Walker* gWalker, RPGCutscene* gScene, int gx, int gy) {
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

            
    };

    class RPGEventTriggerTileX: public RPGEventTrigger {
        public:
            Walker* walker;

            int x;

            RPGEventTriggerTileX(Walker* gWalker, RPGCutscene* gScene, int gx) {
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
    };

    class RPGEventTriggerTileY: public RPGEventTrigger {
        public:
            Walker* walker;

            int y;

            RPGEventTriggerTileY(Walker* gWalker, RPGCutscene* gScene, int gy) {
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
    };

	class RPGEventTriggerXY: public RPGEventTrigger {
	public:
		Entity* watched = nullptr;

		FloatRect checkBox;

		int x;
		int y;

		RPGEventTriggerXY(RPGCutscene* gCutscene, int gx, int gy) {
			cutscene = gCutscene;
			x = gx;
			y = gy;
		}
		RPGEventTriggerXY(Entity* gWatched, RPGCutscene* gCutscene, int gx, int gy): RPGEventTriggerXY(gCutscene, gx, gy) {
			watched = gWatched;
		}

		void prepare() {
			if (watched == nullptr) watched = parent;
			checkBox = {
				x*TILE_WIDTH, y*TILE_HEIGHT,
				TILE_WIDTH, TILE_HEIGHT
			};
		}

		void script() {
			if (rpgScene->inState("duration")) {
				FloatVector2 point = { watched->x, watched->y };
				if (overlapping(&point, &checkBox)) {
					rpgScene->startCutscene(cutscene);
					if (finishOnTrigger) finish();
                    else cutscene->deleteOnFinish = false;
				}
			}
		}
	};

	class RPGEventTriggerBoundary: public RPGEventTrigger {
	public:
		Entity* watched = nullptr;

		float point;
		Direction dir = NoDir;

		RPGEventTriggerBoundary(RPGCutscene* gCutscene, float gp, Direction gDir) {
			cutscene = gCutscene;
			point = gp;
			dir = gDir;
		}
		RPGEventTriggerBoundary(Entity* gWatched, RPGCutscene* gCutscene, float gp, Direction gDir): RPGEventTriggerBoundary(gCutscene, gp, gDir) {
			watched = gWatched;
		}

		void prepare() {
			if (watched == nullptr) watched = parent;
		}

		void script() {
			if (rpgScene->inState("duration")) {
				if ((dir == Left && watched->x <= point) || (dir == Right && watched->x >= point) || (dir == Up && watched->y <= point) || (dir == Down && watched->y >= point)) {
					rpgScene->startCutscene(cutscene);
					if (finishOnTrigger) finish();
				}
			}
		}
	};

    class RPGEventTriggerRectangle: public RPGEventTrigger {
    public:
        Entity* watched = nullptr;

        FloatRect rect;

        RPGEventTriggerRectangle(RPGCutscene* gCutscene, FloatRect gRect) {
			cutscene = gCutscene;
            rect = gRect;
		}
		RPGEventTriggerRectangle(Entity* gWatched, RPGCutscene* gCutscene, FloatRect gRect): RPGEventTriggerRectangle(gCutscene, gRect) {
			watched = gWatched;
		}

        void prepare() {
            if (watched == nullptr) watched = parent;
        }

        void script() {
            if (rpgScene->inState("duration")) {
                if (overlapping(watched->x, watched->y, &rect)) {
                    rpgScene->startCutscene(cutscene);
					if (finishOnTrigger) finish();
                }
            }
        }
    };
}