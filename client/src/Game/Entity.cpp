#include "Entity.h"
#include <memory>

#include "Player.h"


/* 
struct EntityState{
    uint32_t id; 
    unsigned char is_error;
    Vector3 position; 
    Vector3 forward; 
    Vector3 velocity; 
    BoundingBox bounding_box; 
    Vector3 pos_point; 
    unsigned char is_player; 
    uint32_t seq_num; 
    uint32_t health; 
    uint32_t weapon;
};
*/
std::unique_ptr<Entity> GetEntityFromState(util::EntityState state) {

    if(state.is_error) {
        std::unique_ptr<Entity> error_entity = std::make_unique<Entity>();
        error_entity->SetID(state.id);
        error_entity->SetError(true);
        return error_entity;
    }

    if(state.is_player) {
        Player player;
        player.SetID(state.id);
        player.SetPos(state.position);
        player.SetForward(state.forward);
        player.SetVelocity(state.velocity);
        player.SetBoundingBox(state.bounding_box);
        player.SetPosPoint(state.pos_point);
        player.m_latestSeq = state.seq_num;
        player.SetHealth(state.health);
        player.SetWeapon(state.weapon);
        memcpy(player.m_wasd, state.wasd, sizeof(state.wasd));
        player.m_space = state.space;
        player.m_lastTicks = state.lastticks;
        return std::make_unique<Player>(std::move(player));
    } else {
        Entity entity;
        entity.SetID(state.id);
        entity.SetPos(state.position);
        entity.SetForward(state.forward);
        entity.SetVelocity(state.velocity);
        entity.SetBoundingBox(state.bounding_box);
        entity.SetPosPoint(state.pos_point);
        return std::make_unique<Entity>(std::move(entity));
    }
}


Entity::Entity() {
    m_isError = false;

    m_forward = Vector3{0.0f, 2.0f, 0.0f};
    m_pos = Vector3{0.0f, 0.0f, 0.0f};
    m_velocity = Vector3{0.0f, 0.0f, 0.0f};
    m_id = 0;

    m_entityType = EntityType::Base;
    
    m_posPoint = {0.0f, 0.0f, 0.0f};
}

/*
        // meta data 
        EntityType m_entityType;
        uint32_t m_id;
        bool m_isError;

        // physical variable
        Vector3 m_pos;
        Vector3 m_forward;
        Vector3 m_velocity;

        // Collission ralated
        util::BoundingBox m_boundingBox;
        Vector3 m_posPoint; // where the pos mean in model coor
*/

Entity& Entity::operator=(Entity& other) {
    if (this == &other) return *this; // self-assignment check
    m_entityType = other.m_entityType;
    m_id = other.m_id;
    m_isError = other.m_isError;
    m_pos = other.m_pos;
    m_forward = other.m_forward;
    m_velocity = other.m_velocity;
    m_boundingBox = other.m_boundingBox;
    m_posPoint = other.m_posPoint;
    return *this;
}
