#ifndef _UTIL_H_
#define _UTIL_H_
#include "raymath.h"
#include <stdexcept>
#include <inttypes.h>

// C++17 a helper for creating an overloaded visitor
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

// C++17 deduction guide (makes it easy to use)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

namespace util {

// for raylib

typedef struct BoundingBox {
    Vector3 min;            // Minimum vertex box-corner
    Vector3 max;            // Maximum vertex box-corner
} BoundingBox;

// custom 

/**
 * InputState & EntityState : 
 * network communication structs
 * if modify, these parts should be changed : 
 *  - whole Serialize ( proto )
 *  - InputManager
 *  - Entity - GetState
 *  - Entity - GetEntityFromState
 *  - Player - UpdateByInput 
 *  - World -  PushInput
 *           - PrepareState
 *          -  ProvideUpdater
 *          -  GetUpdater
 *  - CAPI - CInputState
 *         - InputConvert
 *  So THINK about it before you modify them
 */





struct InputState {
    uint32_t player_id; 
    unsigned char wasd_pressed[4]; // 0: no op; 1: press; 2: release 
    unsigned char space_pressed; // space key for jumping
    Vector2 mouseDelta; // mouse movement delta
    uint32_t sequence_number = 0; // sequence number for input events
}; 

struct EntityState{
    uint32_t id; 
    unsigned char is_error;
    Vector3 position; 
    Vector3 forward; 
    Vector3 velocity; 
    BoundingBox bounding_box; 
    Vector3 pos_point; 
    unsigned char is_player; 
    uint32_t seq_num; 
    uint32_t health; 
    uint32_t weapon;
    unsigned char wasd[4];
    unsigned char space;
    uint32_t lastticks; 
};

struct ClientHello {
    uint32_t code;
};

struct ServerHello {
    uint32_t code;
    uint32_t playerid;
};


// colors 

typedef struct Color {
    unsigned char r;        // Color red value
    unsigned char g;        // Color green value
    unsigned char b;        // Color blue value
    unsigned char a;        // Color alpha value
} Color;

typedef struct Color Color; 

const  Color  ILIGHTGRAY = { 200, 200, 200, 255 };   // Light Gray
const  Color  IGRAY      = { 130, 130, 130, 255 };   // Gray
const  Color  IDARKGRAY  = { 80, 80, 80, 255 }   ;   // Dark Gray
const  Color  IYELLOW    = { 253, 249, 0, 255 }  ;   // Yellow
const  Color  IGOLD      = { 255, 203, 0, 255 }  ;   // Gold
const  Color  IORANGE    = { 255, 161, 0, 255 }  ;   // Orange
const  Color  IPINK      = { 255, 109, 194, 255 };   // Pink
const  Color  IRED       = { 230, 41, 55, 255 }  ;   // Red
const  Color  IMAROON    = { 190, 33, 55, 255 }  ;   // Maroon
const  Color  IGREEN     = { 0, 228, 48, 255 }   ;   // Green
const  Color  ILIME      = { 0, 158, 47, 255 }   ;   // Lime
const  Color  IDARKGREEN = { 0, 117, 44, 255 }   ;   // Dark Green
const  Color  ISKYBLUE   = { 102, 191, 255, 255 };   // Sky Blue
const  Color  IBLUE      = { 0, 121, 241, 255 }  ;   // Blue
const  Color  IDARKBLUE  = { 0, 82, 172, 255 }   ;   // Dark Blue
const  Color  IPURPLE    = { 200, 122, 255, 255 };   // Purple
const  Color  IVIOLET    = { 135, 60, 190, 255 } ;   // Violet
const  Color  IDARKPURPLE= { 112, 31, 126, 255 } ;   // Dark Purple
const  Color  IBEIGE     = { 211, 176, 131, 255 };   // Beige
const  Color  IBROWN     = { 127, 106, 79, 255 } ;   // Brown
const  Color  IDARKBROWN = { 76, 63, 47, 255 }   ;   // Dark Brown

const  Color  IWHITE     = { 255, 255, 255, 255 };   // White
const  Color  IBLACK     = { 0, 0, 0, 255 }      ;   // Black
const  Color  IBLANK     = { 0, 0, 0, 0 }        ;   // Blank (Transparent)
const  Color  IMAGENTA   = { 255, 0, 255, 255 }  ;   // Magenta
const  Color  IRAYWHITE  = { 245, 245, 245, 255 };   // My own White (raylib logo)

}
#ifndef RAYLIB_H
#endif
#endif