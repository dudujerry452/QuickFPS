/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.9.1 */

#ifndef PB_GAMEDATA_PB_H_INCLUDED
#define PB_GAMEDATA_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
/* 嵌套的 Vector2 定义保持不变 */
typedef struct _PInputState_Vector2 {
    float x;
    float y;
} PInputState_Vector2;

/* -------------------
    InputState
 ------------------- */
typedef struct _PInputState {
    uint32_t player_id;
    /* 【优化点 1】: 将 repeated 字段改为固定大小的数组。
 这需要在 .options 文件中配合 max_count:4 使用。
 这避免了使用复杂的 pb_callback_t，可以直接进行数组拷贝。 */
    pb_size_t wasd_pressed_count;
    uint32_t wasd_pressed[4];
    uint32_t space_pressed;
    bool has_mouseDelta;
    PInputState_Vector2 mouseDelta;
    uint32_t sequence_number;
} PInputState;

typedef struct _PEntityState_Vector3 {
    float x;
    float y;
    float z;
} PEntityState_Vector3;

/* -------------------
   EntityState
 ------------------- */
typedef struct _PEntityState {
    uint32_t id;
    bool is_error;
    bool has_position;
    PEntityState_Vector3 position;
    bool has_forward;
    PEntityState_Vector3 forward;
    bool has_velocity;
    PEntityState_Vector3 velocity;
    bool has_bounding_box_min;
    PEntityState_Vector3 bounding_box_min;
    bool has_bounding_box_max;
    PEntityState_Vector3 bounding_box_max;
    bool has_pos_point;
    PEntityState_Vector3 pos_point;
    bool is_player;
    uint32_t seq_num;
    uint32_t health;
    uint32_t weapon;
    pb_size_t wasd_count;
    bool wasd[4];
    bool space;
    uint32_t lastticks;
} PEntityState;

typedef struct _ClientHello {
    uint32_t code;
} ClientHello;

typedef struct _ServerHello {
    uint32_t code;
    uint32_t playerid;
} ServerHello;

/* -------------------
 EntityStateBatch
 -------------------
 这个消息用于一次性发送多个实体状态，保持不变。 */
typedef struct _EntityStateBatch {
    pb_size_t entity_state_count;
    PEntityState entity_state[20];
} EntityStateBatch;

typedef struct _GameMessage {
    pb_size_t which_payload;
    union {
        PInputState input_state;
        EntityStateBatch entity_state_batch;
        ClientHello chello;
        ServerHello shello;
    } payload;
} GameMessage;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define PInputState_init_default                 {0, 0, {0, 0, 0, 0}, 0, false, PInputState_Vector2_init_default, 0}
#define PInputState_Vector2_init_default         {0, 0}
#define PEntityState_init_default                {0, 0, false, PEntityState_Vector3_init_default, false, PEntityState_Vector3_init_default, false, PEntityState_Vector3_init_default, false, PEntityState_Vector3_init_default, false, PEntityState_Vector3_init_default, false, PEntityState_Vector3_init_default, 0, 0, 0, 0, 0, {0, 0, 0, 0}, 0, 0}
#define PEntityState_Vector3_init_default        {0, 0, 0}
#define ClientHello_init_default                 {0}
#define ServerHello_init_default                 {0, 0}
#define EntityStateBatch_init_default            {0, {PEntityState_init_default, PEntityState_init_default, PEntityState_init_default, PEntityState_init_default, PEntityState_init_default, PEntityState_init_default, PEntityState_init_default, PEntityState_init_default, PEntityState_init_default, PEntityState_init_default, PEntityState_init_default, PEntityState_init_default, PEntityState_init_default, PEntityState_init_default, PEntityState_init_default, PEntityState_init_default, PEntityState_init_default, PEntityState_init_default, PEntityState_init_default, PEntityState_init_default}}
#define GameMessage_init_default                 {0, {PInputState_init_default}}
#define PInputState_init_zero                    {0, 0, {0, 0, 0, 0}, 0, false, PInputState_Vector2_init_zero, 0}
#define PInputState_Vector2_init_zero            {0, 0}
#define PEntityState_init_zero                   {0, 0, false, PEntityState_Vector3_init_zero, false, PEntityState_Vector3_init_zero, false, PEntityState_Vector3_init_zero, false, PEntityState_Vector3_init_zero, false, PEntityState_Vector3_init_zero, false, PEntityState_Vector3_init_zero, 0, 0, 0, 0, 0, {0, 0, 0, 0}, 0, 0}
#define PEntityState_Vector3_init_zero           {0, 0, 0}
#define ClientHello_init_zero                    {0}
#define ServerHello_init_zero                    {0, 0}
#define EntityStateBatch_init_zero               {0, {PEntityState_init_zero, PEntityState_init_zero, PEntityState_init_zero, PEntityState_init_zero, PEntityState_init_zero, PEntityState_init_zero, PEntityState_init_zero, PEntityState_init_zero, PEntityState_init_zero, PEntityState_init_zero, PEntityState_init_zero, PEntityState_init_zero, PEntityState_init_zero, PEntityState_init_zero, PEntityState_init_zero, PEntityState_init_zero, PEntityState_init_zero, PEntityState_init_zero, PEntityState_init_zero, PEntityState_init_zero}}
#define GameMessage_init_zero                    {0, {PInputState_init_zero}}

