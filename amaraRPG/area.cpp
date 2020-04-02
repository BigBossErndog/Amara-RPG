#ifndef AMARA_AREA
#define AMARA_AREA

#include "amaraRPG.h"

namespace Amara {
    class Area : public Amara::Scene {
        public:
            std::deque<Prop*> props;

            nlohmann::json config;
            Amara::StateManager sm;

            Area() {}

            virtual void createObjects() {}
            virtual void createEvents() {}
            virtual void createInteracts() {}

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
                    if (prop->tileX == tx && prop->tileY == ty) {
                        return prop;
                    }
                }
                return nullptr;
            }
    };
}

#endif