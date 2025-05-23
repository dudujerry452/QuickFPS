#include "Entity.h"

Entity::Entity() {
    m_isError = false;

    m_forward = Vector3{0.0f, 2.0f, 0.0f};
    m_pos = Vector3{0.0f, 0.0f, 0.0f};
    m_id = 0;
}