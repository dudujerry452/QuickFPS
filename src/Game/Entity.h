#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "raylib.h"
#include "raymath.h"
#include <inttypes.h>


class Entity {
    public: 
        Entity(); 
        virtual ~Entity() {};

        // Data related
        uint32_t GetID() const {return m_id;} 
        void SetID(uint32_t id) {m_id = id;}
        void SetError(bool value) { m_isError = value; }
        bool IsError() {return m_isError;}

        // Physics related
        Vector3 GetPos() const {return m_pos;}
        Vector3 GetForward() const {return m_forward;} 
        void SetPos(Vector3 pos) {m_pos = pos;}
        void SetForward(Vector3 forward) {m_forward = Vector3Normalize(forward);}
        void SetBoundingBox(const BoundingBox& obj_box) {m_boundingBox = obj_box; }
        BoundingBox GetBoundingBox() const {return m_boundingBox; }
        
        // Update related 
        virtual void PhysicsUpdate() {}; 
        virtual void AnimeUpdate() {}; 

    protected: 
        uint32_t m_id;
        bool m_isError;

        Vector3 m_pos;
        Vector3 m_forward;

        BoundingBox m_boundingBox;
};


#endif