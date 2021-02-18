#pragma once
#ifndef AMARA_WALKER
#define AMARA_WALKER

#include "amaraRPG.h"

namespace Amara {
    class Walker: public Amara::Prop, public Amara::WallFinder {
        public:
            Amara::Direction direction = Down;
            Amara::Direction walkDirection = NoDir;
            Amara::Direction bumpDir = NoDir;

            float movementSpeed = 1;
            float walkSpeed = 1;
            float runSpeed = 4;

            bool justFinishedWalking = false;

            Amara::PathFindingTask* pathTask = nullptr;
            Amara::PathTile currentPathTile;

            Walker(int gx, int gy, std::string tKey): Amara::Prop(gx, gy, tKey) {
                isWalker = true;
            }
            Walker(): Amara::Prop() {
                isWalker = true;
            }
            Walker(nlohmann::json config): Walker() {
                configure(config);
            }
			Walker(int gx, int gy): Amara::Prop(gx, gy) {
                isWalker = true;
            }

            virtual void configure(nlohmann::json config) {
                Amara::Prop::configure(config);
                if (config.find("direction") != config.end()) {
                    direction = config["direction"];
                    face(direction);
                }
                if (config.find("walkSpeed") != config.end()) {
                    walkSpeed = config["walkSpeed"];
                }
                if (config.find("runSpeed") != config.end()) {
                    runSpeed = config["runSpeed"];
                }
                if (config.find("walk") != config.end()) {
                    forceWalk(config["walk"]);
                }
            }

            virtual nlohmann::json toData() {
                nlohmann::json config = Amara::Prop::toData();
                config["direction"] = direction;
                config["walkSpeed"] = walkSpeed;
                config["runSpeed"] = runSpeed;
                return config;
            }

            void run() {
                if (justFinishedWalking && !isBusy()) {
                    std::string standAnimKey = Amara::standAnim(direction);
                    if (anims->get(standAnimKey)) play(standAnimKey);
                }
                justFinishedWalking = false;
                handleWalking();
                Amara::Prop::run();
            }

            bool isBusy() {
                if (walkDirection != NoDir) {
                    return true;
                }
                return false;
            }

            bool canWalk(Amara::Direction dir) {
                if (isBusy()) {
                    return false;
                }
                if (dir == NoDir) {
                    return false;
                }
                snapToTile();

                int ox = Amara::getOffsetX(dir);
                int oy = Amara::getOffsetY(dir);

                if (rpgScene->isWall(tileX + ox, tileY + oy, this, dir)) {
                    if (pathTask != nullptr) {
                        delete pathTask;
                        pathTask = nullptr;
                    }
                    return false;
                }

                return true;
            }

            void forceWalk(Amara::Direction dir, bool replaceAnim) {
                walkDirection = NoDir;
                direction = dir;

                walkDirection = dir;
                movementSpeed = walkSpeed;

                int ox = Amara::getOffsetX(dir);
                int oy = Amara::getOffsetY(dir);
                tileX += ox;
                tileY += oy;

                if (pathTask != nullptr) {
                    if (!pathTask->findingPath && pathTask->foundPath) {
                        if (currentPathTile.x != tileX || currentPathTile.y != tileY) {
                            delete pathTask;
                            pathTask = nullptr;
                        }
                    }
                }

                std::string walkAnimKey = Amara::walkAnim(dir);
                if (replaceAnim && anims->get(walkAnimKey)) {
                    play(walkAnimKey);
                }
            }

            void forceWalk(Amara::Direction dir) {
                forceWalk(dir, true);
            }

            void forceRun(Amara::Direction dir, bool replaceAnim) {
                forceWalk(dir);
                movementSpeed = runSpeed;
                std::string runAnimKey = Amara::runAnim(dir);
                if (replaceAnim && anims->get(runAnimKey)) play(runAnimKey);
            }

            void forceRun(Amara::Direction dir) {
                forceRun(dir, true);
            }

            bool walk(Amara::Direction dir, bool replaceAnim) {
                bumpDir = NoDir;
                if (isBusy()) return false;
                if (!canWalk(dir)) {
                    bumpDir = dir;
                    return false;
                }

                forceWalk(dir, replaceAnim);
                return true;
            }

            bool walk(Amara::Direction dir) {
                return walk(dir, true);
            }

            bool walk(Amara::Direction dir, float spd) {
                if (walk(dir)) {
                    movementSpeed = spd;
                    return true;
                }
                return false;
            }

