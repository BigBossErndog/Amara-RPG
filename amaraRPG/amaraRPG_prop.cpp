#ifndef AMARA_PROP
#define AMARA_PROP

#include "amaraRPG.h"

namespace Amara {
    class Area;

    class Prop: public Amara::Sprite {
        public:
            Amara::Area* area = nullptr;
            int tileX = 0;
            int tileY = 0;

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

            virtual void configure(Amara::Area* gArea) {
                area = gArea;
            }

            virtual void configure(Amara::Area* gArea, nlohmann::json& config) {
                configure(gArea);
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
    };
}

#endif