#include "Physics.h"
#include "World.h"
#include "../Util/Util.h"

bool CheckCollisionBoxes(util::BoundingBox box1, util::BoundingBox box2)
{
    bool collision = true;

    if ((box1.max.x >= box2.min.x) && (box1.min.x <= box2.max.x))
    {
        if ((box1.max.y < box2.min.y) || (box1.min.y > box2.max.y)) collision = false;
        if ((box1.max.z < box2.min.z) || (box1.min.z > box2.max.z)) collision = false;
    }
    else collision = false;

    return collision;
}

void Physics::UpdatePhysicalWorld(World& world) {
    for(auto& [ind, ent] : world.m_entities) {
        if(ent->IsError()) continue;
        const auto getxyz = [](Vector3& vec, int p) -> float& {
            if(p == 0) return vec.x;
            if(p == 1) return vec.y;
            return vec.z;
        };
        for(int i = 0 ; i < 3; i ++) {
            float& velox = getxyz(ent->m_velocity, i);
            getxyz(ent->m_pos, i) += velox;
            for(auto& obj : world.m_worldMap.objects) {
                util::BoundingBox entbox = ent->GetGlobalBoundingBox();
                auto ent_min = getxyz(entbox.min, i);
                auto ent_max = getxyz(entbox.max, i);
                auto obj_min = getxyz(obj.colisionBoxes.min, i);
                auto obj_max = getxyz(obj.colisionBoxes.max, i);
                if(CheckCollisionBoxes(obj.colisionBoxes, entbox)) {

                        if(velox == 0.0f) continue;
                        getxyz(ent->m_pos, i) -= velox;
                        velox = 0;

                        // if(velox > 0) {
                        //     // auto delta = ent_max - obj_min;
                        //     // getxyz(ent->m_pos, i) -= delta; 
                        // } else {
                        //     // auto delta = obj_max - ent_min;
                        //     // getxyz(ent->m_pos, i) += delta; 
                        //     getxyz(ent->m_pos, i) -= velox;
                        //     velox = 0;
                        // }
                } 
            }

        }
    }
    
    
}