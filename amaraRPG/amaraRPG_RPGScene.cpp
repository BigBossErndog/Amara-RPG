#ifndef AMARA_AREA
#define AMARA_AREA

#include "amaraRPG.h"

namespace Amara {
    class RPGScene : public Amara::Scene {
        public:
            std::deque<Amara::Prop*> props;
            Amara::Tilemap* tilemap = nullptr;

            nlohmann::json config;
            nlohmann::json tiledJson;

            Amara::StateManager sm;

            Area() {}

            void create() {
                sm.reset();
                props.clear();

                onPrepare();

                add(tilemap = new Amara::Tilemap());
                tilemap->createLayer(100, 100, 100, 100);
                
                if (!tiledJson.empty()) {

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

            bool isWall(int tx, int ty) {
                Amara::Prop* prop = getPropAt(tx, ty);
                if (prop != nullptr) {
                    if (prop->isActive && prop->isWall) {
                        return true;
                    }
                }

                return false;
            }
    };
}

#endif