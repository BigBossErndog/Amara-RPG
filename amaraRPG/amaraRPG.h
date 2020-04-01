#include "amara.h"


#ifndef AMARARPG_DIRECTIONS
#define AMARARPG_DIRECTIONS

namespace Amara {
    enum Direction {
        NoDir,
        Up,
        UpLeft,
        UpRight,
        Down,
        DownLeft,
        DownRight,
        Left,
        Right
    };

    std::vector<Direction> DirectionsInOrder = { Down, DownLeft, Left, UpLeft, Up, UpRight, Right, DownRight };
    std::vector<Direction> FourDirections = { Down, Left, Up, Right };
    std::vector<Direction> DiagonalDirections = { DownLeft, UpLeft, UpRight, DownRight };
}

#endif

#ifndef AMARA_TILEDIM
#define AMARA_TILEDIM
namespace Amara {
    const int TILE_WIDTH = 32;
    const int TILE_HEIGHT = 32;
}
#endif

#include "walker.cpp"
#include "area.cpp"
#include "prop.cpp"
#include "assistFunctions.cpp"