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

// runtime player -------------------
uint32_t AddPlayer(WorldHandle world); // return player id 
unsigned char AddPlayerByID(WorldHandle world, uint32_t player_id);
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

// data export -----------------------
uint32_t GetEntityNumber(WorldHandle world);
int GetAllEntitiesState(WorldHandle world, uint8_t** out_data, uint32_t* out_size); 

typedef struct CInputState {
    uint32_t player_id; 
    unsigned char wasd_pressed[4]; // 0: no op; 1: press; 2: release 
    unsigned char space_pressed; // space key for jumping
    float mouseDeltax; // mouse movement delta
    float mouseDeltay;
    uint32_t sequence_number; // sequence number for input events
} CInputState; 

void PushInputToWorld(WorldHandle world, CInputState* input);



#ifdef __cplusplus
}
#endif

#endif