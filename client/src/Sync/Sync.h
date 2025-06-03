#ifndef _SYNC_H_
#define _SYNC_H_

#include "../Game/World.h"

struct RenderStateBuffer {
    std::vector<Entity> entities;
    std::vector<MapObject> objects;
    Camera camera;

    uint32_t pfps;
};

struct SyncState {
    std::mutex render_mtx; // 交换数据的锁
    bool data_is_consumed;
};

#endif