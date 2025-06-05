#ifndef _INPUT_H_
#define _INPUT_H_
#include "raylib.h"
#include "../Util/Util.h"
#include <inttypes.h>
#include <mutex>


class InputManager {
    public: 
    static InputManager& GetInst() {
        static InputManager Inst;
        return Inst; 
    }

    void CheckInput();
    util::InputState Peek(); 
    util::InputState Pop();

    private: 
    InputManager():m_squenceNumber(1), p_backBuffer(&m_BufferA), 
                    p_frontBuffer(&m_BufferB) 
                    {
                        memset(m_wasd, 0, sizeof(m_wasd));
                        memset(m_mouse, 0, sizeof(m_mouse));
                    };

    util::InputState m_BufferA, m_BufferB;
    util::InputState *p_backBuffer, *p_frontBuffer;

    // counter 
    uint32_t m_squenceNumber;

    // sync related 
    std::mutex m_swapMutex;
    bool i_consumed;

    // press or not pressed 
    bool m_wasd[4], m_space;
    bool m_mouse[2];
};

#ifndef IM 
#define IM InputManager::GetInst()
#endif


#endif 