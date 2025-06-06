
#include "Player.h"
#include "raymath.h"
#include "rcamera.h"

#include "spdlog/spdlog.h"




Player::Player() {
    m_entityType = EntityType::Player;   
    m_boundingBox = util::BoundingBox{
        Vector3{0,0,0}, 
        Vector3{1,2,1}
    };
    m_posPoint = {0.5f, 1.5f, 0.5f};
}

void Player::PhysicsUpdate() {
    Entity::PhysicsUpdate();
    // auto input = IM.Pop();
    Vector2 v2d{m_velocity.x, m_velocity.z};
    if(Vector2Length(v2d) > 0.07f) {
        v2d = v2d - Vector2Scale(v2d, 0.07f);
    } else {
        v2d = {0,0};
    }
    m_velocity.x = v2d.x, m_velocity.z = v2d.y;

    m_velocity.y -= 0.005f; // gravity
} 

util::PlayerState Player::GetState() const {
    return util::PlayerState{
        m_pos,
        m_forward, 
        m_velocity, 
        m_health, 
        m_weapon
    };
}

void Player::PushNewInput(const util::InputState& new_input) {
    if(!new_input.sequence_number) {
        return;
        // PushNewInput(new_input);
        // m_inputQueue.push_back(input);
    } 
    Vector2 fow{GetForward().x, GetForward().z};
    if(Vector2Length(fow) < 0.00001f) 
        SetForward({1,0,0});
    
    Camera3D tmp = { 0 };
    tmp.position = GetPos();    
    tmp.target = GetPos() + GetForward();      // tmp looking at point
    tmp.up = { 0.0f, 1.0f, 0.0f };          // tmp up vector (rotation towards target)

    CameraYaw(&tmp, -new_input.mouseDelta.x*0.005f, false);
    CameraPitch(&tmp, -new_input.mouseDelta.y*0.005f, true, false, true);
    SetForward(tmp.target - tmp.position);

    Vector2 mov{new_input.moveOnPlane.x, new_input.moveOnPlane.y};
    Vector2 fow2;
    fow = Vector2Normalize(fow);
    fow2 = {-fow.y, fow.x};
    mov = Vector2Normalize(mov);
    mov = Vector2Scale(mov, 0.1f);

    Vector2 delta {
        fow.x*mov.x + fow2.x * mov.y, 
        fow.y*mov.x + fow2.y * mov.y
    };
    if(abs(new_input.moveOnPlane.x) > 0 || abs(new_input.moveOnPlane.y) > 0) {
        m_velocity.x = delta.x; 
        m_velocity.z = delta.y;
    }

    if(m_velocity.y == 0.0f && new_input.isJumping) {
        m_velocity.y = 0.2f; // jump velocity
    }

}

LocalPlayer::LocalPlayer() {
    m_entityType = EntityType::LocalPlayer;
}
void LocalPlayer::PushNewInput(const util::InputState& new_input) {
    if(new_input.sequence_number) {
        Player::PushNewInput(new_input);
        m_inputQueue.push_back(new_input);
    } 
}
