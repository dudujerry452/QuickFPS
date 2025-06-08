#ifndef _SERIALIZE_TOOL_H_
#define _SERIALIZE_TOOL_H_

#include "c_protos/gamedata.pb.h"
#include "../Util/Util.h"

#include <vector> 
#include <cstdint>

// 必须在C++文件中包含C头文件时使用 extern "C"
extern "C" {
#include "c_protos/gamedata.pb.h" // 包含nanopb生成的头文件
}
// ==== Helper Functions for Vector Conversions ====

// Convert from Nanopb's PInputState_Vector2 to local C++ Vector2
Vector2 FromNanopb(const PInputState_Vector2& nanopbVec) {
    return Vector2{nanopbVec.x, nanopbVec.y};
}

// Convert from local C++ Vector2 to Nanopb's PInputState_Vector2
void ToNanopb(const Vector2& nativeVec, PInputState_Vector2* outNanopbVec) {
    if (outNanopbVec) {
        outNanopbVec->x = nativeVec.x;
        outNanopbVec->y = nativeVec.y;
    }
}

// Convert from Nanopb's PEntityState_Vector3 to local C++ Vector3
Vector3 FromNanopb(const PEntityState_Vector3& nanopbVec) {
    return Vector3{nanopbVec.x, nanopbVec.y, nanopbVec.z};
}

// Convert from local C++ Vector3 to Nanopb's PEntityState_Vector3
void ToNanopb(const Vector3& nativeVec, PEntityState_Vector3* outNanopbVec) {
    if (outNanopbVec) {
        outNanopbVec->x = nativeVec.x;
        outNanopbVec->y = nativeVec.y;
        outNanopbVec->z = nativeVec.z;
    }
}


// ==== PInputState Conversions ====

// Convert from Nanopb C struct PInputState to local C++ util::InputState
util::InputState FromNanopb(const PInputState& nanopbInputState) {
    util::InputState nativeState;
    nativeState.player_id = nanopbInputState.player_id;

    // wasd_pressed: Nanopb uint32_t[4] with _count to util::InputState.wasd_pressed (std::array<bool, 4>)
    for (size_t i = 0; i < 4; ++i) {
        // if (i < nanopbInputState.wasd_pressed_count) {
        //     nativeState.wasd_pressed[i] = (nanopbInputState.wasd_pressed[i] != 0);
        // } else {
        //     nativeState.wasd_pressed[i] = false; // Default if not present in nanopb message
        // }
        nativeState.wasd_pressed[i] = nanopbInputState.wasd_pressed[i]; 

    }

    nativeState.space_pressed = (nanopbInputState.space_pressed != 0);

    if (nanopbInputState.has_mouseDelta) {
        nativeState.mouseDelta = FromNanopb(nanopbInputState.mouseDelta);
    } else {
        nativeState.mouseDelta = Vector2(); // Default C++ Vector2
    }

    nativeState.sequence_number = nanopbInputState.sequence_number;
    return nativeState;
}

// Convert from local C++ util::InputState to Nanopb C struct PInputState
// Returns true on success, false if data might be truncated (e.g., wasd_pressed size mismatch if native has more than 4)
bool ToNanopb(const util::InputState& nativeInputState, PInputState* outNanopbInputState) {
    if (!outNanopbInputState) return false;

    // Caller should ensure outNanopbInputState is initialized (e.g., PInputState_init_zero)

    outNanopbInputState->player_id = nativeInputState.player_id;

    // wasd_pressed: util::InputState.wasd_pressed (std::array<bool, 4>) to Nanopb uint32_t[4] with _count
    // Max count for wasd_pressed in PInputState is 4.
    pb_size_t wasd_max_count_in_proto = sizeof(outNanopbInputState->wasd_pressed) / sizeof(outNanopbInputState->wasd_pressed[0]);
    outNanopbInputState->wasd_pressed_count = 0;
    for (size_t i = 0; i < 4; ++i) {
        if (outNanopbInputState->wasd_pressed_count >= wasd_max_count_in_proto) {
            break; // Stop if we've reached the nanopb array's capacity
        }
        outNanopbInputState->wasd_pressed[outNanopbInputState->wasd_pressed_count++] = nativeInputState.wasd_pressed[i] ? 1 : 0;
    }

    outNanopbInputState->space_pressed = nativeInputState.space_pressed ? 1 : 0;

    outNanopbInputState->has_mouseDelta = true; // Assume we always provide mouseDelta
    ToNanopb(nativeInputState.mouseDelta, &outNanopbInputState->mouseDelta);

    outNanopbInputState->sequence_number = nativeInputState.sequence_number;
    return true;
}


// ==== PEntityState Conversions ====

