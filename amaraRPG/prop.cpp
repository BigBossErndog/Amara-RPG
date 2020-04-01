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

            Prop(int gx, int gy, std::string tKey): Amara::Sprite(gx, gy, tKey) {}
            Prop(): Amara::Sprite() {}

            void configure(Amara::Area* gArea) {
                area = gArea;
            }

            void configure(Amara::Area* gArea, nlohmann::json& config) {
                configure(gArea);

                if (config.find("id") != config.end()) {
                    id = config["id"];
                }
                if (config.find("texture") != config.end()) {
                    setTexture(config["texture"]);
                }

                if (config.find("tileX") != config.end()) {
                    tileX = config["tileX"];
                }
                if (config.find("tileY") != config.end()) {
                    tileY = config["tileY"];
                }
                snapToTile();

                if (config.find("originX") != config.end()) {
                    originX = config["originX"];
                }
                if (config.find("originY") != config.end()) {
                    originY = config["originY"];
                }
                if (config.find("isWall") != config.end()) {
                    isWall = config["isWall"];
                }
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