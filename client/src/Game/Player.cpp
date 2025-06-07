
#include "Player.h"
#include "raymath.h"
#include "rcamera.h"

#include "spdlog/spdlog.h"





Player::Player():
Entity(), 
m_latestSeq(0)
{
    m_entityType = EntityType::Player;   
    m_boundingBox = util::BoundingBox{
        {0,0,0}, 
        {1,2,1}
    };
    m_posPoint = {0.5f, 1.5f, 0.5f};
}

void Player::PhysicsUpdate() {
    Entity::PhysicsUpdate();
    UpdateByInput();
    Vector2 v2d{m_velocity.x, m_velocity.z};
    if(Vector2Length(v2d) > 0.07f) {
        v2d = v2d - Vector2Scale(v2d, 0.07f);
    } else {
        v2d = {0,0};
    }
    m_velocity.x = v2d.x, m_velocity.z = v2d.y;

    m_velocity.y -= 0.005f; // gravity
} 

util::EntityState Player::GetState() const {
    return util::EntityState{
        m_id, 
        static_cast<unsigned char>(m_isError),
        m_pos, 
        m_forward, 
        m_velocity, 
        m_boundingBox, 
        m_posPoint,
        1, 
        m_latestSeq, // sequence number
        5, // health
        5,  // weapon
        {m_wasd[0], m_wasd[1], m_wasd[2], m_wasd[3]},
        m_space
    };
}

void Player::PushNewInput(const util::InputState& new_input) {
    if(!new_input.sequence_number) {
        return;
    }
    m_latestSeq = std::max(m_latestSeq, new_input.sequence_number);
    for(int i = 0; i < 4; i++) {
        if( new_input.wasd_pressed[i] == 1) m_wasd[i] = 1; // pressed
        else if(new_input.wasd_pressed[i] == 2) m_wasd[i] = 0; // released
    }
    if(new_input.space_pressed == 1) m_space = 1; // pressed
    else if(new_input.space_pressed == 2) m_space = 0; // released

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


}

void Player::UpdateByInput() {
    Vector2 moveOnPlane = {0, 0};;
    if(m_wasd[0] || m_wasd[1] || m_wasd[2] || m_wasd[3] || m_space) {
        moveOnPlane.x = m_wasd[0]*1.0f - 
                        m_wasd[2]*1.0f;
        moveOnPlane.y = m_wasd[3]*1.0f - 
                        m_wasd[1]*1.0f;
    }
    Vector2 mov{moveOnPlane.x, moveOnPlane.y};
    Vector2 fow{GetForward().x, GetForward().z};
    Vector2 fow2;
    fow = Vector2Normalize(fow);
    fow2 = {-fow.y, fow.x};
    mov = Vector2Normalize(mov);
    mov = Vector2Scale(mov, 0.1f);

    Vector2 delta {
        fow.x*mov.x + fow2.x * mov.y, 
        fow.y*mov.x + fow2.y * mov.y
    };
    if(abs(moveOnPlane.x) > 0 || abs(moveOnPlane.y) > 0) {
        m_velocity.x = delta.x; 
        m_velocity.z = delta.y;
    }

    if(m_velocity.y == 0.0f && m_space) {
        m_velocity.y = 0.2f; // jump velocity
    }
}

LocalPlayer::LocalPlayer() {
    m_entityType = EntityType::LocalPlayer;
    m_lastTicks = 0;
}

LocalPlayer::LocalPlayer(const Player& player) :
Entity(player),
Player(player) {
    m_entityType = EntityType::LocalPlayer;
    m_lastTicks = 0;

    spdlog::debug("player's wasd: {}, {}, {}, {}", m_wasd[0], m_wasd[1], m_wasd[2], m_wasd[3]);
}

void LocalPlayer::PhysicsUpdate() {
    Player::PhysicsUpdate();
    m_lastTicks++;
}

void LocalPlayer::PushNewInput(const util::InputState& new_input) {
    Player::PushNewInput(new_input);
    if(new_input.sequence_number) {
        m_inputQueue.push_back({new_input, m_lastTicks});
        m_lastTicks = 0; 
    } 
}
