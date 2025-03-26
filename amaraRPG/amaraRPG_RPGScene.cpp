namespace Amara {
    class RPGScene : public Amara::Scene, public Amara::WallFinder {
        public:
            Amara::Tilemap* tilemap = nullptr;
            nlohmann::json mapData = nullptr;

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

            virtual void manageScene() {
                if (transition != nullptr) {
                    transition->run();

                    if (transition != nullptr && transition->endScene == this) {
                        if (transition->isFinished) {
                            initialLoaded = true;
                            transition->complete();
                            transition = nullptr;
                        }
                        else if (transition->waitingForPermission) {
                            transition->grantPermission();
                        }
                    }

                    if (transition == nullptr || 
                        (transition->startScene == this && !transition->waitingForPermission && !transition->permissionGranted) || 
                        (transition->endScene == this && transition->permissionGranted)
                    ) {
                        if (!initialLoaded) {
                            load->run();
                            if (!load->stillLoading) {
                                initialLoaded = true;
                                setLoader(properties->loader);
                                rpgCreate();
                            }
                            else whileLoading();
                        }
                        else updateScene();
                    }
                }
                else if (!initialLoaded) {
                    load->run();
                    if (!load->stillLoading) {
                        initialLoaded = true;
                        setLoader(properties->loader);
                        rpgCreate();
                    }
                    else whileLoading();
                }
                else updateScene();
            }

            virtual void run() {
                properties->currentScene = this;
				properties->currentCamera = mainCamera;
                manageScene();
            }

            virtual void updateScene() {
                debugID = id;
                std::string debugCopy;
                if (debugging) {
                    debugID = "";
                    for (int i = 0; i < properties->entityDepth; i++) debugID += "\t";
                    debugID += id;
                    SDL_Log("%s (%s): Running. - depth %d", debugID.c_str(), entityType.c_str(), properties->entityDepth);
                    debugCopy = debugID;
                }

                receiveMessages();
                updateMessages();
                if (isDestroyed) return;
                
                properties->entityDepth = 0;
                properties->scrollX = 0;
                properties->scrollY = 0;
                properties->offsetX = 0;
                properties->offsetY = 0;
                properties->zoomX = 1;
                properties->zoomY = 1;
                properties->zoomFactorX = 1;
                properties->zoomFactorY = 1;
                properties->angle = 0;
                properties->alpha = 1;
                
                rpgUpdate();
                if (isDestroyed) return;

                if (debugging) SDL_Log("%s (%s): Reciting Scripts (%d).", debugCopy.c_str(), entityType.c_str(), scripts.size());
                reciteScripts();
                if (isDestroyed) return;
                
                runChildren();
                if (isDestroyed) return;
                
                std::vector<Amara::Camera*> copyCameras = cameras;
                Amara::Camera* cam;
                for (auto it = copyCameras.begin(); it != copyCameras.end();) {
                    cam = *it;
                    if (cam == nullptr || cam->isDestroyed || cam->parent != this) {
                        ++it;
                        continue;
                    }
                    else {
                        cam->run();
                    }
                    ++it;
                }

                if (debugging) SDL_Log("%s (%s): Finished Running.", debugCopy.c_str(), entityType.c_str());
            }

            virtual void rpgCreate() {
                sm.reset();

                clearCutscenes();

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
            }

            virtual void rpgUpdate() {
                if (sm.state("start")) {
                    if (currentCutscene) {
                        sm.switchState("cutscenes");
                    }
                    else {
                        sm.switchState("duration");
                    }
                }
                else if (sm.state("duration")) {
                    duration();
                }
                else if (sm.state("cutscenes")) {
                    runCutscenes();
                }
				update();
            }

            virtual void onStop() {
                Scene::onStop();
                clearCutscenes();
            }

            virtual void runCutscenes() {
                if (currentCutscene != nullptr && !currentCutscene->isFinished) {
                    currentCutscene->receiveMessages();
                    currentCutscene->script();
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
                            }
                        }
                        else currentCutscene = nullptr;
                        if (!orig->manualDeletion) {
                            if (!orig->initiated) orig->init(properties);
                            orig->destroyScript();
                        }
                    }
                    
                    if (startNextCutscene) {
                        if (cutscenes.size() > 0) {
                            currentCutscene = cutscenes.back();
                            cutscenes.pop_back();
                            if (!currentCutscene->initiated) {
                                currentCutscene->init(properties, this);
                                currentCutscene->prepare();
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
                return tilemap->getTilemapLayer(layerKey);
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

                if (currentCutscene) cutscenes.push_back(currentCutscene);
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
                if (cutscenes.size()) {
                    for (Amara::RPGCutsceneBase* cutscene: cutscenes) {
                        if (cutscene && !cutscene->manualDeletion) {
                            if (!cutscene->initiated) cutscene->init(properties);
                            cutscene->destroyScript();
                        }
                    }
                    cutscenes.clear();
                }
                if (currentCutscene && !currentCutscene->manualDeletion) {
                    if (!currentCutscene->initiated) currentCutscene->init(properties);
                    currentCutscene->destroyScript();
                }
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
