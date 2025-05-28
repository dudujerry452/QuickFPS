#include "Input.h"
#include "spdlog/spdlog.h"
#include <iostream>

void InputManager::CheckInput() {
    auto& m_state = *p_backBuffer;
    m_state = InputState();
    m_state.moveOnPlane.x = IsKeyDown(KEY_W)*1.0f - 
                    IsKeyDown(KEY_S)*1.0f;
    m_state.moveOnPlane.y = IsKeyDown(KEY_D)*1.0f - 
                    IsKeyDown(KEY_A)*1.0f;
    m_state.mouseDelta = GetMouseDelta();
    m_state.sequence_number = m_squenceNumber++;
    {
        std::lock_guard<std::mutex> lock(m_swapMutex);
        std::swap(p_backBuffer, p_frontBuffer);
        i_consumed = 0;
    }
}

InputState InputManager::Peek() {
    std::lock_guard<std::mutex> lock(m_swapMutex);
    if(i_consumed) p_frontBuffer->sequence_number = 0; // invalid
    return *p_frontBuffer;
}

InputState InputManager::Pop() {
    std::lock_guard<std::mutex> lock(m_swapMutex);
    if(i_consumed) p_frontBuffer->sequence_number = 0;
    i_consumed = 1;
    return *p_frontBuffer;
}

