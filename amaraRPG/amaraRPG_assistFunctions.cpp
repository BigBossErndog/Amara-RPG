#ifndef AMARA_TILEHANDLING
#define AMARA_TILEHANDLING

#include "amaraRPG.h"

namespace Amara {
    std::string walkAnim(Amara::Direction dir) {
        switch (dir) {
            case Up:
                return "upWalk";
                break;
            case Down:
                return "downWalk";
                break;
            case Left:
                return "leftWalk";
                break;
            case Right:
                return "rightWalk";
                break;
        }

        return "";
    }
    
    std::string runAnim(Amara::Direction dir) {
        switch (dir) {
            case Up:
                return "upRun";
                break;
            case Down:
                return "downRun";
                break;
            case Left:
                return "leftRun";
                break;
            case Right:
                return "rightRun";
                break;
        }

        return "";
    }

    std::string standAnim(Amara::Direction dir) {
        switch (dir) {
            case Up:
                return "upStand";
                break;
            case Down:
                return "downStand";
                break;
            case Left:
                return "leftStand";
                break;
            case Right:
                return "rightStand";
                break;
        }

        return "";
    }
}

#endif