#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../Game/World.h"
#include "../Sync/Sync.h"

class Renderer {

    public: 

    Renderer() {};

    void Prepare(RenderStateBuffer&& buffer);
    void Render();

    private:

    RenderStateBuffer m_frontBuffer, m_backBuffer;

    
};

#endif 