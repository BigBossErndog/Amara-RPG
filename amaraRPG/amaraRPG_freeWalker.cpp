#pragma once

#include "amaraRPG.h"

namespace Amara {
    class FreeWalker: public Amara::Prop {
    public:
        int walkDirections = 0;

        float movementSpeed = 1;
        float walkSpeed = 1;
        float runSpeed = 4;

        bool justFinishedWalking = false;

        Amara::PathFindingTask* pathTask = nullptr;
        Amara::PathTile currentPathTile;

        FreeWalker(int gx, int gy, std::string tKey): Amara::Prop(gx, gy, tKey) {
            isWalker = true;
        }
        FreeWalker(): Amara::Prop() {
            isWalker = true;
        }
        FreeWalker(nlohmann::json config): FreeWalker() {
            configure(config);
        }
        FreeWalker(int gx, int gy): Amara::Prop(gx, gy) {
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

        bool isBusy() {
            if (walkDirection != NoDir) {
                return true;
            }
            return false;
        }


        void run() {
            if (walkDirections) {
                justFinishedWalking = true;
            }
            walkDirections = 0;

            movementSpeed = (controls->isDown("run")) ? runSpeed : walkSpeed;

            float ox = 0, oy = 0;

            if (controls->isDown("up") && !controls->isDown("down")) {
                walkDirections += Up;
                oy += movementSpeed;
            }
            if (controls->isDown("down") && !controls->isDown("up")) {
                walkDirections += Down;
                oy -= movementSpeed;
            }
            if (controls->isDown("left") && !controls->isDown("right")) {
                walkDirections += Left;
                ox -= movementSpeed;
            }
            if (controls->isDown("right") && !controls->isDown("left")) {
                walkDirections += Right;
                ox += movementSpeed;
            }

            if (abs(ox) && abs(oy)) {
                ox = ox/abs(ox) * sqrt(pow(abs(walkSpeed),2)/2);
                oy = oy/abs(oy) * sqrt(pow(abs(walkSpeed),2)/2);
            }

            if (physics) {
                physics->velocityX += ox;
                physics->velocityY += oy;
            }
            else {
                x += ox;
                y += oy;
            }

            if (walkDirections) {
                justFinishedWalking = false;
            }
        }

        bool isWalking() {
            return (bool)walkDirections; 
        }
    }
}