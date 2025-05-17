#include "Input.h"
#include <iostream>

void InputManager::CheckInput() {
    m_isready = false;
    m_state = InputState();
    if(IsKeyPressed(KEY_W)) {
        m_state.moveForward = 1.0f;
    } else if(IsKeyPressed(KEY_S)) {
        m_state.moveForward = -1.0f;
    }
    if(IsKeyPressed(KEY_D)) {
        m_state.moveStrafe = 1.0f;
    } else if(IsKeyPressed(KEY_A)) {
        m_state.moveStrafe = -1.0f;
    }
    m_isready = true;
}

bool InputManager::IsReady() {
    if(!m_isready) return false;
    return true;
}

InputState InputManager::Peek() {
    if(!m_isready) throw std::runtime_error("InputManager: Peek unexisting InputState");
    return m_state;
}

void InputManager::Pop() {
    m_isready = false;
}

