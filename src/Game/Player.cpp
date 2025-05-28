#include "Player.h"
#include "raymath.h"
#include "rcamera.h"

#include "spdlog/spdlog.h"

PlayerState Player::GetState() const {
    return PlayerState{
        m_pos,
        m_forward, 
        m_health, 
        m_weapon
    };
}

void LocalPlayer::PhysicsUpdate() {
    auto input = IM.Pop();
    if(input.sequence_number) PushNewInput(input);
} 

void LocalPlayer::PushNewInput(const InputState& new_input) {
    if(GetForward() == Vector3{0, 1, 0} || GetForward() == Vector3{0, -1, 0}) // 避免头顶朝天朝地
        SetForward({1,0,0});
    
    Camera3D tmp = { 0 };
    tmp.position = GetPos();    
    // tmp position
    tmp.target = GetPos() + GetForward();      // tmp looking at point
    tmp.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // tmp up vector (rotation towards target)
    tmp.fovy = 60.0f;                                // tmp field-of-view Y
    tmp.projection = CAMERA_PERSPECTIVE;             // tmp projection type

    UpdateCameraPro(
        &tmp,
        Vector3{new_input.moveOnPlane.x*0.1f,
                new_input.moveOnPlane.y*0.1f, 
                0.0f}, 
        Vector3{new_input.mouseDelta.x*0.05f, 
            new_input.mouseDelta.y*0.05f, 0.f},
        // Vector3{0.01, 0.0f, 0.0f}, 
        0.0f
    );
    SetPos(tmp.position);
    // SetForward(tmp.target);
    SetForward(tmp.target - tmp.position);
    m_inputQueue.push_back(new_input);
}