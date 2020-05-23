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

            int tilePaddingTop = 0;
            int tilePaddingBottom = 0;
            int tilePaddingLeft = 0;
            int tilePaddingRight = 0;

            int tileOffsetX = TILE_WIDTH/2;
            int tileOffsetY = TILE_HEIGHT/2;

            bool isWall = true;
            bool isWalker = false;
            bool isPlayer = false;

            bool depthLocked = false;

            Prop(int gx, int gy, std::string tKey): Amara::Sprite(0, 0, tKey) {
                tileX = gx;
                tileY = gy;
                snapToTile();
            }
            Prop(std::string tKey): Amara::Sprite(tKey) {}
            Prop(): Amara::Sprite() {}
            Prop(nlohmann::json config): Prop() {
                configure(config);
            }

            virtual void init(Amara::GameProperties* gameProperties, Amara::Scene* givenScene, Amara::Entity* givenParent) override {
                Amara::Sprite::init(gameProperties, givenScene, givenParent);
                data["isProp"] = true;
                rpgScene = (RPGScene*) givenScene;
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
                if (config.find("tilePaddingTop") != config.end()) {
                    tilePaddingTop = config["tilePaddingTop"];
                }
                if (config.find("tilePaddingBottom") != config.end()) {
                    tilePaddingBottom = config["tilePaddingBottom"];
                }
                if (config.find("tilePaddingLeft") != config.end()) {
                    tilePaddingTop = config["tilePaddingLeft"];
                }
                if (config.find("tilePaddingRight") != config.end()) {
                    tilePaddingTop = config["tilePaddingRight"];
                }
                snapToTile();

                if (config.find("isWall") != config.end()) {
                    isWall = config["isWall"];
                }
            }

            virtual nlohmann::json toData() {
                nlohmann::json config = Amara::Sprite::toData();
                config["tileX"] = tileX;
                config["tileY"] = tileY;
                config["tileOffsetX"] = tileOffsetX;
                config["tileOffsetY"] = tileOffsetY;
                config["tilePaddingTop"] = tilePaddingTop;
                config["tilePaddingBottom"] = tilePaddingBottom;
                config["tilePaddingLeft"] = tilePaddingLeft;
                config["tilePaddingRight"] = tilePaddingRight;
                config["isWall"] = isWall;
                return config;
            }

            void create() {
                snapToTile();
            }

            void update() {
                if (!depthLocked) {
                    depth = y;
                }
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

            void setTilePadding(int pt, int pb, int pl, int pr) {
                tilePaddingTop = pt;
                tilePaddingBottom = pb;
                tilePaddingLeft = pl;
                tilePaddingRight = pr;
            }
            void setTilePadding(int pt, int pl) {
                setTilePadding(pt, pt, pl, pl);
            }
            void setTilePadding(int pt) {
                setTilePadding(pt, pt);
            }

            bool covers(int gx, int gy) {
                for (int i = tileX - tilePaddingLeft; i <= tileX + tilePaddingRight; i++) {
                    for (int j = tileY - tilePaddingTop; j <= tileY + tilePaddingBottom; j++) {
                        if (i == gx && j == gy) {
                            return true;
                        }
                    }
                }
                return false;
            }
    };
}

#endif