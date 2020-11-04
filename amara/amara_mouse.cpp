#pragma once
#ifndef AMARA_MOUSE
#define AMARA_MOUSE

#include "amara.h"

namespace Amara {

    class MouseButton: public Amara::Key {
        public:
            MouseButton(): Amara::Key() {}
    };

    class Mouse: public Pointer {
        public:
            Amara::GameProperties* properties = nullptr;

            Amara::MouseButton* left = nullptr;
			Amara::MouseButton* right = nullptr;
            Amara::MouseButton* middle = nullptr;

            int dx = 0;
            int dy = 0;

            int scrollX = 0;
            int scrollY = 0;

			Mouse(Amara::GameProperties* gameProperties) {
                properties = gameProperties;

				left = new Amara::MouseButton();
				right = new Amara::MouseButton();
                middle = new Amara::MouseButton();
			}

			void manage() {
				left->manage();
				right->manage();
                middle->manage();

                isDown = left->isDown || right->isDown || middle->isDown;
                justDown = left->justDown || right->justDown || middle->justDown;

                scrollX = 0;
                scrollY = 0;
			}
    };
}

#endif