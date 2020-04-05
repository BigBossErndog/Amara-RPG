#ifndef AMARA_AREA
#define AMARA_AREA

#include "amaraRPG.h"

namespace Amara {
    class RPGScene : public Amara::Scene {
        public:
            std::deque<Amara::Prop*> props;
            Amara::Tilemap* tilemap = nullptr;

            nlohmann::json config;

            Amara::StateManager sm;

            RPGScene() {}

            void create() {
                sm.reset();
                props.clear();

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
                    
                }
            }

            void onStop() {
                props.clear();
            }
            
            virtual void onPrepare() {}
            virtual void onCreate() {}
            virtual void onUpdate() {}
            virtual void onDuration() {}
            virtual void handleInteracts() {}
            virtual void handleEvents() {}

            Amara::Prop* addProp(Amara::Prop* prop) {
                props.push_back(prop);
                prop->configure(this);
                add(prop);
                return prop;
            }

            Amara::Prop* addProp(Amara::Prop* prop, nlohmann::json config) {
                addProp(prop);
                prop->configure(this, config);
                return prop;
            }

            Amara::Prop* getPropAt(int tx, int ty) {
                for (Amara::Prop* prop: props) {
                    if (prop->isActive && prop->tileX == tx && prop->tileY == ty) {
                        return prop;
                    }
                }
                return nullptr;
            }

            Amara::TilemapLayer* getLayer(std::string layerKey) {
                return tilemap->getLayer(layerKey);
            }

            bool isWall(int tx, int ty) {
                Amara::Prop* prop = getPropAt(tx, ty);
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
    };
}

#endif