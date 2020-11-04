#pragma once
#ifndef AMARA_GEOMETRY
#define AMARA_GEOMETRY

#include "amara.h"

namespace Amara {
    class IntVector2 {
        public:
            int x = 0;
            int y = 0;

            IntVector2() {}

            IntVector2(int nx, int ny) {
                x = nx;
                y = ny;
            }
    };

    class FloatVector2 {
        public:
            float x = 0;
            float y = 0;

            FloatVector2() {}

            FloatVector2(float nx, float ny) {
                x = nx;
                y = ny;
            }
    };

    class IntVector3: public Amara::IntVector2 {
        public:
            int z = 0;

            IntVector3() {}

            IntVector3(int nx, int ny, int nz) {
                x = nx;
                y = ny;
                z = nz;
            }
    };

    class FloatVector3: public Amara::FloatVector2 {
        public:
            float z = 0;

            FloatVector3() {}

            FloatVector3(float nx, float ny, float nz) {
                x = nx;
                y = ny;
                z = nz;
            }
    };

    class IntRect {
        public:
            int x = 0;
            int y = 0;
            int width = 0;
            int height = 0;

            IntRect() {}

            IntRect(int nx, int ny, int nw, int nh) {
                x = nx;
                y = ny;
                width = nw;
                height = nh;
            }

            bool overlaps(IntRect rect) {
                bool overlapX = Amara::valueInRange(x, rect.x, rect.x + rect.width) || Amara::valueInRange(rect.x, x, x + width);
                bool overlapY = Amara::valueInRange(y, rect.y, rect.y + rect.height) || Amara::valueInRange(rect.y, y, y + height);
                return overlapX && overlapY;
            }

            void copy(IntRect rect) {
                x = rect.x;
                y = rect.y;
                width = rect.width;
                height = rect.height;
            }
    };

    class FloatRect {
        public:
            float x = 0;
            float y = 0;
            float width = 0;
            float height = 0;

            FloatRect() {}

            FloatRect(float nx, float ny, float nw, float nh) {
                x = nx;
                y = ny;
                width = nw;
                height = nh;
            }

            bool overlaps(FloatRect rect) {
                bool overlapX = Amara::valueInRange(x, rect.x, rect.x + rect.width) || Amara::valueInRange(rect.x, x, x + width);
                bool overlapY = Amara::valueInRange(y, rect.y, rect.y + rect.height) || Amara::valueInRange(rect.y, y, y + height);
                return overlapX && overlapY;
            }

            void copy(FloatRect rect) {
                x = rect.x;
                y = rect.y;
                width = rect.width;
                height = rect.height;
            }
    };

    float distanceBetween(float sx, float sy, float ex, float ey) {
        return sqrt(sx*sx + sy*sy);
    }

    float distanceBetween(IntVector2 s, IntVector2 e) {
        return distanceBetween(s.x, s.y, e.x, e.y);
    }

    float distanceBetween(FloatVector2 s, FloatVector2 e) {
        return distanceBetween(s.x, s.y, e.x, e.y);
    }

    int getOffsetX(Amara::Direction dir) {
        switch (dir) {
            case Up:
                return 0;
                break;
            case UpLeft:
                return -1;
                break;
            case UpRight:
                return 1;
                break;
            case Down:
                return 0;
                break;
            case DownLeft:
                return -1;
                break;
            case DownRight:
                return 1;
                break;
            case Left:
                return -1;
                break;
            case Right:
                return 1;
                break;
        }

        return 0;
    }

    int getOffsetY(Amara::Direction dir) {
        switch (dir) {
            case Up:
                return -1;
                break;
            case UpLeft:
                return -1;
                break;
            case UpRight:
                return -1;
                break;
            case Down:
                return 1;
                break;
            case DownLeft:
                return 1;
                break;
            case DownRight:
                return 1;
                break;
            case Left:
                return 0;
                break;
            case Right:
                return 0;
                break;
        }
        return 0;
    }

	Amara::Direction getDirection(std::string dir) {
		if (dir.compare("up") == 0) return Up;
		if (dir.compare("down") == 0) return Down;
		if (dir.compare("left") == 0) return Left;
		if (dir.compare("right") == 0) return Right;
		if (dir.compare("upLeft") == 0) return UpLeft;
		if (dir.compare("upRight") == 0) return UpRight;
		if (dir.compare("downLeft") == 0) return DownLeft;
		if (dir.compare("downRight") == 0) return DownRight;
		return NoDir;
	}
}

#endif