// Convert from Nanopb C struct PEntityState to local C++ util::EntityState
util::EntityState FromNanopb(const PEntityState& nanopbEntityState) {
    util::EntityState nativeState;
    nativeState.id = nanopbEntityState.id;
    nativeState.is_error = nanopbEntityState.is_error;

    if (nanopbEntityState.has_position) nativeState.position = FromNanopb(nanopbEntityState.position);
    else nativeState.position = Vector3();

    if (nanopbEntityState.has_forward) nativeState.forward = FromNanopb(nanopbEntityState.forward);
    else nativeState.forward = Vector3();

    if (nanopbEntityState.has_velocity) nativeState.velocity = FromNanopb(nanopbEntityState.velocity);
    else nativeState.velocity = Vector3();

    if (nanopbEntityState.has_bounding_box_min) nativeState.bounding_box.min = FromNanopb(nanopbEntityState.bounding_box_min);
    else nativeState.bounding_box.min = Vector3();

    if (nanopbEntityState.has_bounding_box_max) nativeState.bounding_box.max = FromNanopb(nanopbEntityState.bounding_box_max);
    else nativeState.bounding_box.max = Vector3();

    if (nanopbEntityState.has_pos_point) nativeState.pos_point = FromNanopb(nanopbEntityState.pos_point);
    else nativeState.pos_point = Vector3();

    nativeState.is_player = nanopbEntityState.is_player;
    nativeState.seq_num = nanopbEntityState.seq_num;
    nativeState.health = nanopbEntityState.health;
    nativeState.weapon = nanopbEntityState.weapon;

    memcpy(&nativeState.wasd, &nanopbEntityState.wasd, sizeof(nanopbEntityState.wasd));
    nativeState.space = nanopbEntityState.space;

    nativeState.lastticks = nanopbEntityState.lastticks;

    return nativeState;
}

// Convert from local C++ util::EntityState to Nanopb C struct PEntityState
bool ToNanopb(const util::EntityState& nativeEntityState, PEntityState* outNanopbEntityState) {
    if (!outNanopbEntityState) return false;

    // Caller should ensure outNanopbEntityState is initialized

    outNanopbEntityState->id = nativeEntityState.id;
    outNanopbEntityState->is_error = nativeEntityState.is_error;

    outNanopbEntityState->has_position = true; ToNanopb(nativeEntityState.position, &outNanopbEntityState->position);
    outNanopbEntityState->has_forward = true;  ToNanopb(nativeEntityState.forward, &outNanopbEntityState->forward);
    outNanopbEntityState->has_velocity = true; ToNanopb(nativeEntityState.velocity, &outNanopbEntityState->velocity);
    outNanopbEntityState->has_bounding_box_min = true; ToNanopb(nativeEntityState.bounding_box.min, &outNanopbEntityState->bounding_box_min);
    outNanopbEntityState->has_bounding_box_max = true; ToNanopb(nativeEntityState.bounding_box.max, &outNanopbEntityState->bounding_box_max);
    outNanopbEntityState->has_pos_point = true; ToNanopb(nativeEntityState.pos_point, &outNanopbEntityState->pos_point);

    outNanopbEntityState->is_player = nativeEntityState.is_player;
    outNanopbEntityState->seq_num = nativeEntityState.seq_num;
    outNanopbEntityState->health = nativeEntityState.health;
    outNanopbEntityState->weapon = nativeEntityState.weapon;

    memcpy(&outNanopbEntityState->wasd, &nativeEntityState.wasd, sizeof(nativeEntityState.wasd));
    outNanopbEntityState->space = nativeEntityState.space;

    outNanopbEntityState->lastticks = nativeEntityState.lastticks;

    return true;
}


// ==== EntityStateBatch Conversions ====

// Convert from Nanopb C struct EntityStateBatch to local C++ util::EntityStateBatch
std::vector<util::EntityState> FromNanopb(const EntityStateBatch& nanopbBatch) {
    std::vector<util::EntityState> nativeBatch;
    for (int i = 0; i < nanopbBatch.entity_state_count; i++) {
        nativeBatch.push_back(FromNanopb(nanopbBatch.entity_state[i]));
    }
    return nativeBatch;
}

// Convert from local C++ util::EntityStateBatch to Nanopb C struct EntityStateBatch
// Returns true on success, false if nativeBatch.entity_states exceeds nanopbBatch's max_count
bool ToNanopb(const std::vector<util::EntityState>& nativeBatch, EntityStateBatch* outNanopbBatch) {
    if (!outNanopbBatch) return false;

    // Caller should ensure outNanopbBatch is initialized

    outNanopbBatch->entity_state_count = nativeBatch.size();
    for (int i = 0; i < nativeBatch.size(); i++) {
        ToNanopb(nativeBatch[i], &outNanopbBatch->entity_state[i]);
    }
    return true;
}

util::ClientHello FromNanopb(const ClientHello& nanopbClientHello) {
    util::ClientHello nativeClientHello;
    nativeClientHello.code = nanopbClientHello.code;
    return nativeClientHello;
}

bool ToNanopb(const util::ClientHello& nativeClientHello, ClientHello* nanopbClientHello) {
    nanopbClientHello->code = nativeClientHello.code;
    return true;
}

util::ServerHello FromNanopb(const ServerHello& nanopbServerHello) {
    util::ServerHello nativeServerHello;
    nativeServerHello.code = nanopbServerHello.code;
    nativeServerHello.playerid = nanopbServerHello.playerid;
    return nativeServerHello;
}

bool ToNanopb(const util::ServerHello& nativeServerHello, ServerHello* nanopbServerHello) {
    nanopbServerHello->code = nativeServerHello.code;
    nanopbServerHello->playerid = nativeServerHello.playerid;
    return true;
}


#endif 