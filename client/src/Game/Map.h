#ifndef _MAP_H_
#define _MAP_H_

#include <vector>
#include <string>
#include "../Util/Util.h"

struct MapObject {
    util::Color color;
    util::BoundingBox colisionBoxes;
};

struct Map {
    std::string name;
    std::vector<MapObject> objects;

    Vector3 respawnPoint;
};

#endif