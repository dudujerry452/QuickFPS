#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../Game/World.h"
#include "../Sync/Sync.h"

class Renderer {

    public: 

    Renderer():i_consumed(1), m_backBuffer(&m_BufferB), m_frontBuffer(&m_BufferA) {};

    void Prepare(RenderStateBuffer&& buffer);
    void Render();

    private:

    RenderStateBuffer *m_frontBuffer, *m_backBuffer;
    RenderStateBuffer m_BufferA, m_BufferB;

    private: // multithread mutex
    std::mutex m_swapMutex;
    bool i_consumed;
    
};

#endif 