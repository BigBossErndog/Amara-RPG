#pragma once

#include "amara.h"

namespace Amara {
    class TouchPointer: public Amara::Key {
    public:
        Amara::GameProperties* properties = nullptr;
        SDL_FingerID id;
        
        int x = 0;
        int y = 0;
        int dx = 0;
        int dy = 0;
        int movementX = 0;
        int movementY = 0;
        float pressure = 0;

        bool inUse = false;

        TouchPointer(Amara::GameProperties* gProperties): Amara::Key() {
            properties = gProperties;
        }

        void manage() {
            Amara::Key::manage();
        }

        void virtualizeXY(SDL_Event& e) {
            float upScale, offset;

            x = e.tfinger.x * (float)properties->resolution->width;
            y = e.tfinger.y * (float)properties->resolution->height;
            dx = x;
            dy = y;

            float ratioRes = ((float)properties->resolution->width / (float)properties->resolution->height);
            float ratioWin = ((float)properties->window->width / (float)properties->window->height);

            if (ratioRes < ratioWin) {
                upScale = ((float)properties->window->height/(float)properties->resolution->height);
                offset = ((float)properties->window->width - ((float)properties->resolution->width * upScale))/2;
                dx = x + offset;
            }
            else if (ratioRes > ratioWin) {
                upScale = ((float)properties->window->width/(float)properties->resolution->width);
                offset = ((float)properties->window->height - ((float)properties->resolution->height * upScale))/2;
                dy = y + offset;
            }
        }
    };

    class TouchManager {
    public:
        Amara::GameProperties* properties = nullptr;
        bool isActivated = true;

        std::vector<TouchPointer*> pointers;
        int numFingers = 0;

        TouchPointer* lastPointer = nullptr;
        TouchPointer* activePointer = nullptr;

        TouchManager(Amara::GameProperties* gProperties) {
            properties = gProperties;
            pointers.clear();
            for (int i = 0; i < 10; i++) {
                pointers.push_back(new TouchPointer(properties));
            }
        }

        TouchPointer* getPointer(SDL_FingerID gid) {
            for (TouchPointer* pointer: pointers) {
                if (pointer->inUse && pointer->id == gid) {
                    return pointer;
                }
            }
            return nullptr;
        }

        TouchPointer* newPointer(SDL_FingerID gid) {
            for (TouchPointer* pointer: pointers) {
                if (!pointer->inUse) {
                    numFingers += 1;
                    pointer->id = gid;
                    pointer->inUse = true;
                    return pointer;
                }
            }
            return nullptr;
        }
        void removePointer(SDL_FingerID gid) {
            TouchPointer* pointer = getPointer(gid);
            if (pointer && pointer->inUse) {
                pointer->inUse = false;
                numFingers = false;
            }
        }

        void manage() {
            isActivated = false;
            for (TouchPointer* pointer: pointers) {
                pointer->manage();
            }
        }
    };
}