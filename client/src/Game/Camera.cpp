#include "Camera.h"
#include "../Game/World.h"

WorldCamera::WorldCamera() {
    m_camera = { 0 };
    m_camera.position = { 0.0f, 2.0f, 4.0f };    // Camera position
    m_camera.target = { 0.0f, 2.0f, 0.0f };      // Camera looking at point
    m_camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    m_camera.fovy = 60.0f;                                // Camera field-of-view Y
    // m_camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type
}

bool WorldCamera::Connect(World* world, uint32_t entity_id) {
    if(!world) return false;
    auto obj = world->GetEntity(entity_id);
    if(!obj) return false;
    m_world = world; 
    m_followId = entity_id;
    return true;
}

bool WorldCamera::Update() {
    auto obj = m_world->GetEntity(m_followId);
    if(!obj) return false; // not exists

    m_camera.position = obj->GetPos();
    m_camera.target = obj->GetPos() + obj->GetForward();
    return true;
}

