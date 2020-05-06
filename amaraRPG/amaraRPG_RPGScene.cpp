#pragma once
#ifndef AMARA_AREA
#define AMARA_AREA

#include "amaraRPG.h"

namespace Amara {
    class RPGScene : public Amara::Scene, public Amara::WallFinder {
        public:
            std::deque<Amara::Prop*> props;
            Amara::Tilemap* tilemap = nullptr;

            std::deque<Amara::CutsceneBase*> cutscenes;
            Amara::CutsceneBase* currentCutscene = nullptr;

            nlohmann::json config;

            Amara::StateManager sm;

            RPGScene() {}

            void preload() {
                onPreload();
            }

            void create() {
                sm.reset();
                props.clear();

                for (Amara::CutsceneBase* cutscene: cutscenes) {
                    delete cutscene;
                } 
                cutscenes.clear();

                onPrepare();
                add(tilemap = new Amara::Tilemap());

                if (!config.empty()) {
                    if (config.find("map_texture") != config.end()) {
                        tilemap->setTexture(config["map_texture"]);
                    }
                    if (config.find("map_json") != config.end()) {
                        tilemap->setTiledJson(config["map_json"]);
                        tilemap->createAllLayers();
                    }

                    Amara::TilemapLayer* layer;

                    if (config.find("map_hiddenLayers") != config.end()) {
                        for (std::string layerKey: config["map_hiddenLayers"]) {
                            layer = getLayer(layerKey);
                            if (layer) {
                                layer->setVisible(false);
                            }
                        }
                    }

                    int aboveDepth = tilemap->heightInPixels * 2;
                    if (config.find("map_aboveLayersDepth") != config.end()) {
                        aboveDepth = config["map_aboveLayersDepth"];
                    }
                    if (config.find("map_aboveLayers") != config.end()) {
                        for (std::string layerKey: config["map_aboveLayers"]) {
                            layer = getLayer(layerKey);
                            if (layer) {
                                layer->depth = aboveDepth;
                            }
                        }
                    }

                    if (config.find("map_wallLayers") != config.end()) {
                        tilemap->setWalls(config["map_wallLayers"]);
                    }

                    TILE_WIDTH = tilemap->tileWidth;
                    TILE_HEIGHT = tilemap->tileHeight;
                }



                onCreate();
            }

            void update() {
                onUpdate();
                if (sm.state("start")) {
                    sm.switchState("duration");
                }
                else if (sm.state("duration")) {
                    onDuration();
                }
                else if (sm.state("cutscenes")) {
                    if (currentCutscene != nullptr) {
                        currentCutscene->script();
                        currentCutscene->script(this);
                    }

                    if (currentCutscene == nullptr || currentCutscene->finished) {
                        if (currentCutscene != nullptr) {
                            if (currentCutscene->deleteOnFinish) {
                                delete currentCutscene;
                            }
                        }

                        cutscenes.pop_back();
                        if (cutscenes.size() > 0) {
                            currentCutscene = cutscenes.back();
                        }
                        else {
                            currentCutscene = nullptr;
                            sm.switchState("duration");
                        }
                    }
                }
                
                Amara::Prop* prop;
                for (std::deque<Amara::Prop*>::iterator it = props.begin(); it != props.end(); it++) {
                    prop = *it;
                    if (prop->isDestroyed || prop->rpgScene != this) {
                        props.erase(it--);
                    }
                }
            }
            
            virtual void onPreload() {}
            virtual void onPrepare() {}
            virtual void onCreate() {}
            virtual void onUpdate() {}
            virtual void onDuration() {}
            virtual void handleInteracts() {}
            virtual void handleEvents() {}

            Amara::Prop* addProp(Amara::Prop* prop) {
                props.push_back(prop);
                prop->configure(this);
                Amara::Scene::add(prop);
                return prop;
            }

            Amara::Prop* addProp(Amara::Prop* prop, nlohmann::json config) {
                addProp(prop);
                prop->configure(this, config);
                return prop;
            }

            Amara::Prop* getPropAt(int tx, int ty, Amara::Prop* exclusion) {
                for (Amara::Prop* prop: props) {
                    if (!prop->isActive || prop->isDestroyed) continue;
                    if (prop == exclusion) continue;

                    if (tx < prop->tileX - prop->tilePaddingLeft) {
                        continue;
                    }
                    if (tx > prop->tileX + prop->tilePaddingRight) {
                        continue;
                    }
                    if (ty < prop->tileY - prop->tilePaddingTop) {
                        continue;
                    }
                    if (ty > prop->tileY + prop->tilePaddingBottom) {
                        continue;
                    }

                    return prop;
                }
                return nullptr;
            }
            
            Amara::Prop* getPropAt(int tx, int ty) {
                return getPropAt(tx, ty, nullptr);
            }

            Amara::TilemapLayer* getLayer(std::string layerKey) {
                return tilemap->getLayer(layerKey);
            }

            bool isWall(int tx, int ty, Amara::Prop* propExclusion) {
                Amara::Prop* prop = getPropAt(tx, ty, propExclusion);
                if (prop != nullptr) {
                    if (prop->isActive && prop->isWall) {
                        return true;
                    }
                }

                Amara::TilemapLayer* layer;
                if (config.find("map_wallLayers") != config.end()) {
                    for (std::string layerKey: config["map_wallLayers"]) {
                        layer = getLayer(layerKey);
                        if (layer && layer->getTileAt(tx, ty).id != -1) {
                            return true;
                        }
                    }
                }

                return false;
            }

            bool isWall(int tx, int ty) {
                return isWall(tx, ty, nullptr);
            }

            Amara::CutsceneBase* startCutscene(Amara::CutsceneBase* cutscene) {
                cutscene->init(properties);
                cutscene->prepare();
                cutscene->prepare(this);

                cutscenes.push_back(cutscene);
                currentCutscene = cutscene;

                sm.switchState("cutscenes");
            }

            bool inCutscene() {
                return sm.inState("cutscene");
            }

            virtual int getMapWidth() {
                return tilemap->getMapWidth();
            }
            virtual int getMapHeight() {
                return tilemap->getMapHeight();
            }
    };
}

#endif