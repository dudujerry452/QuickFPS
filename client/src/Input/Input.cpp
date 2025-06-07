#include "Input.h"
#include "spdlog/spdlog.h"
#include <iostream>

void InputManager::CheckInput() {
    auto& m_state = *p_backBuffer;
    memset(&m_state, 0, sizeof(m_state));
    bool hasinput = false;
    if(IsKeyPressed(KEY_W))     {hasinput = true; m_state.wasd_pressed[0] = 1;  }
    if(IsKeyPressed(KEY_A))     {hasinput = true; m_state.wasd_pressed[1] = 1;  }  
    if(IsKeyPressed(KEY_S))     {hasinput = true; m_state.wasd_pressed[2] = 1;  } 
    if(IsKeyPressed(KEY_D))     {hasinput = true; m_state.wasd_pressed[3] = 1;  } 
    if(IsKeyPressed(KEY_SPACE)) {hasinput = true; m_state.space_pressed = 1;}  
    if(IsKeyReleased(KEY_W))    {hasinput = true; m_state.wasd_pressed[0] = 2; }
    if(IsKeyReleased(KEY_A))    {hasinput = true; m_state.wasd_pressed[1] = 2; }  
    if(IsKeyReleased(KEY_S))    {hasinput = true; m_state.wasd_pressed[2] = 2; } 
    if(IsKeyReleased(KEY_D))    {hasinput = true; m_state.wasd_pressed[3] = 2; } 
    if(IsKeyReleased(KEY_SPACE)){hasinput = true; m_state.space_pressed = 2;        }

    m_state.mouseDelta = GetMouseDelta();
    if(m_state.mouseDelta.x != 0 || m_state.mouseDelta.y != 0)  hasinput = true;
    if(!hasinput) return; 

    // debug output 
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
    // if(m_wasd[0] || m_wasd[1] || m_wasd[2] || m_wasd[3] || m_space) {
    //     p_frontBuffer->moveOnPlane.x = m_wasd[0]*1.0f - 
    //                     m_wasd[2]*1.0f;
    //     p_frontBuffer->moveOnPlane.y = m_wasd[3]*1.0f - 
    //                     m_wasd[1]*1.0f;
    //     p_frontBuffer->mouseDelta = {0, 0};
    //     p_frontBuffer->isJumping = m_space;
    //     p_frontBuffer->sequence_number = m_squenceNumber++;
    // }
    p_frontBuffer->sequence_number = 0;
    return *p_frontBuffer;
}