/* Field tags (for use in manual encoding/decoding) */
#define PInputState_Vector2_x_tag                1
#define PInputState_Vector2_y_tag                2
#define PInputState_player_id_tag                1
#define PInputState_wasd_pressed_tag             2
#define PInputState_space_pressed_tag            3
#define PInputState_mouseDelta_tag               4
#define PInputState_sequence_number_tag          5
#define PEntityState_Vector3_x_tag               1
#define PEntityState_Vector3_y_tag               2
#define PEntityState_Vector3_z_tag               3
#define PEntityState_id_tag                      1
#define PEntityState_is_error_tag                2
#define PEntityState_position_tag                3
#define PEntityState_forward_tag                 4
#define PEntityState_velocity_tag                5
#define PEntityState_bounding_box_min_tag        6
#define PEntityState_bounding_box_max_tag        7
#define PEntityState_pos_point_tag               8
#define PEntityState_is_player_tag               9
#define PEntityState_seq_num_tag                 10
#define PEntityState_health_tag                  11
#define PEntityState_weapon_tag                  12
#define PEntityState_wasd_tag                    13
#define PEntityState_space_tag                   14
#define PEntityState_lastticks_tag               15
#define ClientHello_code_tag                     1
#define ServerHello_code_tag                     1
#define ServerHello_playerid_tag                 2
#define EntityStateBatch_entity_state_tag        1
#define GameMessage_input_state_tag              1
#define GameMessage_entity_state_batch_tag       2
#define GameMessage_chello_tag                   3
#define GameMessage_shello_tag                   4

/* Struct field encoding specification for nanopb */
#define PInputState_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   player_id,         1) \
X(a, STATIC,   REPEATED, UINT32,   wasd_pressed,      2) \
X(a, STATIC,   SINGULAR, UINT32,   space_pressed,     3) \
X(a, STATIC,   OPTIONAL, MESSAGE,  mouseDelta,        4) \
X(a, STATIC,   SINGULAR, UINT32,   sequence_number,   5)
#define PInputState_CALLBACK NULL
#define PInputState_DEFAULT NULL
#define PInputState_mouseDelta_MSGTYPE PInputState_Vector2

#define PInputState_Vector2_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, FLOAT,    x,                 1) \
X(a, STATIC,   SINGULAR, FLOAT,    y,                 2)
#define PInputState_Vector2_CALLBACK NULL
#define PInputState_Vector2_DEFAULT NULL

