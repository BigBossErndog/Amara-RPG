#pragma once
#ifndef AMARA_PLAYER
#define AMARA_PLAYER

#include "amaraRPG.h"

namespace Amara {
    class Player: public Amara::Walker {
        public:
            Amara::Direction lastWalkDir = NoDir;
            Amara::Direction walkBuffer = NoDir;
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
                    Amara::Direction verDir = NoDir;
                    Amara::Direction horDir = NoDir;
                    Amara::Direction lastPressDir = NoDir;
                    
                    if (controls->isDown("up") && !controls->justDown("down")) {
                        if (controls->justDown("up")) {
                            lastPressDir = Up;
                            if (isBusy()) {
                                walkBuffer = Up;
                            }
                        }
                        verDir = Up;
                    }

                    if (controls->isDown("down") && !controls->justDown("up")) {
                        if (controls->justDown("down")) {
                            lastPressDir = Down;
                            if (isBusy()) {
                                walkBuffer = Down;
                            }
                        }
                        verDir = Down;
                    }

                    if (controls->isDown("left") && !controls->isDown("right")) {
                        if (controls->justDown("left")) {
                            lastPressDir = Left;
                            if (isBusy()) {
                                walkBuffer = Left;
                            }
                        }
                        horDir = Left;
                    }

                    if (controls->isDown("right") && !controls->isDown("left")) {
                        if (controls->justDown("right")) {
                            lastPressDir = Right;
                            if (isBusy()) {
                                walkBuffer = Right;
                            }
                        }
                        horDir = Right;
                    }

                    if (walkBuffer != NoDir && walk(walkBuffer)) {
                        lastWalkDir = walkBuffer;
                        walkBuffer = NoDir;
                    }
                    else {
                        if (!isBusy()) walkBuffer = NoDir;
                        if (lastPressDir != NoDir) {
                            if (verDir == lastPressDir) {
                                if (walk(verDir)) {
                                    lastWalkDir = verDir;
                                }
                                else {
                                    face(verDir);
                                }
                            }
                            else if (horDir == lastPressDir) {
                                if (walk(horDir)) {
                                    lastWalkDir = horDir;
                                }
                                else {
                                    face(horDir);
                                }
                            }
                        }
                        else {
                            if (verDir == lastWalkDir && walk(verDir)) {
                                
                            }
                            else if (horDir == lastWalkDir && walk(horDir)) {
                                
                            }
                            else {
                                if (verDir != NoDir && walk(verDir)) {
                                    lastWalkDir = verDir;
                                }
                                else if (horDir != NoDir && walk(horDir)) {
                                    lastWalkDir = horDir;
                                }
                                else {
                                    if (verDir != NoDir && verDir == direction) {
                                        face(verDir);
                                    }
                                    else if (horDir != NoDir && horDir == direction) {
                                        face(horDir);
                                    }
                                    else {
                                        face(verDir);
                                        face(horDir);
                                    }
                                }
                            }
                        }
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