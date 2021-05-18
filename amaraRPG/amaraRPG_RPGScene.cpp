#pragma once
#ifndef AMARA_AREA
#define AMARA_AREA

#include "amaraRPG.h"

namespace Amara {
    class RPGScene : public Amara::Scene, public Amara::WallFinder {
        public:
            Amara::Tilemap* tilemap = nullptr;
            nlohmann::json mapData;

            Amara::LightLayer* lighting = nullptr;

            std::deque<Amara::CutsceneBase*> cutscenes;
            Amara::CutsceneBase* currentCutscene = nullptr;

            std::string area;
            int floor = 0;

            Amara::StateManager sm;

            RPGScene() {}

            virtual void configure(nlohmann::json config) {
                Amara::Scene::configure(config);
                if (config.find("area") != config.end()) {
                    area = config["area"];
                }
                if (config.find("floor") != config.end()) {
                    floor = config["floor"];
                }
            }

            virtual nlohmann::json toData() {
				nlohmann::json config = Amara::Scene::toData();
                config["key"] = scenes->key;
				config["area"] = area;
                config["floor"] = floor;
				return config;
			}

            virtual void run() {
                properties->currentScene = this;

                if (!initialLoaded) {
                    if (transition != nullptr) {
                        transition->update();
                    }

                    load->run();

                    if (!load->stillLoading) {
                        if (transition != nullptr) {
                            if (transition->finished) {
                                initialLoaded = true;
                                transition->complete();
                                transition = nullptr;
                            }
                            else if (transition->waitingForPermission) {
                                transition->grantPermission();
                                setLoader(properties->loader);
                                rpgCreate();
                            }
                            else if (transition->permissionGranted) {
                                updateScene();
                            }
                        }
                        else {
                            initialLoaded = true;

                            setLoader(properties->loader);
                            rpgCreate();
                        }
                    }
                }
                else {
                    updateScene();
                    if (transition != nullptr) {
                        transition->update();
                        if (transition && transition->fromWake) {
                            if (transition->finished) {
                                transition->complete();
                                transition = nullptr;
                            }
                            else if (transition->waitingForPermission) {
                                transition->grantPermission();
                            }
                        }
                    }
                }
            }
            void updateScene() {
				reciteScripts();
                rpgUpdate();

                for (Amara::Entity* entity : entities) {
                    if (entity->isDestroyed || entity->parent != this) continue;
                    entity->run();
                }

                for (Amara::Camera* cam : cameras) {
                    if (cam->isDestroyed || cam->parent != this) continue;
                    cam->run();
                }

                afterUpdate();
            }

            virtual void rpgCreate() {
                sm.reset();

                for (Amara::CutsceneBase* cutscene: cutscenes) {
                    delete cutscene;
                }
                cutscenes.clear();
                currentCutscene = nullptr;

                lighting = nullptr;

                prepare();
                add(tilemap = new Amara::Tilemap());

                int i = 0;
                int aboveDepth = 0;

                if (!mapData.empty()) {
                    i = 0;
                    if (mapData.find("texture") != mapData.end()) {
                        tilemap->setTexture(mapData["texture"]);
                    }
                    if (mapData.find("json") != mapData.end()) {
                        tilemap->setTiledJson(mapData["json"]);
                        tilemap->createAllLayers();
                    }
                    if (mapData.find("config") != mapData.end()) {
                        tilemap->configure(mapData["config"]);
                    }

                    Amara::TilemapLayer* layer;

                    if (mapData.find("hiddenLayers") != mapData.end()) {
                        for (std::string layerKey: mapData["hiddenLayers"]) {
                            layer = getLayer(layerKey);
                            if (layer) {
                                layer->setVisible(false);
                            }
                        }
                    }

                    aboveDepth = tilemap->heightInPixels;
                    if (mapData.find("aboveLayersDepth") != mapData.end()) {
                        aboveDepth = mapData["aboveLayersDepth"];
                    }
                    i = 0;
                    if (mapData.find("aboveLayers") != mapData.end()) {
                        for (std::string layerKey: mapData["aboveLayers"]) {
                            layer = getLayer(layerKey);
                            if (layer) {
                                layer->depth = aboveDepth + i;
                                i += 1;
                            }
                        }
                    }

                    if (mapData.find("wallLayers") != mapData.end()) {
                        tilemap->setWalls(mapData["wallLayers"]);
                    }

                    Amara::TILE_WIDTH = tilemap->tileWidth;
                    Amara::TILE_HEIGHT = tilemap->tileHeight;
					if (mapData.find("setBounds") != mapData.end() && mapData["setBounds"]) {
	                    tilemap->setCameraBounds(mainCamera);
					}
                }

                create();

                if (lighting != nullptr) {
                    lighting->depth = aboveDepth + i + 1;
                }
            }

            virtual void rpgUpdate() {
                if (sm.state("start")) {
                    if (transition == nullptr) {
                        if (currentCutscene) {
                            sm.switchState("cutscenes");

                        }
                        else {
                            sm.switchState("duration");
                        }
                    }
                }
                else if (sm.state("duration")) {
                    duration();
                }
                else if (sm.state("cutscenes")) {
                    if (currentCutscene != nullptr && !currentCutscene->finished) {
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
				else if (sm.state("transition")) {
					if (!transition) {
						sm.returnState();
					}
				}
				update();
            }

			virtual Amara::SceneTransitionBase* startTransition(Amara::SceneTransitionBase* gTransition) {
                Amara::Scene::startTransition(gTransition);
				if (transition) {
					sm.switchState("transition");
				}
                return transition;
            }
            
            Amara::Prop* getPropAt(int tx, int ty, Amara::Prop* exclusion) {
                Amara::Prop* prop;
                for (Amara::Entity* entity: entities) {
                    if (entity->data.find("isProp") == entity->data.end() || !entity->data["isProp"]) {
                        continue;
                    }
                    prop = (Amara::Prop*)entity;

                    if (!prop->isActive || prop->isDestroyed) continue;
                    if (prop == exclusion) continue;

                    if (prop->covers(tx, ty)) {
                        return prop;
                    }
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
                Amara::Prop* prop;
                for (Amara::Entity* entity: entities) {
                    if (!entity->isActive || entity->isDestroyed) continue;
                    if (entity->data.find("isProp") == entity->data.end() || !entity->data["isProp"]) {
                        continue;
                    }
                    prop = (Amara::Prop*)entity;
                    if (prop == propExclusion) continue;
                    if (prop->isWall) {
                        if (!prop->covers(tx, ty)) continue;
                        return true;
                    }
                }

                return tilemap->isWall(tx, ty);
            }

            bool isWall(int tx, int ty) {
                return isWall(tx, ty, nullptr);
            }

            bool isWall(int tx, int ty, Amara::Prop* propExclusion, Amara::Direction dir) {
                Amara::Prop* prop;
                for (Amara::Entity* entity: entities) {
                    if (!entity->isActive || entity->isDestroyed) continue;
                    if (entity->data.find("isProp") == entity->data.end() || !entity->data["isProp"]) {
                        continue;
                    }
                    prop = (Amara::Prop*)entity;
                    if (prop == propExclusion) continue;
                    if (prop->isWall) {
                        if (!prop->covers(tx, ty)) continue;
                        return true;
                    }
                }

                return tilemap->isWall(tx, ty, dir);
            }

            bool isWall(int tx, int ty, Amara::Direction dir) {
                return isWall(tx, ty, nullptr, dir);
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

            virtual void prepare() {}
            virtual void duration() {}
            virtual void handleInteracts() {}
            virtual void handleEvents() {}
    };
}

#endif