#define PEntityState_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   id,                1) \
X(a, STATIC,   SINGULAR, BOOL,     is_error,          2) \
X(a, STATIC,   OPTIONAL, MESSAGE,  position,          3) \
X(a, STATIC,   OPTIONAL, MESSAGE,  forward,           4) \
X(a, STATIC,   OPTIONAL, MESSAGE,  velocity,          5) \
X(a, STATIC,   OPTIONAL, MESSAGE,  bounding_box_min,   6) \
X(a, STATIC,   OPTIONAL, MESSAGE,  bounding_box_max,   7) \
X(a, STATIC,   OPTIONAL, MESSAGE,  pos_point,         8) \
X(a, STATIC,   SINGULAR, BOOL,     is_player,         9) \
X(a, STATIC,   SINGULAR, UINT32,   seq_num,          10) \
X(a, STATIC,   SINGULAR, UINT32,   health,           11) \
X(a, STATIC,   SINGULAR, UINT32,   weapon,           12) \
X(a, STATIC,   REPEATED, BOOL,     wasd,             13) \
X(a, STATIC,   SINGULAR, BOOL,     space,            14) \
X(a, STATIC,   SINGULAR, UINT32,   lastticks,        15)
#define PEntityState_CALLBACK NULL
#define PEntityState_DEFAULT NULL
#define PEntityState_position_MSGTYPE PEntityState_Vector3
#define PEntityState_forward_MSGTYPE PEntityState_Vector3
#define PEntityState_velocity_MSGTYPE PEntityState_Vector3
#define PEntityState_bounding_box_min_MSGTYPE PEntityState_Vector3
#define PEntityState_bounding_box_max_MSGTYPE PEntityState_Vector3
#define PEntityState_pos_point_MSGTYPE PEntityState_Vector3

#define PEntityState_Vector3_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, FLOAT,    x,                 1) \
X(a, STATIC,   SINGULAR, FLOAT,    y,                 2) \
X(a, STATIC,   SINGULAR, FLOAT,    z,                 3)
#define PEntityState_Vector3_CALLBACK NULL
#define PEntityState_Vector3_DEFAULT NULL

#define ClientHello_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   code,              1)
#define ClientHello_CALLBACK NULL
#define ClientHello_DEFAULT NULL

#define ServerHello_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   code,              1) \
X(a, STATIC,   SINGULAR, UINT32,   playerid,          2)
#define ServerHello_CALLBACK NULL
#define ServerHello_DEFAULT NULL

#define EntityStateBatch_FIELDLIST(X, a) \
X(a, STATIC,   REPEATED, MESSAGE,  entity_state,      1)
#define EntityStateBatch_CALLBACK NULL
#define EntityStateBatch_DEFAULT NULL
#define EntityStateBatch_entity_state_MSGTYPE PEntityState

#define GameMessage_FIELDLIST(X, a) \
X(a, STATIC,   ONEOF,    MESSAGE,  (payload,input_state,payload.input_state),   1) \
X(a, STATIC,   ONEOF,    MESSAGE,  (payload,entity_state_batch,payload.entity_state_batch),   2) \
X(a, STATIC,   ONEOF,    MESSAGE,  (payload,chello,payload.chello),   3) \
X(a, STATIC,   ONEOF,    MESSAGE,  (payload,shello,payload.shello),   4)
#define GameMessage_CALLBACK NULL
#define GameMessage_DEFAULT NULL
#define GameMessage_payload_input_state_MSGTYPE PInputState
#define GameMessage_payload_entity_state_batch_MSGTYPE EntityStateBatch
#define GameMessage_payload_chello_MSGTYPE ClientHello
#define GameMessage_payload_shello_MSGTYPE ServerHello

extern const pb_msgdesc_t PInputState_msg;
extern const pb_msgdesc_t PInputState_Vector2_msg;
extern const pb_msgdesc_t PEntityState_msg;
extern const pb_msgdesc_t PEntityState_Vector3_msg;
extern const pb_msgdesc_t ClientHello_msg;
extern const pb_msgdesc_t ServerHello_msg;
extern const pb_msgdesc_t EntityStateBatch_msg;
extern const pb_msgdesc_t GameMessage_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define PInputState_fields &PInputState_msg
#define PInputState_Vector2_fields &PInputState_Vector2_msg
#define PEntityState_fields &PEntityState_msg
#define PEntityState_Vector3_fields &PEntityState_Vector3_msg
#define ClientHello_fields &ClientHello_msg
#define ServerHello_fields &ServerHello_msg
#define EntityStateBatch_fields &EntityStateBatch_msg
#define GameMessage_fields &GameMessage_msg

/* Maximum encoded size of messages (where known) */
#define ClientHello_size                         6
#define EntityStateBatch_size                    2980
#define GAMEDATA_PB_H_MAX_SIZE                   GameMessage_size
#define GameMessage_size                         2983
#define PEntityState_Vector3_size                15
#define PEntityState_size                        146
#define PInputState_Vector2_size                 10
#define PInputState_size                         54
#define ServerHello_size                         12

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
