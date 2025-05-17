#ifndef _PLAYER_H_
#define _PLAYER_H_
#include "raylib.h"

struct PlayerState {
    Vector3 position; 
    Vector3 quarternion; 
    uint32_t health; 
    uint32_t weapon;
};

class Player: virtual public Entity {

    public: 

    PlayerState GetState() const; 

    private: 
    PlayerState m_state; 

};

class LocalPlayer: public Player{ 

    std::deque<InputState> m_inputque; 

}



#endif