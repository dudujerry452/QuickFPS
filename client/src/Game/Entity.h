#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "../Util/Util.h"
#include <inttypes.h>



/**
 * Entity class
 * export as unique_ptr 
 * export as Clone
 * export as EntityState 
 */
class Entity {

    public: 
    enum class EntityType {
        Base, // most basic
        Player, 
        LocalPlayer
    };

    public: 
        Entity(); 
        virtual ~Entity() {};
        Entity& operator=(Entity& other);

        virtual Entity* Clone() const {
            return new Entity(*this);
        }

        // Data related
        uint32_t GetID() const {return m_id;} 
        void SetID(uint32_t id) {m_id = id;}
        void SetError(bool value) { m_isError = value; }
        bool IsError() {return m_isError;}
        EntityType GetType() const {return m_entityType;}

        virtual util::EntityState GetState() const {
            return util::EntityState{
                m_id, 
                static_cast<unsigned char>(m_isError),
                m_pos, 
                m_forward, 
                m_velocity, 
                m_boundingBox, 
                m_posPoint,
                0, 
                0, // health
                0  // weapon
            };
        }


        // Physics related
        Vector3 GetPos() const {return m_pos;}
        Vector3 GetVelocity() const {return m_velocity; }
        Vector3 GetForward() const {return m_forward;} 
        util::BoundingBox GetBoundingBox() const {return m_boundingBox; }
        util::BoundingBox GetGlobalBoundingBox() const {
            auto v1 = m_boundingBox.min + m_pos - m_posPoint; 
            auto v2 = m_boundingBox.max + m_pos - m_posPoint;
            return {v1, v2};
        }
        void SetPos(Vector3 pos) {m_pos = pos;}
        void SetForward(Vector3 forward) {m_forward = Vector3Normalize(forward);}
        void SetBoundingBox(const util::BoundingBox& obj_box) {m_boundingBox = obj_box; }
        void SetVelocity(Vector3 velo) {m_velocity = velo; } 
        void SetPosPoint(Vector3 point) {m_posPoint = point; }
        

        // Update related 
        virtual void PhysicsUpdate() {}; 
        virtual void AnimeUpdate() {}; 

    friend class Physics;

    protected: 

        // meta data 
        EntityType m_entityType;
        uint32_t m_id;
        bool m_isError;

        // physical variable
        Vector3 m_pos;
        Vector3 m_forward;
        Vector3 m_velocity;

        // Collission ralated
        util::BoundingBox m_boundingBox;
        Vector3 m_posPoint; // where the pos mean in model coor
};


#endif