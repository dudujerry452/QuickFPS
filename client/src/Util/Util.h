#ifndef _UTIL_H_
#define _UTIL_H_
#include "raymath.h"
#include <stdexcept>
#include <inttypes.h>

namespace util {


// custom 


struct InputState {
    uint32_t player_id; 
    uint32_t tick;
    Vector2 moveOnPlane;
    Vector2 mouseDelta;
    bool isShooting = false;
    bool isJumping = false;
    uint32_t sequence_number = 0;
};
struct PlayerState {
    Vector3 position; 
    Vector3 forward; 
    Vector3 velocity; 
    uint32_t health; 
    uint32_t weapon;
};



// for raylib

typedef struct BoundingBox {
    Vector3 min;            // Minimum vertex box-corner
    Vector3 max;            // Maximum vertex box-corner
} BoundingBox;







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