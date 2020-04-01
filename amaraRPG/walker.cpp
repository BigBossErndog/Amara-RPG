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
            float runSpeed = 2;

            Walker(int gx, int gy, std::string tKey): Amara::Prop(gx, gy, tKey) {
                isWalker = true;
            }
            Walker(): Amara::Prop() {
                isWalker = true;
            }

            void configure(Amara::Area* gArea, nlohmann::json& config) {
                Amara::Prop::configure(gArea, config);
                if (config.find("dir") != config.end()) {
                    direction = config["dir"];
                }
            }

            void update() {
                handleWalking();
            }

            bool walk(Amara::Direction dir, bool replaceAnim) {
                if (isBusy()) {
                    return false;
                }
                snapToTile();

                int ox = Amara::getOffsetX(dir);
                int oy = Amara::getOffsetY(dir);
                tileX += ox;
                tileY += oy;

                walkDirection = NoDir;

                Amara::Prop* block = area->getPropAt(tileX + ox, tileY + oy);
                if (block != nullptr && block->isWall) {
                    return false;
                }

                walkDirection = dir;
                movementSpeed = walkSpeed;
                if (replaceAnim) {
                    play(walkAnim(dir));
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

            bool run(Amara::Direction dir) {
                if (walk(dir, false)) {
                    movementSpeed = runSpeed;
                    play(runAnim(dir));
                    return true;
                }

                return false;
            }

            bool isBusy() {
                if (walkDirection != NoDir) {
                    return true;
                }
                return false;
            }

            void face(Amara::Direction dir) {
                direction = dir;
                walkDirection = NoDir;
            }

            void handleWalking() {
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
                            }
                        }
                    }
                    
                }
            }
    };
}

#endif