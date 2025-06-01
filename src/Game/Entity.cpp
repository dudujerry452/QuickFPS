#include "Entity.h"

Entity::Entity() {
    m_isError = false;

    m_forward = Vector3{0.0f, 2.0f, 0.0f};
    m_pos = Vector3{0.0f, 0.0f, 0.0f};
    m_velocity = Vector3{0.0f, 0.0f, 0.0f};
    m_id = 0;

    m_entityType = EntityType::Base;
}
Entity& Entity::operator=(const Entity& other) {
    m_boundingBox = other.m_boundingBox;
    m_forward = other.m_forward;
    m_id = other.m_id;
    m_isError = other.m_isError;
    m_pos = other.m_pos;
    return *this;
}
