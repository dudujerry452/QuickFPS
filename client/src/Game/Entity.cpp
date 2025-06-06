#include "Entity.h"

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
