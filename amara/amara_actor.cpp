namespace Amara {
    class Tween;
    class Actor: public Amara::Entity {
    private:
        bool inRecital = false;
        std::vector<Amara::Script*> scriptBuffer;
        
    public:
        std::list<Amara::Script*> scripts;
        bool actingPaused = false;

        Actor(): Amara::Entity() {}

        using Amara::Entity::init;
        void init() {
            Amara::Entity::init();
            entityType = "actor";
        }

        nlohmann::json toData() {
            nlohmann::json config = Amara::Entity::toData();
            config["numberOfScripts"] = scripts.size();
            return config;
        }

        Amara::Actor* getActor(std::string key) {
            return (Amara::Actor*)get(key);
        }

        void destroyScript(Amara::Script* script) {
            if (script != nullptr) {
                Script* check;
                for (auto it = scripts.begin(); it != scripts.end();) {
                    check = *it;
                    if (check == script) {
                        it = scripts.erase(it);
                        continue;
                    }
                    ++it;
                }
                if (!script->manualDeletion) properties->taskManager->queueDeletion(script);
            } 
        }

        virtual Amara::Script* recite(Amara::Script* script) {
            if (script == nullptr) {
                SDL_Log("Null script was given.");
                return nullptr;
            }
            if (isDestroyed) {
                destroyScript(script);
                return script;
            }
            if (!inRecital) {
                scripts.push_back(script);
            }
            else {
                scriptBuffer.push_back(script);
            }
            script->inQueue = true;
            script->init(properties, this);
            script->prepare();
            return script;
        }

        virtual Amara::Script* chain(Amara::Script* script) {
            if (scriptBuffer.size() > 0) {
                Amara::Script* lastScript = scriptBuffer.back();
                return lastScript->chain(script);
            }
            if (scripts.size() > 0) {
                Amara::Script* lastScript = scripts.back();
                return lastScript->chain(script);
            }
            return recite(script);
        }

        virtual void reciteScripts() {
            if (scripts.size() == 0 || actingPaused) return;
            
            inRecital = true;
            for (Amara::Script* script: scripts) {
                if (!script->isFinished) {
                    script->receiveMessages();
                    script->updateMessages();
                    if (!isDestroyed && !script->isFinished) script->script();
                    if (isDestroyed) break;
                }
                if (isDestroyed) {
                    clearScripts();
                    inRecital = false;
                    break;
                }
            }

            std::vector<Script*> chained;
            chained.clear();
            
            Amara::Script* script;
            for (auto it = scripts.begin(); it != scripts.end();) {
                script = *it;
                if (script->isFinished) {
                    if (!script->endConfig.is_null()) configure(script->endConfig);
                    if (script->chainedScripts.size() > 0) {
                        if (!isDestroyed) {
                            for (Amara::Script* chainedScript: script->chainedScripts) chained.push_back(chainedScript);
                        }
                        script->chainedScripts.clear();
                    }
                    script->inQueue = false;
                    if (!script->manualDeletion) {
                        properties->taskManager->queueDeletion(script);
                    }
                    it = scripts.erase(it);
                    continue;
                }
                ++it;
            }

            if (isDestroyed) {
                inRecital = false;
                clearScripts();
                return;
            }

            for (Amara::Script* chain: chained) {
                recite(chain);
            }

            inRecital = false;
            pipeScriptBuffer();
        }

        void pipeScriptBuffer() {
            for (Amara::Script* script: scriptBuffer) {
                scripts.push_back(script);
            }
            scriptBuffer.clear();
        }

        bool stillActing() {
            return (scripts.size() > 0 || scriptBuffer.size() > 0);
        }

        bool isActing() { return stillActing(); }

        bool notActing() {
            return !stillActing();
        }

        bool isReciting(Amara::Script* script) {
            for (Amara::Script* check: scripts) {
                if (check == script) return true;
            }
            for (Amara::Script* check: scriptBuffer) {
                if (check == script) return true;
            }
            return false;
        }

        void run() {
			debugID = id;
			std::string debugCopy;
			if (debugging) {
				debugID = "";
				for (int i = 0; i < properties->entityDepth; i++) debugID += "\t";
				debugID += id;
				SDL_Log("%s (%s): Running.", debugID.c_str(), entityType.c_str());
				debugCopy = debugID;
			}
            
            receiveMessages();
            updateMessages();

            Amara::Interactable::run();
            if (isInteractable && interact.isDraggable && interact.isDown) {
				interact.isBeingDragged = true;
				if (physics) {
					physics->velocityX = interact.movementX;
					physics->velocityY = interact.movementY;
				}
				else {
					x += interact.movementX;
					y += interact.movementY;
				}
			}
			else interact.isBeingDragged = false;


            update();

            if (physics != nullptr) {
                if (physics->isActive) physics->run();
                
                if (physics->isDestroyed) {
                    removePhysics();
                }
            }

            if (attachedTo != nullptr) {
                if (attachedTo->isDestroyed) {
                    attachedTo = nullptr;
                }
                else {
                    x = attachedTo->x + attachmentOffsetX;
                    y = attachedTo->y + attachmentOffsetY;
                }
            }
            
            if (debugging) SDL_Log("%s (%s): Reciting Scripts.", debugCopy.c_str(), entityType.c_str());
            reciteScripts();

            runChildren();
            checkChildren();

            if (debugging) SDL_Log("%s (%s): Finished Running.", debugCopy.c_str(), entityType.c_str());
        }

        using Amara::Entity::destroy;
        
        virtual void destroy(bool recursiveDestroy) {
            clearScripts();
            Amara::Entity::destroy(recursiveDestroy);
        }

        Amara::Actor* clearScripts() {
            for (Amara::Script* script: scripts) {
                script->deleteScript();
            }
            scripts.clear();
            for (Amara::Script* script: scriptBuffer) {
                script->deleteScript();
            }
            scriptBuffer.clear();
            return this;
        }

        Amara::Actor* clearScript(std::string gid) {
            Amara::Script* script;
            for (auto it = scripts.begin(); it != scripts.end();) {
                script = *it;
                if (script->id.compare(gid) == 0) {
                    if (script->chainedScripts.size() > 0) {
                        for (Amara::Script* chainedScript: script->chainedScripts) recite(chainedScript);
                    }
                    it = scripts.erase(it);
                    if (!script->manualDeletion) {
                        properties->taskManager->queueDeletion(script);
                    }
                    break;
                }
                ++it;
            }
            for (auto it = scriptBuffer.begin(); it != scriptBuffer.end();) {
                script = *it;
                if (script->id.compare(gid) == 0) {
                    if (script->chainedScripts.size() > 0) {
                        for (Amara::Script* chainedScript: script->chainedScripts) recite(chainedScript);
                    }
                    it = scriptBuffer.erase(it);
                    if (!script->manualDeletion) {
                        properties->taskManager->queueDeletion(script);
                    }
                    break;
                }
                ++it;
            }
            return this;
        }

        Amara::Script* getScript(std::string gid) {
            for (Amara::Script* script: scripts) {
                if (script->id.compare(gid) == 0) {
                    return script;
                }
            }
            return nullptr;
        }

        Amara::Actor* cancelScripts() {
            for (Amara::Script* script: scripts) {
                script->cancel();
                if (!script->manualDeletion) {
                    properties->taskManager->queueDeletion(script);
                }
            }
            scripts.clear();
            for (Amara::Script* script: scriptBuffer) {
                script->cancel();
                if (!script->manualDeletion) {
                    properties->taskManager->queueDeletion(script);
                }
            }
            scriptBuffer.clear();

            return this;
        }

        Amara::Actor* pauseActing() {
            actingPaused = true;
            return this;
        }
        Amara::Actor* resumeActing() {
            actingPaused = false;
            return this;
        }
    };
}