            bool run(Amara::Direction dir, bool replaceAnim) {
                if (walk(dir, false)) {
                    movementSpeed = runSpeed;
                    std::string runAnimKey = Amara::runAnim(dir);
                    if (replaceAnim && anims->get(runAnimKey)) play(runAnimKey);
                    return true;
                }

                return false;
            }

            bool run(Amara::Direction dir) {
                return run(dir, true);
            }

            bool walkTo(int gx, int gy, bool shouldRun) {
                if (isBusy()) {
                    return false;
                }

                if (tileX == gx && tileY == gy) {
                    if (pathTask != nullptr) {
                        delete pathTask;
                    }
                    pathTask = nullptr;
                    return true;
                }

                if (pathTask != nullptr) {
                    if (pathTask->findingPath) {
                        return false;
                    }
                    else if (!pathTask->foundPath) {
                        delete pathTask;
                        pathTask = nullptr;
                        return false;
                    }
                    else if (pathTask->targetX == gx && pathTask->targetY == gy) {
                        if (tileX == gx && tileY == gy) {
                            delete pathTask;
                            pathTask = nullptr;
                            return true;
                        }
                        else {
                            currentPathTile = pathTask->dequeue();
                            while (currentPathTile.x == tileX && currentPathTile.y == tileY && currentPathTile.id > 0) {
                                currentPathTile = pathTask->dequeue();
                            }
                            if (shouldRun) {
                                if (!run(currentPathTile.direction)) {
                                    if (pathTask == nullptr) return false;
                                    delete pathTask;
                                    pathTask = nullptr;
                                    return false;
                                }
                            }
                            else {
                                if (!walk(currentPathTile.direction)) {
                                    if (pathTask == nullptr) return false;
                                    delete pathTask;
                                    pathTask = nullptr;
                                    return false;
                                }
                            }
                        }
                    }
                }
                if (pathTask == nullptr && !rpgScene->locked) {
                    pathTask = new Amara::PathFindingTask(rpgScene);
                    pathTask->from(tileX, tileY)->to(gx, gy)->start();
                }
                return false;
            }

            bool walkTo(int gx, int gy) {
                return walkTo(gx, gy, false);
            }

            bool runTo(int gx, int gy) {
                return walkTo(gx, gy, true);
            }

            void face(Amara::Direction dir) {
                if (isBusy()) return;
                if (dir == NoDir) return;
                direction = dir;
                walkDirection = NoDir;
                std::string standAnimKey = Amara::standAnim(dir);
                if (anims->get(standAnimKey)) play(standAnimKey);
            }

            bool isFacing(Amara::Direction dir) {
                return direction == dir;
            }

            bool isFacing() {

            }

            void faceTowards() {

            }

			void stopWalking() {
				if (walkDirection != NoDir) {
					tileX -= Amara::getOffsetX(walkDirection);
					tileY -= Amara::getOffsetY(walkDirection);
				}
				walkDirection = NoDir;
				bumpDir = NoDir;
				snapToTile();
			}

            virtual void handleWalking() {
                bumpDir = NoDir;
                if (walkDirection != NoDir) {
                    int sx = x;
                    int sy = y;
                    int ox = Amara::getOffsetX(walkDirection);
                    int oy = Amara::getOffsetY(walkDirection);
                    int tx = tileX * TILE_WIDTH + tileOffsetX;
                    int ty = tileY * TILE_HEIGHT + tileOffsetY;

                    if (isPlayer && rpgScene->sm.state("duration") && controls->isDown("run")) {
                        if (movementSpeed != runSpeed) {
                            if (fmod(x, runSpeed) == fmod((float)tileOffsetX, runSpeed)) {
                                if (fmod(y, runSpeed) == fmod((float)tileOffsetY, runSpeed)) {
                                    movementSpeed = runSpeed;
                                    play(Amara::runAnim(walkDirection));
                                }
                            }
                        }
                    }

                    x += ox * movementSpeed;
                    y += oy * movementSpeed;

                    if (sx < tx) {
                        if (x >= tx) {
                            x = tx;
                            if (walkDirection == UpRight) {
                                walkDirection = Up;
                            }
                            else if (walkDirection == DownRight) {
                                walkDirection = Down;
                            }
                            else {
                                walkDirection = NoDir;
                                justFinishedWalking = true;
                            }
                        }
                    }
                    else if (sx > tx) {
                        if (x <= tx) {
                            x = tx;
                            if (walkDirection == UpLeft) {
                                walkDirection = Up;
                            }
                            else if (walkDirection == DownLeft) {
                                walkDirection = Down;
                            }
                            else {
                                walkDirection = NoDir;
                                justFinishedWalking = true;
                            }
                        }
                    }

                    if (sy < ty) {
                        if (y >= ty) {
                            y = ty;
                            if (walkDirection == DownLeft) {
                                walkDirection = Left;
                            }
                            else if (walkDirection == DownRight) {
                                walkDirection = Right;
                            }
                            else {
                                walkDirection = NoDir;
                                justFinishedWalking = true;
                            }
                        }
                    }
                    else if (sy > ty) {
                        if (y <= ty) {
                            y = ty;
                            if (walkDirection == UpLeft) {
                                walkDirection = Left;
                            }
                            else if (walkDirection == UpRight) {
                                walkDirection = Right;
                            }
                            else {
                                walkDirection = NoDir;
                                justFinishedWalking = true;
                            }
                        }
                    }
                }
            }

