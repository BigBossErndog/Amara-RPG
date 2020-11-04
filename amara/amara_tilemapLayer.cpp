#pragma once
#ifndef AMARA_TILEMAPLAYER
#define AMARA_TILEMAPLAYER

#include "amara.h"

namespace Amara {
    class Tilemap;

    int tiledGID2ID(unsigned long tilegid, unsigned long firstgid) {
        bool fhorizontal = (tilegid & Amara::TILED_FLIPPEDHORIZONTALLY) != 0;
        bool fvertical = (tilegid & Amara::TILED_FLIPPEDVERTICALLY) != 0;
        bool fdiagonal = (tilegid & Amara::TILED_FLIPPEDANTIDIAGONALLY) != 0;
        
        tilegid = tilegid & ~(Amara::TILED_FLIPPEDHORIZONTALLY | Amara::TILED_FLIPPEDVERTICALLY | Amara::TILED_FLIPPEDANTIDIAGONALLY);
        int tileId = (int)(tilegid - firstgid);

        return tileId;
    }


    class TilemapLayer: public Amara::Actor {
        public:
            SDL_Renderer* gRenderer = nullptr;
            Amara::ImageTexture* texture = nullptr;
            std::string textureKey;

            bool givenTiledJson = false;
            nlohmann::json tiledJson;
            std::string tiledJsonKey;

            std::string tiledLayerKey;

            std::vector<Amara::Tile> tiles;

            Amara::Tilemap* tilemap = nullptr;
            Amara::Entity* tilemapEntity = nullptr;
            
            int width = 0;
            int height = 0;

            float originX = 0;
            float originY = 0;

            float scaleX = 1;
            float scaleY = 1;

            SDL_Rect viewport;
            SDL_Rect srcRect;
            SDL_Rect destRect;
            SDL_Point origin;
            SDL_RendererFlip flip;

            SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
            
            int tileWidth = 0;
            int tileHeight = 0;

            int imageWidth = 0;
            int imageHeight = 0;

            int widthInPixels = 0;
            int heightInPixels = 0;

            std::unordered_map<int, Amara::TileAnimation> animations;

            TilemapLayer(float gw, float gh, float tw, float th): Amara::Actor() {
                width = gw;
                height = gh;
                tileWidth = tw;
                tileHeight = th;

                widthInPixels = width * tileWidth;
                heightInPixels = height * tileHeight;

                Amara::Tile tile;
                tiles.resize(width*height, tile);
                for (int t = 0; t < tiles.size(); t++) {
                    Tile& tile = tiles[t];
                    tile.x = (t % width);
                    tile.y = floor(((float)t) / (float)width);
                }
            }

            TilemapLayer(std::string gTextureKey, std::string gTiledJsonKey) {
                textureKey = gTextureKey;
                tiledJsonKey = gTiledJsonKey;
            }

            TilemapLayer(std::string gTextureKey, std::string gTiledJsonKey, std::string gLayerKey): TilemapLayer(gTextureKey, gTiledJsonKey) {
                tiledLayerKey = gLayerKey;
            }

            void init(Amara::GameProperties* gameProperties, Amara::Scene* givenScene, Amara::Entity* givenParent) {
                Amara::Actor::init(gameProperties, givenScene, givenParent);
                gRenderer = properties->gRenderer;

                if (!textureKey.empty()) {
                    setTexture(textureKey);
                }
                if (!tiledJsonKey.empty()) {
                    setTiledJson(tiledJsonKey);
                }
                if (!tiledLayerKey.empty()) {
                    setupTiledLayer(tiledLayerKey);
                }

                data["entityType"] = "tilemapLayer";
            }

            void setupLayer(std::string gTextureKey, std::string gTiledJsonKey, std::string gLayerKey) {
                textureKey = gTextureKey;
                tiledJsonKey = gTiledJsonKey;
                tiledLayerKey = gLayerKey;
                
                setTexture(textureKey);
                setTiledJson(tiledJsonKey);
                setupTiledLayer(gLayerKey);
            }

            void setTilemap(Amara::Tilemap* gTilemap, Amara::Entity* gTilemapEntity) {
                tilemap = gTilemap;
                tilemapEntity = gTilemapEntity;
            }

            void setTiledJson(std::string tiledJsonKey) {
                if (!tiledJsonKey.empty()) {
                    tiledJson = ((Amara::JsonFile*) load->get(tiledJsonKey))->jsonObj;
                    if (!tiledJson.empty()) {
                        givenTiledJson = true;

                        width = tiledJson["width"];
                        height = tiledJson["height"];
                        tileWidth = tiledJson["tilewidth"];
                        tileHeight = tiledJson["tileheight"];

                        widthInPixels = width * tileWidth;
                        heightInPixels = height * tileHeight;

                        Amara::Tile tile;
                        tiles.resize(width*height, tile);

                        animations.clear();
                        if (tiledJson.find("tilesets") != tiledJson.end()) {
                            for (nlohmann::json& tilesetJson: tiledJson["tilesets"]) {
                                if (tilesetJson.find("tiles") != tilesetJson.end()) {
                                    for (nlohmann::json& animData: tilesetJson["tiles"]) {
                                        int tileId = animData["id"];
                                        animations.insert(std::make_pair(tileId, Amara::TileAnimation(properties, animData)));
                                    }
                                }
                            }
                        }
                    }
                    else {
                        std::cout << "JSON with key: \"" << tiledJsonKey << "\" was not found." << std::endl;
                    }
                }
            }

