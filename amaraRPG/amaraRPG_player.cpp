#pragma once
#ifndef AMARA_PLAYER
#define AMARA_PLAYER

#include "amaraRPG.h"

namespace Amara {
    class Player: public Amara::Walker {
        public:
            Amara::Direction lastWalkDir = NoDir;
            Amara::Direction walkBuffer = NoDir;
            Amara::Direction faceDir = NoDir;
            bool controlsEnabled = true;

            Player(int gx, int gy, std::string tKey): Amara::Walker(gx, gy, tKey) {
                isPlayer = true;
            }
            Player(): Amara::Walker() {
                isPlayer = true;
            }
            Player(nlohmann::json config): Player() {
                configure(config);
            }

            virtual void configure(nlohmann::json config) {
                Amara::Walker::configure(config);
                if (config.find("controlsEnabled") != config.end()) {
                    controlsEnabled = config["controlsEnabled"];
                } 
            }

            virtual nlohmann::json toData() {
                nlohmann::json config = Amara::Walker::toData();
                config["controlsEnabled"] = controlsEnabled;
                return config;
            }

            void handleWalking() {
                Amara::Walker::handleWalking();
                if (controlsEnabled && rpgScene->sm.inState("duration")) {
                    Amara::Direction preDir = NoDir;
                    
                    if (controls->isDown("up")) {
                        if (isBusy() && controls->justDown("up")) {
                            walkBuffer = Up;
                        }
                        if (preDir == NoDir || preDir == lastWalkDir) {
                            preDir = Up;
                        }
                    }

                    if (controls->isDown("down")) {
                        if (isBusy() && controls->justDown("down")) {
                            walkBuffer = Down;
                        }
                        if (preDir == NoDir || preDir == lastWalkDir) {
                            preDir = Down;
                        }
                    }

                    if (controls->isDown("left")) {
                        if (isBusy() && controls->justDown("left")) {
                            walkBuffer = Left;
                        }
                        if (preDir == NoDir || preDir == lastWalkDir) {
                            preDir = Left;
                        }
                    }

                    if (controls->isDown("right")) {
                        if (isBusy() && controls->justDown("right")) {
                            walkBuffer = Right;
                        }
                        if (preDir == NoDir || preDir == lastWalkDir) {
                            preDir = Right;
                        }
                    }

                    if (walkBuffer != NoDir && !isBusy()) {
                        if (walk(walkBuffer)) {
                            lastWalkDir = walkBuffer;
                        }
                        else if (faceDir == NoDir) {
                            face(walkBuffer);
                            faceDir = walkBuffer;
                        }
                        walkBuffer = NoDir;
                        lastWalkDir = walkBuffer;
                    }
                    else if (preDir != NoDir) {
                        if (walk(preDir)) {
                            lastWalkDir = preDir;
                        }
                        else {
                            if (faceDir == NoDir) {
                                face(preDir);
                                faceDir = preDir;
                            }
                            if (!isBusy()) {
                                lastWalkDir = preDir;
                            }
                        }
                    }
                    else {
                        faceDir = NoDir;
                    }
                }
            }

            void enableControls() {
                controlsEnabled = true;
            }
            void disableControls() {
                controlsEnabled = false;
            }

            bool interactWith(Amara::Prop* prop) {
                if (!isActive) return false;
                if (!prop->isActive) return false;
                if (!controls->justDown("interact")) return false;

                if (rpgScene->sm.inState("duration")) {
                    int ox = Amara::getOffsetX(direction);
                    int oy = Amara::getOffsetY(direction);
                    int tx = tileX + ox;
                    int ty = tileY + oy;
                    
                    if (tx == prop->tileX && ty == prop->tileY) {
                        return true;
                    }
                }

                return false;
            }

            bool interactWith(int ix, int iy) {
                if (!isActive) return false;
                if (isBusy()) return false;

                int ox = Amara::getOffsetX(direction);
                int oy = Amara::getOffsetY(direction);
                int tx = tileX + ox;
                int ty = tileY + oy;

                if (controls->justDown("interact") && tx == ix && ty == iy) {
                    return true;
                }

                return false;
            }
    };
}

#endif