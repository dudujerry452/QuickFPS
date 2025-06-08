#include "c_api.h"
#include "../Game/World.h"
#include "../Serialize/Serialize.h"

WorldHandle CreateWorld() {
    auto world = new World();
    return reinterpret_cast<WorldHandle>(world);
}
void DestroyWorld(WorldHandle world) {
    auto w = reinterpret_cast<World*>(world);
    delete w;
}
void WorldUpdate(WorldHandle world) {
    auto w = reinterpret_cast<World*>(world);
    w->WorldUpdate();
}

uint32_t AddPlayer(WorldHandle world) {
    auto w = reinterpret_cast<World*>(world);
    auto player = std::make_unique<LocalPlayer>();
    uint32_t id = w->AddEntity(std::move(player));
    w->SetLocalPlayer(id);
    w->Attach(id);
    return id;
}
void RemovePlayer(WorldHandle world, uint32_t player_id) {
    auto w = reinterpret_cast<World*>(world);
    w->DelObject(player_id);
    if(w->GetLocalPlayer() == player_id) {
        w->SetLocalPlayer(0); // reset local player
    }
}

void SetPlayerPosition(WorldHandle world, uint32_t player_id, 
                              float pos_x, float pos_y, float pos_z) {
    auto w = reinterpret_cast<World*>(world);
    auto& player = w->GetEntity(player_id);
    if(player) {
        player->SetPos({pos_x, pos_y, pos_z});
    }
}
void SetPlayerForward(WorldHandle world, uint32_t player_id,
                              float forward_x, float forward_y, float forward_z) {
    auto w = reinterpret_cast<World*>(world);
    auto& player = w->GetEntity(player_id);
    if(player) {
        player->SetForward({forward_x, forward_y, forward_z});
    }
}
void SetPlayerVelocity(WorldHandle world, uint32_t player_id,
                              float velocity_x, float velocity_y, float velocity_z) {
    auto w = reinterpret_cast<World*>(world);
    auto& player = w->GetEntity(player_id);
    if(player) {
        player->SetVelocity({velocity_x, velocity_y, velocity_z});
    }
}
void SetPlayerBoundingBox(WorldHandle world, uint32_t player_id,
                              float box_min_x, float box_min_y, float box_min_z,
                              float box_max_x, float box_max_y, float box_max_z) {
    auto w = reinterpret_cast<World*>(world);
    auto& player = w->GetEntity(player_id);
    if(player) {
        player->SetBoundingBox({
            {box_min_x, box_min_y, box_min_z},
            {box_max_x, box_max_y, box_max_z}
        });
    }
}
void SetPlayerHealth(WorldHandle world, uint32_t player_id, uint32_t health) {
    auto w = reinterpret_cast<World*>(world);
    auto& player = w->GetEntity(player_id);
    if(player) {
        if(auto p = dynamic_cast<Player*>(player.get())) {
            p->SetHealth(health);
        }
    }
}
void SetPlayerWeapon(WorldHandle world, uint32_t player_id, uint32_t weapon) {
    auto w = reinterpret_cast<World*>(world);
    auto& player = w->GetEntity(player_id);
    if(player) {
        if(auto p = dynamic_cast<Player*>(player.get())) {
            p->SetWeapon(weapon);
        }
    }
}


int GetAllEntitiesState(WorldHandle world, uint8_t** out_data, uint32_t* out_size) {
    auto w = reinterpret_cast<World*>(world);
    auto ent_list = w->GetUpdater(); 
    auto bs = serialization::serialize(ent_list); 
    if(!bs.has_value()) {
        return 1; // error code 
    }
    auto& b = bs.value();

    *out_data = (uint8_t*)malloc(b.size());
    if(*out_data == nullptr) {
        *out_size = 0;
        return 2;
    }

    memcpy(*out_data, b.data(), b.size());
    *out_size = b.size();
    
    return 0; //SUCCESS
}

util::InputState InputConvert(CInputState* input) {
    return {
        input->player_id,
        {input->wasd_pressed[0], input->wasd_pressed[1], input->wasd_pressed[2], input->wasd_pressed[3]},
        input->space_pressed,
        {input->mouseDeltax, input->mouseDeltay},
        input->sequence_number
    };
}

void PushInputToWorld(WorldHandle world, CInputState *input) {
    auto w = reinterpret_cast<World*>(world);
    if(!w) {
        return;
    }
    w->PushInput(InputConvert(input));
}