            void setupTiledLayer(std::string layerKey) {
                if (!givenTiledJson) return; 
                nlohmann::json layers = tiledJson["layers"];

                unsigned long tileId;
                int firstgid = tiledJson["tilesets"][0]["firstgid"];

                bool fhorizontal;
                bool fvertical;
                bool fdiagonal;

                int numLayers = layers.size();
                for (size_t l = 0; l < numLayers; l++) {
                    if (tiledLayerKey.compare(layers[l]["name"]) != 0) continue;
                    alpha = layers[l]["opacity"];

                    for (size_t t = 0; t < layers[l]["data"].size(); t++) {
                        tileId = (unsigned long)layers[l]["data"][t];

                        bool fhorizontal = (tileId & Amara::TILED_FLIPPEDHORIZONTALLY) != 0;
                        bool fvertical = (tileId & Amara::TILED_FLIPPEDVERTICALLY) != 0;
                        bool fdiagonal = (tileId & Amara::TILED_FLIPPEDANTIDIAGONALLY) != 0;
                        
                        Amara::Tile& tile = tiles.at(t);
                        tileId = tileId & ~(Amara::TILED_FLIPPEDHORIZONTALLY | Amara::TILED_FLIPPEDVERTICALLY | Amara::TILED_FLIPPEDANTIDIAGONALLY);
                        tile.id = (int)(tileId - firstgid);
                        tile.x = (t % width);
                        tile.y = floor(((float)t) / (float)width);
                        tile.fhorizontal = fhorizontal;
                        tile.fvertical = fvertical;
                        tile.fdiagonal = fdiagonal;
                    }
                }
            }

            void setupTiledLayer(std::string tiledJsonKey, std::string gLayerKey) {
                setTiledJson(tiledJsonKey);
                setupTiledLayer(gLayerKey);
            }

            void setupTiledLayer(std::vector<int> gTiles) {
                for (size_t j = 0; j < gTiles.size(); j++) {
                    tiles.at(j).id = gTiles.at(j);
                }
            }

            bool setTexture(std::string gTextureKey) {
                Amara::Loader* load = properties->loader;
                texture = (Amara::ImageTexture*)(load->get(gTextureKey));
                if (texture != nullptr) {
                    textureKey = texture->key;

                    imageWidth = width;
                    imageHeight = height;
                    return true;
                }
                else {
                    std::cout << "Texture with key: \"" << gTextureKey << "\" was not found." << std::endl;
                }
                return false;
            }

            Amara::Tile& getTileAt(int gx, int gy) {
                int index = (gy * width) + gx;
                return tiles[index];
            }

            Amara::Tile& setTileAt(int gx, int gy, int nid) {
                Amara::Tile& tile = getTileAt(gx, gy);
                tile.id = nid;
                return tile;
            }

            Amara::Tile& getTileAtXY(float gx, float gy) {
                float px = 0;
                float py = 0;
                if (tilemapEntity) {
                    px = tilemapEntity->x;
                    py = tilemapEntity->y;
                }

                int fx = floor((gx - x - px)/tileWidth);
                int fy = floor((gy - y - py)/tileHeight);
                if (fx < 0) fx = 0;
                if (fx >= width) fx = width - 1;
                if (fy < 0) fy = 0;
                if (fy >= height) fy = height - 1;
                return getTileAt(fx, fy);
            }

            Amara::Tile& setTile(int index, int nid) {
                Amara::Tile& tile = tiles[index];
                tile.id = nid;
                return tile;
            }

            void run() {
                std::unordered_map<int, Amara::TileAnimation>::iterator it = animations.begin();

                while(it != animations.end()) {
                    it->second.update();
                    it++;
                }
                Amara::Actor::run();
            }

