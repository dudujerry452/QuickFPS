#include "World.h"
#include "inttypes.h"
#include "Physics.h"
#include <assert.h>
#include <iostream> 

using std::cout; 
using std::endl;

namespace util {
int IGetRandomValue(int min, int max)
{
    int value = 0;

    if (min > max)
    {
        int tmp = max;
        max = min;
        min = tmp;
    }

#if defined(SUPPORT_RPRAND_GENERATOR)
    value = rprand_get_value(min, max);
#else
    // WARNING: Ranges higher than RAND_MAX will return invalid results
    // More specifically, if (max - min) > INT_MAX there will be an overflow,
    // and otherwise if (max - min) > RAND_MAX the random value will incorrectly never exceed a certain threshold
    // NOTE: Depending on the library it can be as low as 32767
    if ((unsigned int)(max - min) > (unsigned int)RAND_MAX)
    {
        throw std::runtime_error("GetRandomValue: Range exceeds RAND_MAX, use a different method for larger ranges");
    }

    value = (rand()%(abs(max - min) + 1) + min);
#endif
    return value;
}
}


World::World(): 
m_localPlayer(0), 
m_EntitiesBufferFront(&m_EntitiesBufferA),
m_EntitiesBufferBack(&m_EntitiesBufferB), 
m_UpdaterBufferFront(&m_UpdaterBufferA),
m_UpdaterBufferBack(&m_UpdaterBufferB)
{

    const int MAX_COLUMNS = 1;

    float heights[MAX_COLUMNS] = { 0 };
    Vector3 positions[MAX_COLUMNS] = { 0 };
    util::Color colors[MAX_COLUMNS] = { 0 };

    for (int i = 0; i < MAX_COLUMNS; i++)
    {
        heights[i] = (float)util::IGetRandomValue(1, 12);
        positions[i] = { 2.f, 0.0f , 2.f };
        colors[i] = { (unsigned char)util::IGetRandomValue(20, 255), (unsigned char)util::IGetRandomValue(10, 55), 30, 255 };
    }

    m_worldMap.objects.push_back({util::ILIGHTGRAY, util::BoundingBox({{0.0f, -0.5f, 0.0f}, {32.0f, -0.1f, 32.0f}})});
    m_worldMap.objects.push_back({util::IBLUE,      util::BoundingBox({{ -16.0f, 0.0f, 0.0f }, {-15.0f, 2.5f, 32.0f}})});
    m_worldMap.objects.push_back({util::ILIME,      util::BoundingBox({{ 16.0f, 0.0f, 0.0f }, {17.0f, 2.5f, 32.0f}})});
    m_worldMap.objects.push_back({util::IGOLD,      util::BoundingBox({{ 0.0f, 0.0f, 16.0f }, {32.0f, 2.5f, 17.0f}})});

    for (int i = 0; i < MAX_COLUMNS; i++)
    {
        m_worldMap.objects.push_back({colors[i], util::BoundingBox({positions[i], positions[i] + Vector3({2.0f, heights[i], 2.0f})})});
    }

    auto ptr = std::make_unique<Entity>(); // GetEntity
    ptr->SetError(1);
    m_entities[0] = std::move(ptr);

    SetFPS(60); // default fps = 60
    memset(m_FPS_duration, 0, sizeof(m_FPS_duration));
    m_FPS_cnt = 0;
    m_FPS = 0;

    i_isStateConsumed = true;
    ProvideUpdater();
}

World::~World() {
    
}

std::unique_ptr<Entity>& World::GetEntity(uint32_t obj_id) { 
    if(m_entities.find(obj_id) != m_entities.end())
        return m_entities[obj_id]; 
    else return m_entities[0]; // return error entity
}

uint32_t World::AddEntity(std::unique_ptr<Entity>&& obj_ptr) {
    auto id = NewID();
    obj_ptr->SetID(id);
    m_entities[id] = std::forward<decltype(obj_ptr)>(obj_ptr);
    return id;
}
bool World::DelObject(uint32_t obj_id) {
    return m_entities.erase(obj_id);
}

void World::WorldUpdate() { 

    // handle input with concurrent queue
    std::vector<util::InputState> inputs;
    int size = m_inputQueue.try_dequeue_bulk(
        std::back_inserter(inputs), 1000
    );
    if(size > 0) {
        for(const auto& input: inputs) {
            auto& ent = GetEntity(input.player_id);
            if(ent->IsError()) continue; // not exists
            auto player = dynamic_cast<Player*>(ent.get());
            if(!player) {
                spdlog::error("World::WorldPhysicsUpdate: Entity is not a Player: {}", input.player_id);
                continue; // not player
            }
            player->PushNewInput(input);
        }
    }

    // handle world state update 

    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        if(!i_isStateConsumed) { // if not consumed, consume now
            spdlog::info("world get update state, size: {}", m_EntitiesBufferFront->size());
            m_entities.clear(); // clear all entities
            for(auto& ent: *m_EntitiesBufferFront) {
                // TODO: 增量式更新 -------------------
                // if(ent.GetID() == 0) { 
                //     // DelObject(ent.GetID());
                // } else {
                //     auto& obj = GetEntity(ent.GetID());
                //     if(obj->IsError()) {
                //         spdlog::error("World::WorldUpdate: Entity not exists: {}", ent.GetID());
                //         continue; // not exists
                //     }
                //     *obj = ent; // update state
                // }
                // ---------------
                uint32_t id = ent->GetID();
                if(id == 0) continue; 
                m_entities[id] = std::move(ent); // move entity to world
            }
            i_isStateConsumed = true; // mark as consumed
        }
    }

    WorldPhysicsUpdate(); 
    WorldAnimeUpdate();

    ProvideUpdater(); 
}

