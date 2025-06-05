#include "Input.h"
#include "spdlog/spdlog.h"
#include <iostream>

void InputManager::CheckInput() {
    auto& m_state = *p_backBuffer;
    m_state = util::InputState();
    if(IsKeyPressed(KEY_W)) m_wasd[0] = 1;
    if(IsKeyPressed(KEY_A)) m_wasd[1] = 1;
    if(IsKeyPressed(KEY_S)) m_wasd[2] = 1;
    if(IsKeyPressed(KEY_D)) m_wasd[3] = 1;
    if(IsKeyPressed(KEY_SPACE)) m_space = 1;
    if(IsKeyReleased(KEY_W)) m_wasd[0] = 0;
    if(IsKeyReleased(KEY_A)) m_wasd[1] = 0;
    if(IsKeyReleased(KEY_S)) m_wasd[2] = 0;
    if(IsKeyReleased(KEY_D)) m_wasd[3] = 0;
    if(IsKeyReleased(KEY_SPACE)) m_space = 0;

    m_state.moveOnPlane.x = m_wasd[0]*1.0f - 
                    m_wasd[2]*1.0f;
    m_state.moveOnPlane.y = m_wasd[3]*1.0f - 
                    m_wasd[1]*1.0f;
    m_state.isJumping = m_space;
    m_state.mouseDelta = GetMouseDelta();
    {
        std::lock_guard<std::mutex> lock(m_swapMutex);
        m_state.sequence_number = m_squenceNumber++;
        std::swap(p_backBuffer, p_frontBuffer);
        i_consumed = 0;
    }
}

util::InputState InputManager::Peek() {
    std::lock_guard<std::mutex> lock(m_swapMutex);
    if(i_consumed) p_frontBuffer->sequence_number = 0; // invalid
    return *p_frontBuffer;
}

util::InputState InputManager::Pop() {
    std::lock_guard<std::mutex> lock(m_swapMutex);
    if(!i_consumed) {
        i_consumed = 1;
        return *p_frontBuffer;
    }
    if(m_wasd[0] || m_wasd[1] || m_wasd[2] || m_wasd[3] || m_space) {
        p_frontBuffer->moveOnPlane.x = m_wasd[0]*1.0f - 
                        m_wasd[2]*1.0f;
        p_frontBuffer->moveOnPlane.y = m_wasd[3]*1.0f - 
                        m_wasd[1]*1.0f;
        p_frontBuffer->mouseDelta = {0, 0};
        p_frontBuffer->isJumping = m_space;
        p_frontBuffer->sequence_number = m_squenceNumber++;
    }
    else p_frontBuffer->sequence_number = 0;
    i_consumed = 1;
    return *p_frontBuffer;
}

