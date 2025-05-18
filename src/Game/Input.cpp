#include "Input.h"
#include "spdlog/spdlog.h"
#include <iostream>

void InputManager::CheckInput() {
    m_isready = false;
    m_state = InputState();
    m_state.moveOnPlane.x = IsKeyDown(KEY_W)*1.0f - 
                    IsKeyDown(KEY_S)*1.0f;
    m_state.moveOnPlane.y = IsKeyDown(KEY_D)*1.0f - 
                    IsKeyDown(KEY_A)*1.0f;
    m_state.mouseDelta = GetMouseDelta();
    // spdlog::debug("delta = {}, {}", m_state.mouseDelta.x, m_state.mouseDelta.y);
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