void World::WorldUpdateFixed() {
    auto now = std::chrono::steady_clock::now();
    WorldUpdate();
    auto end = std::chrono::steady_clock::now(); 
    uint32_t duration = static_cast<uint32_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end - now).count()
    );
    UpdateFPS(duration);
}



void World::WorldPhysicsUpdate() {



    for(auto& [id, ent]: m_entities) {
        ent->PhysicsUpdate();
    }
    Physics::UpdatePhysicalWorld(*this);
}
void World::WorldAnimeUpdate() {
    bool ret = m_camera.Update();
    if(!ret) {
        spdlog::error("Camera Update Failed!");
        return;
    }
    
    for(auto& [id, ent]: m_entities) {
        ent->AnimeUpdate();
    }
}

void World::SetFPS(uint32_t fps) {
    assert(fps != 0);
    m_targetFPS = fps;
    m_FPS_targetDuration = static_cast<uint32_t>(1000/fps);
}

void World::UpdateFPS(uint32_t duration) {
    if(duration < m_FPS_targetDuration) {
        uint32_t remain = m_FPS_targetDuration - duration;
        std::this_thread::sleep_for(
            std::chrono::milliseconds(remain)
        );
        duration = m_FPS_targetDuration;
    }
    m_FPS_duration[m_FPS_cnt] = duration;
    m_FPS_cnt = (m_FPS_cnt+1)%5;
    uint32_t sum = 0;
    for(int i = 0; i < 5; i ++) {
        sum += m_FPS_duration[i];
    }
    if(sum == 0) sum = 1;
    m_FPS = 5000/sum;
}

uint32_t World::GetFPS() {
    return m_FPS;
}

void World::Attach(uint32_t entity_id)
{ 
    auto& obj = GetEntity(entity_id);
    assert(!obj->IsError());
    m_camera.Connect(this, entity_id);
}

void World::Attach() {
    Attach(m_localPlayer);
}

RenderStateBuffer World::GetRenderState() {
    RenderStateBuffer render_buffer;
    // render_buffer.entities.reserve(m_entities.size());
    for(auto& e: m_entities) {
        render_buffer.entities.push_back(
            *e.second // copy data
        );
    }
    render_buffer.objects = m_worldMap.objects;
    render_buffer.camera = m_camera.GetCamera();
    render_buffer.pfps = GetFPS();
    return render_buffer;
}

void World::PushInput(const util::InputState& input) {
    m_inputQueue.enqueue(input);
}

void World::PrepareState(std::unique_ptr<Entity>& ent) {
    m_EntitiesBufferBack->push_back(std::move(ent)); 
}
void World::SwapState(uint32_t seq_num) {
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        i_isStateConsumed = false;
        std::swap(m_EntitiesBufferFront, m_EntitiesBufferBack); 
        m_updateSequenceNumber = seq_num; 
    }
    m_EntitiesBufferBack->clear(); // clear back buffer
} 

void World::ProvideUpdater() { // after thread start, it is always ready to provide entity state
    for(auto& [ind, ent]: m_entities) {
        if(ent->GetID() == 0) continue; // skip error entity
        std::unique_ptr<Entity> ent_ptr(ent->Clone()); // move entity to back buffer
        m_UpdaterBufferBack->emplace_back(
            std::move(ent_ptr)
        ); // move entity to back buffer
    }
    {
        std::lock_guard<std::mutex> lock(m_updaterMutex);
        std::swap(m_UpdaterBufferFront, m_UpdaterBufferBack); // swap front and back buffer
    }
    m_UpdaterBufferBack->clear(); // clear back buffer
}

std::vector<util::EntityState> World::GetUpdater() {
    std::lock_guard<std::mutex> lock(m_updaterMutex);
    std::vector<util::EntityState> updater_states;
    updater_states.reserve(m_UpdaterBufferFront->size());
    for(auto& ent: *m_UpdaterBufferFront) {
        if(ent->GetID() == 0) continue; // skip error entity
        updater_states.push_back(ent->GetState());
    }
    return updater_states;
}

uint32_t World::NewID() {
    static uint32_t current_id = 100;
    return current_id++;
}

SyncState g_syncState;
