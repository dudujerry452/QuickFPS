#ifndef _MAP_H_
#define _MAP_H_

#include "raylib.h"
#include <vector>
#include <string>

struct MapObject {
    Color color;
    BoundingBox colisionBoxes;
};

struct Map {
    std::string name;
    std::vector<MapObject> objects;

    Vector3 respawnPoint;
};

#endif