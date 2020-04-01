#ifndef AMARA_EVENTS
#define AMARA_EVENTS

#include "amara.h"

namespace Amara {
    class Interactable;

    enum EventType {
        SCENELEFTCLICK,
        SCENERIGHTCLICK,
        SCENEMIDDLECLICK,
        SCENELEFTRELEASE,
        SCENERIGHTRELEASE,
        SCENEMIDDLERELEASE,

        OBJECTLEFTCLICK,
        OBJECTRIGHTCLICK,
        OBJECTMIDDLECLICK,

        OBJECTLEFTRELEASE,
        OBJECTRIGHTRELEASE,
        OBJECTMIDDLERELEASE
    };

    class Event {
        public:
            Amara::EventType type;
            bool disabled = false;
            std::vector<Amara::Interactable*> ignore;
    };
    
    class EventManager {
        public:
            Amara::GameProperties* properties = nullptr;
            Amara::InputManager* input = nullptr;

            std::vector<Amara::Event*> eventList;

            EventManager(Amara::GameProperties* gameProperties) {
                properties = gameProperties;
                input = properties->input;
            }
            
            void addEvent(Amara::EventType type) {
                Amara::Event* evt = new Amara::Event();
                evt->type = type;
                eventList.push_back(evt);
            }

            void manage() {
                for (Amara::Event* evt : eventList) {
                    delete evt;
                }
                eventList.clear();

                Amara::Mouse* mouse = input->mouse;
                if (mouse->left->justDown) {
                    addEvent(SCENELEFTCLICK);
                    addEvent(OBJECTLEFTCLICK);
                }
                if (mouse->right->justDown) {
                    addEvent(SCENERIGHTCLICK);
                    addEvent(OBJECTRIGHTCLICK);
                }
                if (mouse->middle->justDown) {
                    addEvent(SCENEMIDDLECLICK);
                    addEvent(OBJECTMIDDLECLICK);
                }
            }
    };
}

#endif