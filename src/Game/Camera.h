#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "raylib.h"
#include "rcamera.h"
#include <inttypes.h>

class World;

class WorldCamera {

    public:
    WorldCamera();

    bool Connect(World* world, uint32_t entity_id);
    bool Update(); // attach's id is checked in update
    Camera& GetCamera() { return m_camera; }

    private: 
    Camera m_camera; 
    uint32_t m_followId; 
    World* m_world; 

};

#endif