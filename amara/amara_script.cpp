namespace Amara {
    class Game;
    class Scene;
    class Actor;

    class Script: public Amara::StateManager, public Amara::Broadcaster {
        public:
            Amara::GameProperties* properties = nullptr;
            Amara::Game* game = nullptr;
            Amara::Scene* scene = nullptr;
            Amara::Actor* parent = nullptr;
            Amara::Entity* parentEntity = nullptr;
            Amara::InputManager* input = nullptr;
            Amara::ControlScheme* controls = nullptr;
            Amara::AudioGroup* audio = nullptr;
            Amara::AssetManager* assets = nullptr;
            Amara::TaskManager* tasks = nullptr;
            Amara::Loader* load = nullptr;

			std::string id;

            bool manualDeletion = false;
            bool deleteChained = true;
            bool inQueue = false;
            bool isDestroyed = false;

			bool initiated = false;
            
            nlohmann::json endConfig = nullptr;

            std::vector<Amara::Script*> chainedScripts;

            Script(bool deleteWhenDone): Amara::StateManager() {
                manualDeletion = deleteWhenDone;
            }

            Script(): Script(true) {}

            Amara::Script* chain(Amara::Script* gScript, bool parallel) {
                if (parallel || chainedScripts.size() == 0) chainedScripts.push_back(gScript);
                else chainedScripts.back()->chain(gScript);
                return gScript;
            }

            Amara::Script* chain(Amara::Script* gScript) {
                return chain(gScript, false);
            }

            Amara::Script* parallel(Amara::Script* gScript) {
                chain(gScript, true);
                return this;
            }

            std::vector<Amara::Script*> unchain() {
                std::vector<Amara::Script*> recScripts = chainedScripts;
                chainedScripts.clear();
                return recScripts;
            }

            void destroyChains() {
                if (chainedScripts.size() > 0) {
                    for (Amara::Script* chainedScript: chainedScripts) {
                        chainedScript->properties = properties;
                        chainedScript->destroyScript();
                    }
                    chainedScripts.clear();
                }
            }

            Amara::Script* thenConfigure(nlohmann::json config) {
                if (endConfig.is_null()) endConfig = nlohmann::json::object();
                endConfig.update(config);
                return this;
            }

            Amara::Script* thenConfigure(std::string key, nlohmann::json val) {
                nlohmann::json config = nlohmann::json::object();
                config[key] = val;
                return thenConfigure(config);
            }

            virtual void init(Amara::GameProperties* gameProperties) {
                Amara::StateManager::properties = gameProperties;
                properties = gameProperties;
                game = properties->game;
                if (parentEntity) scene = parentEntity->scene;
                else scene = properties->currentScene;
                input = properties->input;
                controls = properties->controls;
                audio = properties->audio;
                assets = properties->assets;
                load = properties->loader;
                messages = properties->messages;
                tasks = properties->taskManager;

                initiated = true;
                isFinished = false;
                
                reset();
                init();
            }

            virtual void init(Amara::GameProperties* gameProperties, Amara::Actor* parentActor) {
                parent = parentActor;
                parentEntity = (Amara::Entity*)parent;
                Amara::StateManager::init(gameProperties);
                init(gameProperties);
            }

            virtual void init() {}

            bool notActing() {
                return isFinished && !inQueue;
            }
            bool isActing() {
                return !notActing();
            }

            bool isFinished = false;

            virtual void finish() {
                isFinished = true;
            }
            void finishEvt() {
                if (evt()) {
                    finish();
                }
            }

            virtual void prepare() {}
            virtual void script() {}
			virtual void cancel() {}

            virtual void destroyScript() {
                if (isDestroyed) return;
                isDestroyed = true;
                if (deleteChained) {
                    destroyChains();
                }
                properties->taskManager->queueDeletion(this);
            }
            
            virtual ~Script() {
                if (deleteChained) destroyChains();
            }
    };

    bool StateManager::waitOnScript(Amara::Script* script) {
        if (evt()) {
            if (script == nullptr || script->notActing())  {
                if (script && script->manualDeletion) {
                    script->tasks->queueDeletion(script);
                }
                nextEvt();
            }
            else {
                script->manualDeletion = true;
                if (nextEvtOn(script->notActing())) {
                    script->tasks->queueDeletion(script);
                }
            }
            return true;
        }
        return false;
    }
}
