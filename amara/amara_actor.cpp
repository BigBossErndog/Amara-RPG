#pragma once
#ifndef AMARA_ACTOR
#define AMARA_ACTOR



namespace Amara {
    class Tween;
    class Actor: public Amara::Entity {
        public:
            std::vector<Amara::Script*> scripts;
            bool actingPaused = false;

            Actor(): Amara::Entity() {}

            using Amara::Entity::init;
            void init() {
                Amara::Entity::init();
                entityType = "actor";
            }

            Amara::Actor* getActor(std::string key) {
                return (Amara::Actor*)get(key);
            }

			void destroyScript(Amara::Script* script) {
				if (script != nullptr) {
					Script* check;
					for (auto it = scripts.begin(); it != scripts.end(); ++it) {
						check = *it;
						if (check == script) {
							scripts.erase(it--);
							continue;
						}
					}
					if (script->deleteOnFinish) properties->taskManager->queueDeletion(script);
					if (script->chainedScript) destroyScript(script->chainedScript);
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
                scripts.push_back(script);
                script->init(properties, this);
                script->prepare();
                script->prepare(this);
                return script;
            }

            virtual Amara::Script* chain(Amara::Script* script) {
                if (scripts.size() > 0) {
                    Amara::Script* lastScript = scripts.back();
                    return lastScript->chain(script);
                }
                return recite(script);
            }

            virtual void reciteScripts() {
                if (scripts.size() == 0 || actingPaused) return;
                for (Amara::Script* script: scripts) {
                    if (!script->finished) {
                        script->receiveMessages();
                        script->script();
                        script->script(this);
                    }
					if (isDestroyed) {
						clearScripts();
						return;
					}
                }

                std::vector<Script*> chained;
                chained.clear();
                
                Amara::Script* script;
                for (auto it = scripts.begin(); it != scripts.end(); ++it) {
                    script = *it;
                    if (script->finished) {
                        if (script->chainedScript != nullptr) {
                            chained.push_back(script->chainedScript);
                            script->chainedScript = nullptr;
                        }
                        scripts.erase(it--);
                        if (script->deleteOnFinish) {
                            delete script;
                        }
                    }
                }
                for (Amara::Script* chain: chained) {
                    recite(chain);
                }
            }

            bool stillActing() {
                return (scripts.size() > 0);
            }

			bool notActing() {
				return (scripts.size() == 0);
			}

            bool isReciting(Amara::Script* script) {
                for (Amara::Script* check: scripts) {
                    if (check == script) return true;
                }
                return false;
            }

            void run() {
                reciteScripts();
                if (!isDestroyed) Amara::Entity::run();
            }

            Amara::Entity* add(Amara::Entity* entity) {
                return Amara::Entity::add(entity);
            }

            void clearScripts() {
                for (Amara::Script* script: scripts) {
                    if (script->deleteOnFinish) {
                        delete script;
                    }
                }
                scripts.clear();
            }

			void clearScript(std::string gid) {
				Amara::Script* script;
                for (auto it = scripts.begin(); it != scripts.end(); ++it) {
                    script = *it;
                    if (script->id.compare(gid) == 0) {
                        if (script->chainedScript != nullptr) {
                            recite(script->chainedScript);
                        }
                        scripts.erase(it--);
                        if (script->deleteOnFinish) {
                            delete script;
                        }
						return;
                    }
                }
			}

			Amara::Script* getScript(std::string gid) {
				for (Amara::Script* script: scripts) {
					if (script->id.compare(gid) == 0) {
						return script;
					}
				}
				return nullptr;
			}

			void cancelScripts() {
				for (Amara::Script* script: scripts) {
					script->cancel();
					script->cancel(this);
                    if (script->deleteOnFinish) {
                        delete script;
                    }
                }
                scripts.clear();
			}

            void pauseActing() {
                actingPaused = true;
            }
            void resumeActing() {
                actingPaused = false;
            }

            void destroy() {
                clearScripts();
                Amara::Entity::destroy();
            }
    };
}

#endif
