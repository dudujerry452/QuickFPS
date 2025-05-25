#ifndef _WORLD_H_
#define _WORLD_H_

#include "Map.h"
#include "Entity.h"
#include "Player.h"
#include "Camera.h"
#include <map>
#include <vector>
#include <memory>
#include <assert.h>
#include <inttypes.h>

#include "spdlog/spdlog.h"

#define MAX_COLUMNS 20

struct RenderStateBuffer;

class World {
    
    public: 

    // load and set world. TODO 
    World(); 
    ~World(); 


    // set & get
    void SetMap(const Map& themap) { m_worldMap = themap; }
    Map GetMap() const {return m_worldMap; }
    std::unique_ptr<Entity>& GetEntity(uint32_t obj_id);

    // single thread 
    uint32_t AddEntity(std::unique_ptr<Entity>&& obj_ptr);
    bool DelObject(uint32_t obj_id);

    // player set & get
    void SetLocalPlayer(uint32_t player_id) {
        assert(m_entities.find(player_id) != m_entities.end());
        spdlog::debug("Set LocalPlayer: {}", player_id);
        m_localPlayer = player_id;
    }
    uint32_t GetLocalPlayer() const { return m_localPlayer; }

    // uint32_t AddPlayer(std::unique_ptr<Player>&& player_ptr, bool is_local = false);
    // auto GetPlayers() {return m_players;}
    // auto GetLocalPlayer() {return m_localPlayer;}

    // runtime 
    bool IsCollide(uint32_t obj_id);

    // update related
    void WorldPhysicsUpdate(); 
    void WorldAnimeUpdate();

    // camera control 
    void Attach(uint32_t entity_id);
    void Attach();

    // render data transmit
    RenderStateBuffer GetRenderState();
    


    private: 
    // Data ------------------
    Map m_worldMap; 

    // Runtime -----------------
    std::unordered_map<uint32_t, std::unique_ptr<Entity> > m_entities;
    uint32_t m_localPlayer;
    WorldCamera m_camera;

    private: 

    // meta 
    static uint32_t NewID();
    
};


#endif