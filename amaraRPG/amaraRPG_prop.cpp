#pragma once
#ifndef AMARA_PROP
#define AMARA_PROP

#include "amaraRPG.h"

namespace Amara {
    class RPGScene;

    class Prop: public Amara::Sprite {
        public:
            Amara::RPGScene* rpgScene = nullptr;
            int tileX = 0;
            int tileY = 0;

            int tileSpace = 0;

            int tileOffsetX = TILE_WIDTH/2;
            int tileOffsetY = TILE_HEIGHT/2;

            bool isWall = true;
            bool isWalker = false;
            bool isPlayer = false;

            Prop(int gx, int gy, std::string tKey): Amara::Sprite(0, 0, tKey) {
                tileX = gx;
                tileY = gy;
                snapToTile();
            }
            Prop(): Amara::Sprite() {}
            Prop(nlohmann::json config): Prop() {
                configure(config);
            }

            virtual void configure(Amara::RPGScene* gScene) {
                rpgScene = gScene;
            }

            virtual void configure(nlohmann::json config) {
                Amara::Sprite::configure(config);

                if (config.find("tileX") != config.end()) {
                    tileX = config["tileX"];
                }
                if (config.find("tileY") != config.end()) {
                    tileY = config["tileY"];
                }
                if (config.find("tileOffsetX") != config.end()) {
                    tileOffsetX = config["tileOffsetX"];
                }
                if (config.find("tileOffsetY") != config.end()) {
                    tileOffsetY = config["tileOffsetY"];
                }
                if (config.find("tileSpace") != config.end()) {
                    tileSpace = config["tileSpace"];
                }
                snapToTile();

                if (config.find("isWall") != config.end()) {
                    isWall = config["isWall"];
                }
            }

            virtual void configure(Amara::RPGScene* gScene, nlohmann::json config) {
                configure(gScene);
                configure(config);
            }

            virtual nlohmann::json toData() {
                nlohmann::json config = Amara::Sprite::toData();
                config["tileX"] = tileX;
                config["tileY"] = tileY;
                config["tileOffsetX"] = tileOffsetX;
                config["tileOffsetY"] = tileOffsetY;
                config["tileSpace"] = tileSpace;
                config["isWall"] = isWall;
                return config;
            }

            void create() {
                snapToTile();
            }

            void goTo(int gx, int gy) {
                tileX = gx;
                tileY = gy;
                snapToTile();
            }

            void snapToTile() {
                x = tileX * TILE_WIDTH + tileOffsetX;
                y = tileY * TILE_HEIGHT + tileOffsetY;
            }

            Amara::Direction getDirectionTo(Amara::Prop* other) {
                int objx = other->tileX;
                int objy = other->tileY;

                if (tileX == objx && tileY == objy) {
                    return Down;
                }

                float angle = atan2(objy - tileY, objx - tileX);
                float dirNum = (int)round(angle/(M_PI/4)) % DirectionsInOrder.size();
                if (dirNum < 0) {
                    dirNum += 8;
                }
                Amara::Direction direction = Amara::DirectionsInOrder[dirNum];

                return direction;
            }
    };
}

#endif