#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "raylib.h"
#include <inttypes.h>

class Entity {
    public: 
        Vector3 GetPos() const;
        
    private: 
        uint32_t m_id;
        Vector3 m_pos;
};


#endif