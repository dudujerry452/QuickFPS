#ifndef _WORLD_H_
#define _WORLD_H_

#include "Map.h"
#include "Entity.h"
#include "Player.h"
#include <map>
#include <vector>
#include <memory>
#include <inttypes.h>

#define MAX_COLUMNS 20

class World {
    
    public: 

    // load and set world. TODO 
    World(); 
    ~World(); 


    // set & get
    void SetMap(const Map& themap) { m_worldMap = themap; }
    Map GetMap() const {return m_worldMap; }
    auto& GetEntity(uint32_t obj_id) {return m_entities[obj_id]; }
    uint32_t AddEntity(std::unique_ptr<Entity>&& obj_ptr);
    bool DelObject(uint32_t obj_id);

    // player set & get
    // uint32_t AddPlayer(std::unique_ptr<Player>&& player_ptr, bool is_local = false);
    // auto GetPlayers() {return m_players;}
    // auto GetLocalPlayer() {return m_localPlayer;}

    // runtime 
    bool IsCollide(uint32_t obj_id);

    // update related
    void WorldPhysicsUpdate(); 
    void WorldAnimeUpdate();
    


    private: 
    // Data ------------------
    Map m_worldMap; 

    // Runtime -----------------
    std::map<uint32_t, std::unique_ptr<Entity> > m_entities;

    private: 

    // meta 

    static uint32_t NewID();
    
};


#endif