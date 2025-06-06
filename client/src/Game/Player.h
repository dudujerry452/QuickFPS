#ifndef _PLAYER_H_
#define _PLAYER_H_
#include "../Util/Util.h"
#include "Entity.h"

#include <queue>


class Player: virtual public Entity {

    public: 

    Player();
    virtual ~Player() {};

    Player& operator=(Player& other) {
        if (this == &other) return *this; // self-assignment check
        Entity::operator=(other);
        m_health = other.m_health;
        m_weapon = other.m_weapon;
        return *this;
    }

    Player* Clone() const override {
        return new Player(*this);
    }

    // Physics related
    void PhysicsUpdate() override;

    // Input related
    util::EntityState GetState() const override; 
    virtual void PushNewInput(const util::InputState& new_input);
    // void ApplyAuthInput(const util::PlayerState& auth_state);

    // Game related
    void SetHealth(uint32_t health) { m_health = health; }
    uint32_t GetHealth() const { return m_health; }
    void SetWeapon(uint32_t weapon) { m_weapon = weapon; }
    uint32_t GetWeapon() const { return m_weapon; }

    protected:
    // ------ deal with input --------

    protected: 
    uint32_t m_health; 
    uint32_t m_weapon;

};

class LocalPlayer: virtual public Player{ 
 // 这里的m_state代表预测的玩家状态

    public: 

    LocalPlayer();
    ~LocalPlayer() override {}

    LocalPlayer* Clone() const override {
        return new LocalPlayer(*this);
    }

    LocalPlayer& operator=(LocalPlayer& other) {
        if (this == &other) return *this; // self-assignment check
        Player::operator=(other); 
        // not copy input queue
        return *this;
    }

    // void PhysicsUpdate() override;
    void PushNewInput(const util::InputState& new_input) override;

    

    private: 
    std::deque<util::InputState> m_inputQueue; 
    private: 

};


#endif