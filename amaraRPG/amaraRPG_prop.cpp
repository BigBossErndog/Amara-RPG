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

            float tileOffsetX = Amara::TILE_WIDTH/2;
            float tileOffsetY = Amara::TILE_HEIGHT/2;

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
			Prop(int gx, int gy): Amara::Sprite(0, 0) {
				tileX = gx;
				tileY = gy;
				snapToTile();
			}

            virtual void init(Amara::GameProperties* gameProperties, Amara::Scene* givenScene, Amara::Entity* givenParent) override {
				rpgScene = (RPGScene*) givenScene;
                Amara::Sprite::init(gameProperties, givenScene, givenParent);
                data["isProp"] = true;
				snapToTile();
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
                    tilePaddingLeft = config["tilePaddingLeft"];
                }
                if (config.find("tilePaddingRight") != config.end()) {
                    tilePaddingRight = config["tilePaddingRight"];
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

            void run() {
				Amara::Sprite::run();
                if (!depthLocked) {
                    depth = y;
                }
            }

            void goTo(int gx, int gy) {
                tileX = gx;
                tileY = gy;
                snapToTile();
            }

			void setTileX(int gx) {
				tileX = gx;
				snapTileX();
			}

			void setTileY(int gy) {
				tileY = gy;
				snapTileY();
			}

			void setTileOffset(float gx, float gy) {
				tileOffsetX = gx;
				tileOffsetY = gy;
				snapToTile();
			}
			void setTileOffset(float go) {
				setTileOffset(go, go);
			}

            void snapToTile() {
                x = tileX * Amara::TILE_WIDTH + tileOffsetX;
                y = tileY * Amara::TILE_HEIGHT + tileOffsetY;
            }

			void snapTileX() {
				x = tileX * Amara::TILE_WIDTH + tileOffsetX;
			}

			void snapTileY() {
				y = tileY * Amara::TILE_HEIGHT + tileOffsetY;
			}

			bool isOn(float gx, float gy) {
				if (round(x) == round(gx * Amara::TILE_WIDTH + tileOffsetX)) {
					if (round(y) == round(gy * Amara::TILE_HEIGHT + tileOffsetY)) {
						return true;
					}
				}
				return false;
			}

			bool isOnX(float gx) {
				if (round(x) == round(gx * Amara::TILE_WIDTH + tileOffsetX)) {
					return true;
				}
				return false;
			}
			bool isOnY(float gy) {
				if (round(y) == round(gy * Amara::TILE_HEIGHT + tileOffsetY)) {
					return true;
				}
				return false;
			}

            Amara::Direction getDirectionTo(Amara::Prop* other, std::vector<Amara::Direction> list) {
                int objx = other->tileX;
                int objy = other->tileY;

                if (tileX == objx && tileY == objy) {
                    return Down;
                }

                return getDirectionBetween(tileX, tileY, objx, objy, list);
            }
			Amara::Direction getDirectionTo(Amara::Prop* other) {
				return getDirectionTo(other, DirectionsInOrder);
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

            bool isNextTo(int gx, int gy, bool diagonal) {
                for (int i = tileX-tilePaddingLeft-1; i <= tileX+tilePaddingRight+1; i++) {
                    for (int j = tileY-tilePaddingTop-1; j <= tileY+tilePaddingBottom+1; j++) {
                        if (!diagonal && i == j) continue;
                        if (i == gx && j == gy) {
                            return true;
                        }
                    }
                }
                return false;
            }
            bool isNextTo(int gx, int gy) {
                return isNextTo(gx, gy, false);
            }

            bool isNextTo(Amara::Prop* other, bool diagonal) {
                return isNextTo(other->tileX, other->tileY, diagonal);
            }
            bool isNextTo(Amara::Prop* other) {
                return isNextTo(other, false);
            }
    };
}

#endif
