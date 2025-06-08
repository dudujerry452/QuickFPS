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

    // auto ptr = std::make_unique<Entity>(); // GetEntity
    // ptr->SetError(1);
    // m_entities[0] = std::move(ptr);

    SetFPS(60); // default fps = 60
    memset(m_FPS_duration, 0, sizeof(m_FPS_duration));
    m_FPS_cnt = 0;
    m_FPS = 0;

    i_isStateConsumed = true;
    i_isUpdaterConsumed = true;
    ProvideUpdater();
}

World::~World() {
    
}

Entity* World::GetEntity(uint32_t obj_id) { 
    if(m_entities.find(obj_id) != m_entities.end())
        return m_entities[obj_id].get(); 
    else return nullptr; // return error entity
}

uint32_t World::AddEntity(std::unique_ptr<Entity>&& obj_ptr) {
    auto id = NewID();
    obj_ptr->SetID(id);
    m_entities[id] = std::forward<decltype(obj_ptr)>(obj_ptr);
    return id;
}
bool World::AddEntity(std::unique_ptr<Entity>&& obj_ptr, uint32_t obj_id) {
    if(GetEntity(obj_id)) return false;
    obj_ptr->SetID(obj_id);
    m_entities[obj_id] = std::forward<decltype(obj_ptr)>(obj_ptr);
    return true;
}
bool World::DelObject(uint32_t obj_id) {
    return m_entities.erase(obj_id);
}

void World::InitLocalPlayer(uint32_t player_id) {
    auto player = GetEntity(player_id);
    m_localPlayer = player_id;
    if(player) {
        if(player->GetType() != Entity::EntityType::LocalPlayer) {
            
            auto lplayer = std::make_unique<LocalPlayer>(*(player));
            DelObject(player_id);
            AddEntity(std::move(lplayer));
        } else {
            return;
        }
    } else {
        std::unique_ptr<LocalPlayer> lplayer = std::make_unique<LocalPlayer>();
        AddEntity(std::move(lplayer), player_id);
        auto newplayer = GetEntity(player_id);
        newplayer->SetPos({0,1.5,4});
        newplayer->SetForward({1,0,0});
    }
}

void World::WorldUpdate() { 


    bool apply0 = ApplyInputs();
    bool apply1 = ApplyUpdater();

    // normal logics

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
    if(m_localPlayer){
        bool ret = m_camera.Update();
        if(!ret) {
            spdlog::error("Camera Update Failed!");
            return;
        }
    }
    
    for(auto& [id, ent]: m_entities) {
        ent->AnimeUpdate();
    }
}

bool World::ApplyInputs() {
    std::vector<util::InputState> inputs;
    int size = m_inputQueue.try_dequeue_bulk(
        std::back_inserter(inputs), 1000
    );
    if(size > 0) {
        for(const auto& input: inputs) {
            auto ent = GetEntity(input.player_id);
            if(!ent) continue; // not exists
            auto player = dynamic_cast<Player*>(ent);
            if(!player) {
                spdlog::error("World::WorldPhysicsUpdate: Entity is not a Player: {}", input.player_id);
                continue; // not player
            }
            player->PushNewInput(input);
        }
        return true;
    }
    return false;
}

bool World::ApplyUpdater() {
    if(m_localPlayer) {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        if(i_isStateConsumed) return false;

        m_entities.clear(); 
        for(auto& state: *m_EntitiesBufferFront) {
            auto ptr = GetEntityFromState(state);
            auto id = ptr->GetID(); 
            AddEntity(std::move(ptr), id);
        }
        i_isStateConsumed = true;
    }
    return true;
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
    assert(GetEntity(entity_id));
    m_camera.Connect(this, entity_id);
}

void World::Attach() {
    Attach(m_localPlayer);
}

RenderStateBuffer World::GetRenderState() {
    RenderStateBuffer render_buffer;
    // render_buffer.entities.reserve(m_entities.size());
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        for(auto& e: m_entities) {
            render_buffer.entities.push_back(
                *e.second // copy data
            );
        }
    }
    render_buffer.objects = m_worldMap.objects;
    render_buffer.camera = m_camera.GetCamera();
    render_buffer.pfps = GetFPS();
    return render_buffer;
}

void World::PushInput(const util::InputState& input) {
    if(input.sequence_number == 0) return; 
    m_inputQueue.enqueue(input);
}

void World::PrepareState(std::vector<util::EntityState> states) {
    for(auto & state: states) {
        m_EntitiesBufferBack->push_back(std::move(state));
    }
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        i_isStateConsumed = false;
        std::swap(m_EntitiesBufferFront, m_EntitiesBufferBack); 
    }
    m_EntitiesBufferBack->clear(); // clear back buffer
    
}

void World::ProvideUpdater() { // after thread start, it is always ready to provide entity state
    for(auto& [ind, ent]: m_entities) {
        auto state = ent->GetState();
        m_UpdaterBufferBack->push_back(std::move(state));
    }
    {
        std::lock_guard<std::mutex> lock(m_updaterMutex);
        std::swap(m_UpdaterBufferFront, m_UpdaterBufferBack); // swap front and back buffer
        i_isUpdaterConsumed = false;
    }
    m_UpdaterBufferBack->clear(); // clear back buffer
}

std::vector<util::EntityState> World::GetUpdater() {
    std::lock_guard<std::mutex> lock(m_updaterMutex);
    return std::move(*m_UpdaterBufferFront);
}

uint32_t World::NewID() {
    static uint32_t current_id = 100;
    return current_id++;
}

SyncState g_syncState;
