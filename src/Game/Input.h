#ifndef _INPUT_H_
#define _INPUT_H_
#include "raylib.h"
#include <inttypes.h>

struct InputState {
    Vector2 moveOnPlane;
    Vector2 mouseDelta;
    bool isShooting = false;
    bool isJumping = false;
    uint32_t sequence_number = 0;
};

class InputManager {
    public: 
    static InputManager& GetInst() {
        static InputManager Inst;
        return Inst; 
    }

    void CheckInput();
    InputState Peek(); 

    private: 
    InputManager() {};

    InputState m_state;
};

#ifndef IM 
#define IM InputManager::GetInst()
#endif


#endif 