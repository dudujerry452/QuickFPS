#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "raylib.h"
#include "rcamera.h"
#include <inttypes.h>

class World;

class WorldCamera {

    public:
    WorldCamera(): m_follow_id(0) {}

    void Init();
    void Attach(uint32_t entity_id) { m_follow_id = entity_id; }
    bool Update(World* world); // attach's id is checked in update

    Camera& GetCamera() { return m_camera; }

    private: 
    Camera m_camera; 
    uint32_t m_follow_id; 

};

#endif