            bool bumped(int gx, int gy) {
                if (bumpDir != NoDir && !isBusy() && rpgScene->sm.inState("duration")) {
                    int offsetX = Amara::getOffsetX(bumpDir);
                    int offsetY = Amara::getOffsetY(bumpDir);

                    for (int i = tileX - tilePaddingLeft; i <= tileX + tilePaddingRight; i++) {
                        for (int j = tileY - tilePaddingTop; j <= tileY + tilePaddingBottom; j++) {
                            if (i + offsetX == gx && j + offsetY == gy) {
                                return true;
                            }
                        }
                    }
                }
                return false;
            }

			bool bumpedX(int gx) {
				return bumped(gx, tileY);
			}

			bool bumpedY(int gy) {
				return bumped(tileX, gy);
			}

            bool bumped(Amara::Prop* prop) {
                if (bumpDir != NoDir && !isBusy() && rpgScene->sm.inState("duration")) {
                    int offsetX = Amara::getOffsetX(bumpDir);
                    int offsetY = Amara::getOffsetY(bumpDir);

                    for (int i = tileX - tilePaddingLeft; i <= tileX + tilePaddingRight; i++) {
                        for (int j = tileY - tilePaddingTop; j <= tileY + tilePaddingBottom; j++) {
                            if (prop->covers(i + offsetX, j + offsetY)) {
                                return true;
                            }
                        }
                    }
                }
                return false;
            }

            // Arguments for function pointer: Bumper, Bumpee, Direction
            bool bumped(Amara::Prop* prop, void(*callBack)(Amara::Prop*, Amara::Prop*, Amara::Direction)) {
                if (bumped(prop)) {
                    callBack(this, prop, bumpDir);
                    return true;
                }
                return false;
            }

			bool stoodOn(int ix, int iy) {
                if (isBusy()) return false;
                if (!rpgScene->sm.inState("duration") || scene->transition) return false;
                if (tileX == ix && tileY == iy) {
					return true;
				}
				return false;
            }

			bool stoodOnX(int ix) {
				return stoodOn(ix, tileY);
			}
			bool stoodOnY(int iy) {
				return stoodOn(tileX, iy);
			}

            bool stoodOn(Amara::Prop* prop) {
                if (prop == nullptr) return false;
                if (!prop->isActive) return false;

                for (int i = prop->tileX - prop->tilePaddingLeft; i <= prop->tileX + prop->tilePaddingRight; i++) {
                    for (int j = prop->tileY - prop->tilePaddingTop; j <= prop->tileY + prop->tilePaddingBottom; j++) {
                        if (stoodOn(i, j)) {
                            return true;
                        }
                    }
                }
                return false;
            }
    };

	class WalkTo: public Amara::Script {
		public:
			Walker* walker;
			float toX;
			float toY;

			Amara::Direction dirOnEnd = NoDir;

			WalkTo(float tx, float ty) {
				toX = tx;
				toY = ty;
			}

			WalkTo(float tx, float ty, Amara::Direction gDirOnEnd): WalkTo(tx, ty) {
				dirOnEnd = gDirOnEnd;
			}

			void finish() {
				if (dirOnEnd != NoDir) {
					walker->face(dirOnEnd);
				}
				Amara::Script::finish();
			}

			void prepare(Actor* actor) {
				walker = (Walker*)actor;
			}

			void script() {
				start();
				if (evt()) {
					if (walker->walkTo(toX, toY)) {
						nextEvt();
					}
				}
				finishEvt();
			}
	};
}

#endif
