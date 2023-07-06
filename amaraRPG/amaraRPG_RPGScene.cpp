#pragma once
#ifndef AMARA_AREA
#define AMARA_AREA

#include "amaraRPG.h"

namespace Amara {
    class RPGScene : public Amara::Scene, public Amara::WallFinder {
        public:
            Amara::Tilemap* tilemap = nullptr;
            nlohmann::json mapData = nullptr;

            Amara::LightLayer* lighting = nullptr;

            std::deque<Amara::RPGCutsceneBase*> cutscenes;
            Amara::RPGCutsceneBase* currentCutscene = nullptr;

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
                            if (transition->isFinished) {
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
                            if (transition->isFinished) {
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
                receiveMessages();
                updateMessages();
                
                rpgUpdate();

                reciteScripts();

                runChildren();
                checkChildren();

                for (Amara::Camera* cam : cameras) {
                    if (cam->isDestroyed || cam->parent != this) continue;
                    cam->run();
                }
            }

            virtual void rpgCreate() {
                sm.reset();

                clearCutscenes();

                lighting = nullptr;

                prepare();
				mapData = createMap();

                int i = 0;
                int aboveDepth = 0;

                if (!mapData.empty() && !mapData.is_null()) {
					add(tilemap = new Amara::Tilemap());
					tilemap->id = "tilemap";
                    
                    i = 0;
                    tilemap->configure(mapData);

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
                    runCutscenes();
                }
				else if (sm.state("transition")) {
					if (!transition) {
						sm.returnState();
					}
				}
				update();
            }

            virtual void runCutscenes() {
                if (currentCutscene != nullptr && !currentCutscene->isFinished) {
                    currentCutscene->receiveMessages();
                    currentCutscene->script();
                    currentCutscene->script(this);
                }

                if (currentCutscene == nullptr || currentCutscene->isFinished) {
                    bool startNextCutscene = true;
                    if (currentCutscene != nullptr) {
                        RPGCutsceneBase* chained = currentCutscene->chainedCutscene;
                        RPGCutsceneBase* orig = currentCutscene;
                        if (chained) {
                            currentCutscene = chained;
                            startNextCutscene = false;
                            if (!currentCutscene->initiated) {
                                currentCutscene->init(properties, this);
                                currentCutscene->prepare();
                                currentCutscene->prepare(this);
                            }
                        }
                        if (orig->deleteOnFinish) {
                            delete orig;
                        }
                    }
                    
                    if (startNextCutscene) {
                        cutscenes.pop_back();
                        if (cutscenes.size() > 0) {
                            currentCutscene = cutscenes.back();
                            if (!currentCutscene->initiated) {
                                currentCutscene->init(properties, this);
                                currentCutscene->prepare();
                                currentCutscene->prepare(this);
                            }
                        }
                        else {
                            currentCutscene = nullptr;
                            sm.returnState();
                        }
                    }
                }
            }

			virtual Amara::SceneTransitionBase* startTransition(Amara::SceneTransitionBase* gTransition) {
                Amara::Scene::startTransition(gTransition);
				if (transition && !inState("transition")) {
					sm.switchState("transition");
				}
                return transition;
            }
            
            Amara::Prop* getPropAt(int tx, int ty, Amara::Prop* exclusion) {
                Amara::Prop* prop;
                for (Amara::Entity* entity: children) {
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
                if (tilemap != nullptr && tilemap->isWall(tx, ty)) return true;

                Amara::Prop* prop;
                for (Amara::Entity* entity: children) {
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

                return false;
            }

            bool isWall(int tx, int ty) {
                return isWall(tx, ty, nullptr);
            }

            bool isWall(int tx, int ty, Amara::Prop* propExclusion, Amara::Direction dir) {
                if (tilemap != nullptr && tilemap->isWall(tx, ty, dir)) return true;
                
                Amara::Prop* prop;
                for (Amara::Entity* entity: children) {
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

                return false;
            }

            bool isWall(int tx, int ty, Amara::Direction dir) {
                return isWall(tx, ty, nullptr, dir);
            }

            Amara::RPGCutsceneBase* startCutscene(Amara::RPGCutsceneBase* cutscene) {
                cutscene->init(properties, this);
                cutscene->prepare();
                cutscene->prepare(this);

                cutscenes.push_back(cutscene);
                currentCutscene = cutscene;

                if (!inState("cutscenes")) sm.switchState("cutscenes");

                return cutscene;
            }
            Amara::RPGCutsceneBase* addCutscene(Amara::RPGCutsceneBase* cutscene) {
                if (cutscenes.size() == 0 && currentCutscene == nullptr) return startCutscene(cutscene);
                cutscenes.push_front(cutscene);
                if (!inState("cutscenes")) {
                    sm.switchState("cutscenes");
                }
                return cutscene;
            }

            void clearCutscenes() {
                for (Amara::RPGCutsceneBase* cutscene: cutscenes) {
                    delete cutscene;
                }
                cutscenes.clear();
                currentCutscene = nullptr;
                if (inState("cutscenes")) {
                    sm.returnState();
                }
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

			virtual nlohmann::json createMap() { return nullptr; }
            virtual void prepare() {}
            virtual void duration() {}
            virtual void handleInteracts() {}
            virtual void handleEvents() {}

			bool inState(std::string key) {
				return sm.inState(key);
			}
    };

    Amara::Prop* Amara::Prop::getPropAt(int gx, int gy) {
        return rpgScene->getPropAt(gx, gy);
    }
    bool Amara::Prop::tilemapIsWall(int gx, int gy) {
        return rpgScene->tilemap->isWall(gx, gy);
    }
}

#endif
