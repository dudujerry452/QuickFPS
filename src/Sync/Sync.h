#ifndef _SYNC_H_
#define _SYNC_H_

#include "../Game/World.h"

struct RenderStateBuffer {
    std::vector<Entity> entities;
    std::vector<MapObject> objects;
    Camera camera;
};

#endif