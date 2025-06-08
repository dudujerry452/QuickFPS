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
        m_latestSeq = other.m_latestSeq;
        m_health = other.m_health;
        m_weapon = other.m_weapon;
        memcpy(m_wasd, other.m_wasd, 4);
        m_space = other.m_space;
        return *this;
    }

    Player* Clone() const override {
        return new Player(*this);
    }

    // Physics related
    void PhysicsUpdate() override;

    // Input related
    util::EntityState GetState() const override; 
    uint32_t GetLatestSeq() const { return m_latestSeq; }
    virtual void PushNewInput(const util::InputState& new_input);
    void UpdateByInput(); 
    // void ApplyAuthInput(const util::PlayerState& auth_state);

    // Game related
    void SetHealth(uint32_t health) { m_health = health; }
    uint32_t GetHealth() const { return m_health; }
    void SetWeapon(uint32_t weapon) { m_weapon = weapon; }
    uint32_t GetWeapon() const { return m_weapon; }


    friend class World;
    friend std::unique_ptr<Entity> GetEntityFromState(util::EntityState state);  

    protected:
    // ------ deal with input --------

    protected: 

    // input related 
    uint32_t m_latestSeq; 
    unsigned char m_wasd[4]; // 0: no op; 1: press; 2: release
    unsigned char m_space; // space key for jumping
    uint32_t m_lastTicks; 

    uint32_t m_health; 
    uint32_t m_weapon;



};

class LocalPlayer: virtual public Player{ 
 // 这里的m_state代表预测的玩家状态

    public: 

    LocalPlayer();
    LocalPlayer(const Entity& entity);
    LocalPlayer(const Player& player); 
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

    void PhysicsUpdate() override;
    void PushNewInput(const util::InputState& new_input) override;
    uint32_t GetOldestSeq() const { 
        if(m_inputQueue.empty()) return 0;
        return m_inputQueue.front().first.sequence_number; 
    }

    friend class World;

    private: 
    std::deque<std::pair<util::InputState, uint32_t> > m_inputQueue;
    private: 

};


#endif