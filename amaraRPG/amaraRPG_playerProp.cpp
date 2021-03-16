#pragma once
#ifndef AMARA_PLAYER
#define AMARA_PLAYER

#include "amaraRPG.h"

namespace Amara {
    class PlayerProp: public Amara::Walker {
        public:
            Amara::Direction lastWalkDir = NoDir;
            Amara::Direction walkBuffer = NoDir;

            bool controlsEnabled = true;
            bool allowRunning = false;

            PlayerProp(int gx, int gy, std::string tKey): Amara::Walker(gx, gy, tKey) {
                isPlayer = true;
            }
            PlayerProp(): Amara::Walker() {
                isPlayer = true;
            }
            PlayerProp(nlohmann::json config): PlayerProp() {
                configure(config);
            }

            virtual void configure(nlohmann::json config) {
                Amara::Walker::configure(config);
                if (config.find("controlsEnabled") != config.end()) {
                    controlsEnabled = config["controlsEnabled"];
                }
                if (config.find("allowRunning") != config.end()) {
                    allowRunning = config["allowRunning"];
                }
            }

            virtual nlohmann::json toData() {
                nlohmann::json config = Amara::Walker::toData();
                config["controlsEnabled"] = controlsEnabled;
                config["allowRunning"] = allowRunning;
                return config;
            }

            void handleWalking() {
				Amara::Walker::handleWalking();
                if (controlsEnabled && rpgScene->sm.inState("duration") && !scene->transition) {
                    Amara::Direction verDir = NoDir;
                    Amara::Direction horDir = NoDir;
                    Amara::Direction lastPressDir = NoDir;

                    if (controls->isDown("up") && !controls->isDown("down")) {
                        if (controls->justDown("up")) {
                            lastPressDir = Up;
                            if (isBusy()) {
                                walkBuffer = Up;
                            }
                        }
                        verDir = Up;
                    }

                    if (controls->isDown("down") && !controls->isDown("up")) {
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

                    bool shouldRun = allowRunning && controls->isDown("run");

                    if (walkBuffer != NoDir) {
                        if (!isBusy()) {
                            if (!((shouldRun && run(walkBuffer)) || walk(walkBuffer))) {
                                face(walkBuffer);
                            }
                            lastWalkDir = walkBuffer;
                            walkBuffer = NoDir;
                        }
                    }
                    else {
                        if (lastPressDir != NoDir) {
                            if (verDir == lastPressDir) {
                                if (((shouldRun && run(verDir)) || walk(verDir))) {
                                    lastWalkDir = verDir;
                                }
                                else {
                                    face(verDir);
                                }
                            }
                            else if (horDir == lastPressDir) {
                                if (((shouldRun && run(horDir)) || walk(horDir))) {
                                    lastWalkDir = horDir;
                                }
                                else {
                                    face(horDir);
                                }
                            }
                        }
                        else {
                            if (verDir != NoDir && verDir == direction && ((shouldRun && run(verDir)) || walk(verDir))) {
                                lastWalkDir = verDir;
                            }
                            else if (horDir != NoDir && horDir == direction && ((shouldRun && run(horDir)) || walk(horDir))) {
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

            void enableControls() {
                controlsEnabled = true;
            }
            void disableControls() {
                controlsEnabled = false;
            }

            bool interactWith(int ix, int iy) {
                if (!isActive) return false;
                if (!controls->justDown("interact")) return false;

                if (rpgScene->sm.inState("duration") && !scene->transition) {
                    int ox = Amara::getOffsetX(direction);
                    int oy = Amara::getOffsetY(direction);
                    int tx = tileX + ox;
                    int ty = tileY + oy;

                    if (tx == ix && ty == iy) {
                        return true;
                    }
                }

                return false;
            }

            bool interactWith(Amara::FloatRect rect) {
                for (int i = rect.x; i < rect.x + rect.width; i++) {
                    for (int j = rect.y; j < rect.y + rect.height; i++) {
                        if (interactWith(i, j)) {
                            return true;
                        }
                    }
                }
                return false;
            }

            bool interactWith(Amara::Prop* prop) {
                if (prop == nullptr) return false;
                if (!prop->isActive) return false;
                for (int i = prop->tileX - prop->tilePaddingLeft; i <= prop->tileX + prop->tilePaddingRight; i++) {
                    for (int j = prop->tileY - prop->tilePaddingTop; j <= prop->tileY + prop->tilePaddingBottom; j++) {
                        if (interactWith(i, j)) {
                            return true;
                        }
                    }
                }
                return false;
            }
    };
}

#endif
