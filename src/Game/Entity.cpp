#include "Entity.h"

Entity::Entity() {
    m_forward = Vector3{0.0f, 2.0f, 0.0f};
    m_pos = Vector3{0.0f, 0.0f, 0.0f};
    m_id = 0;
}