#ifndef _C_API_H_
#define _C_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef void* WorldHandle; 
WorldHandle CreateWorld();
void DestroyWorld(WorldHandle world);
void WorldUpdate(WorldHandle world);

uint32_t AddPlayer(WorldHandle world); // return player id 
void RemovePlayer(WorldHandle world, uint32_t player_id); // return 0 if success, else error code

void SetPlayerPosition(WorldHandle world, uint32_t player_id, 
                             float pos_x, float pos_y, float pos_z);
void SetPlayerForward(WorldHandle world, uint32_t player_id,
                             float forward_x, float forward_y, float forward_z);
void SetPlayerVelocity(WorldHandle world, uint32_t player_id,
                             float velocity_x, float velocity_y, float velocity_z);
void SetPlayerBoundingBox(WorldHandle world, uint32_t player_id,
                             float box_min_x, float box_min_y, float box_min_z,
                             float box_max_x, float box_max_y, float box_max_z);
void SetPlayerHealth(WorldHandle world, uint32_t player_id, uint32_t health);
void SetPlayerWeapon(WorldHandle world, uint32_t player_id, uint32_t weapon);
// void PushPlayerInput(WorldHandle world, uint32_t player_id, 
//                      float move_x, float move_y, 
//                      float mouse_delta_x, float mouse_delta_y, 
//                      int is_jumping);

#ifdef __cplusplus
}
#endif

#endif