            void draw(int vx, int vy, int vw, int vh) {
                if (!isVisible) return;
                if (alpha < 0) alpha = 0;
                if (alpha > 1) alpha = 1;

                int tx, ty, frame, maxFrame = 0;
                float tileAngle = 0;

                viewport.x = vx;
                viewport.y = vy;
                viewport.w = vw;
                viewport.h = vh;
                SDL_RenderSetViewport(gRenderer, &viewport);

                if (texture != nullptr) {
                    SDL_Texture* tex = (SDL_Texture*)texture->asset;
                    SDL_SetTextureBlendMode(tex, blendMode);
                    SDL_SetTextureAlphaMod(tex, alpha * properties->alpha * 255);
                }

                float nzoomX = 1 + (properties->zoomX-1)*zoomFactorX*properties->zoomFactorX;
                float nzoomY = 1 + (properties->zoomY-1)*zoomFactorY*properties->zoomFactorY;

                float px = 0;
                float py = 0;
                if (tilemapEntity) {
                    px = tilemapEntity->x;
                    py = tilemapEntity->y;
                }

                Amara::Camera* cam = properties->currentCamera;
                Amara::Tile& startTile = getTileAtXY(properties->scrollX, properties->scrollY);
                Amara::Tile& endTile = getTileAtXY(properties->scrollX+(vw/nzoomX), properties->scrollY+(vh/nzoomY));
                int startX = startTile.x;
                int startY = startTile.y;
                int endX = endTile.x;
                int endY = endTile.y;

                if (startX < 0) startX = 0;
                if (startY < 0) startY = 0;
                if (endX >= width) endX = width - 1;
                if (endY >= height) endY = height - 1;

                destRect.w = ceil((tileWidth * scaleX) * nzoomX);
                destRect.h = ceil((tileHeight * scaleY) * nzoomY);
                
                for (int i = startX; i <= endX; i++) {
                    for (int j = startY; j <= endY ; j++) {
                        Amara::Tile& tile = getTileAt(i, j);
                        frame = tile.id;

                        if (frame == -1) {
                            continue;
                        }

                        tileAngle = 0;
                        flip = SDL_FLIP_NONE;
                        if (tile.fhorizontal) {
                            if (tile.fvertical) {
                                if (tile.fdiagonal) {
                                    tileAngle = 90;
                                    flip = SDL_FLIP_VERTICAL;
                                }
                                else {
                                    tileAngle = 180;
                                }
                            }
                            else if (tile.fdiagonal) {
                                tileAngle = 90;
                            }
                            else {
                                flip = SDL_FLIP_HORIZONTAL;
                            }
                        }
                        else if (tile.fvertical) {
                            if (tile.fdiagonal) {
                                tileAngle = -90;
                            }
                            else {
                                flip = SDL_FLIP_VERTICAL;
                            }
                        }
                        
                        tx = tile.x * tileWidth + px;
                        ty = tile.y * tileHeight + py;

                        bool skipDrawing = false;
                        
                        destRect.x = floor((x + tx - properties->scrollX*scrollFactorX + properties->offsetX - (originX * imageWidth)) * nzoomX);
                        destRect.y = floor((y-z + ty - properties->scrollY*scrollFactorY + properties->offsetY - (originY * imageHeight)) * nzoomY);

                        origin.x = destRect.w * originX + destRect.w/2;
                        origin.y = destRect.h * originY + destRect.h/2;

                        if (destRect.x + destRect.w <= 0) skipDrawing = true;
                        if (destRect.y + destRect.h <= 0) skipDrawing = true;
                        if (destRect.x >= vw) skipDrawing = true;
                        if (destRect.y >= vh) skipDrawing = true;
                        if (destRect.w <= 0) skipDrawing = true;
                        if (destRect.h <= 0) skipDrawing = true;

                        if (!skipDrawing) {
                            int hx, hy, hw, hh = 0;
                            hw = destRect.w;
                            hh = destRect.h;

                            if (destRect.x >= 0) {
                                hx = destRect.x + vx;
                            }
                            else {
                                hw -= -(destRect.x);
                                hx = vx;
                            }
                            if (destRect.y >= 0) {
                                hy = destRect.y + vy;
                            }
                            else {
                                hh -= -(destRect.h);
                                hy = vy;
                            }
                            if (hx + hw > vx + vw) hw = (vx - hx);
                            if (hy + hh > vy + vh) hh = (vy - hy);

                            checkForHover(hx, hy, hw, hh);
                            
                            auto got = animations.find(frame);
                            if (got != animations.end()) {
                                frame = got->second.currentTileId;
                            }

                            if (texture != nullptr) {
                                SDL_Texture* tex = (SDL_Texture*)texture->asset;
                                maxFrame = ((texture->width / tileWidth) * (texture->height / tileHeight));
                                frame = frame % maxFrame;

                                srcRect.x = (frame % (texture->width / tileWidth)) * tileHeight;
                                srcRect.y = floor(frame / (texture->width / tileWidth)) * tileHeight;
                                srcRect.w = tileWidth;
                                srcRect.h = tileHeight;

                                SDL_RenderCopyEx(
                                    gRenderer,
                                    tex,
                                    &srcRect,
                                    &destRect,
                                    angle + tileAngle,
                                    &origin,
                                    flip
                                );
                            }
                        }
                    }
                    
                    Amara::Actor::draw(vx, vy, vw, vh);
                }
            }

            void setCameraBounds(Amara::Camera* cam) {
                cam->setBounds(x, y, widthInPixels, heightInPixels);
            }

            int convertGID(unsigned long id) {
                if (!tiledJson.empty() && tiledJson.find("tilesets") != tiledJson.end()) {
                    int firstgid = tiledJson["tilesets"][0]["firstgid"];
                    return tiledGID2ID(id, firstgid);
                }
                return -1;
            }
    };
}

#endif