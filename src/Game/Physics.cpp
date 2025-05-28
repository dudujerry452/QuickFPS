#include "Physics.h"
#include "World.h"

void Physics::UpdatePhysicalWorld(World& world) {
    for(auto& [ind, ent] : world.m_entities) {
        ent->m_pos += ent->m_velocity;
    }
}