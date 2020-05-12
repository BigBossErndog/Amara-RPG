#pragma once
#ifndef AMARA_WALKER
#define AMARA_WALKER

#include "amaraRPG.h"

namespace Amara {
    class Walker: public Amara::Prop {
        public:
            Direction direction = Down;
            Direction walkDirection = NoDir;

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

            virtual void configure(nlohmann::json config) {
                Amara::Prop::configure(config);
                if (config.find("direction") != config.end()) {
                    direction = config["direction"];
                }
                if (config.find("walkSpeed") != config.end()) {
                    walkSpeed = config["walkSpeed"];
                }
                if (config.find("runSpeed") != config.end()) {
                    runSpeed = config["runSpeed"];
                }
            }

            virtual nlohmann::json toData() {
                nlohmann::json config = Amara::Prop::toData();
                config["direction"] = direction;
                config["walkSpeed"] = walkSpeed;
                config["runSpeed"] = runSpeed;
                return config;
            }

            void update() {
                if (justFinishedWalking && !isBusy()) {
                    play(standAnim(direction));
                }
                justFinishedWalking = false;
                handleWalking();
                Amara::Prop::update();
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

                if (rpgScene->isWall(tileX + ox, tileY + oy)) {
                    if (pathTask != nullptr) {
                        delete pathTask;
                        pathTask = nullptr;
                    }
                    return false;
                }

                return true;
            }

            bool walk(Amara::Direction dir, bool replaceAnim) {
                if (!canWalk(dir)) {
                    return false;
                }

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

                if (replaceAnim) {
                    play(Amara::walkAnim(dir));
                }

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
                    if (replaceAnim) play(Amara::runAnim(dir));
                    return true;
                }

                return false;
            }
            
            bool run(Amara::Direction dir) {
                return run(dir, true);
            }

            bool walkTo(int gx, int gy) {
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
                            if (!walk(currentPathTile.direction)) {
                                delete pathTask;
                                pathTask = nullptr;
                                return false;
                            }
                        }
                    }
                }
                if (pathTask == nullptr) {
                    pathTask = new Amara::PathFindingTask(rpgScene->tilemap);
                    pathTask->from(tileX, tileY)->to(gx, gy)->start();
                }
                return false;
            }

            void face(Amara::Direction dir) {
                if (isBusy()) return;
                if (dir == NoDir) return;
                direction = dir;
                walkDirection = NoDir;
                play(standAnim(dir));
            }

            bool isFacing(Amara::Direction dir) {
                return direction == dir;
            }

            bool isFacing() {

            }

            void faceTowards() {
                
            }

            virtual void handleWalking() {
                if (walkDirection != NoDir) {
                    int sx = x;
                    int sy = y;
                    int ox = Amara::getOffsetX(walkDirection);
                    int oy = Amara::getOffsetY(walkDirection);
                    int tx = tileX * TILE_WIDTH + tileOffsetX;
                    int ty = tileY * TILE_HEIGHT + tileOffsetY;

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
    };

    typedef Walker NPC;
}

#endif