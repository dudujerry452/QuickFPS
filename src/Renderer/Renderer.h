#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../Game/World.h"
#include "Camera.h"

class Renderer {

    public: 


    void Init(); 
    // attach view to a object
    void Attach(World* render_target, uint32_t entity_id);
    // attach view to player
    void Attach(World* render_target); 
    void Render(World* render_target);
    Camera& GetCamera() {return m_camera.GetCamera(); }

    private:

    WorldCamera m_camera;

};

#endif 