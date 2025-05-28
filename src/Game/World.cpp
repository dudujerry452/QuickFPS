#include "World.h"
#include "inttypes.h"
#include "../Sync/Sync.h"

World::World(): m_localPlayer(0) {

    float heights[MAX_COLUMNS] = { 0 };
    Vector3 positions[MAX_COLUMNS] = { 0 };
    Color colors[MAX_COLUMNS] = { 0 };

    for (int i = 0; i < MAX_COLUMNS; i++)
    {
        heights[i] = (float)GetRandomValue(1, 12);
        positions[i] = (Vector3){ (float)GetRandomValue(-15, 15), heights[i]/2.0f, (float)GetRandomValue(-15, 15) };
        colors[i] = (Color){ (unsigned char)GetRandomValue(20, 255), (unsigned char)GetRandomValue(10, 55), 30, 255 };
    }

    m_worldMap.objects.push_back({LIGHTGRAY, BoundingBox({(Vector3){0.0f, -0.1f, 0.0f}, (Vector3){32.0f, 0.0f, 32.0f}})});
    m_worldMap.objects.push_back({BLUE, BoundingBox({(Vector3){ -16.0f, 2.5f, 0.0f }, (Vector3){1.0f, 5.0f, 32.0f}})});
    m_worldMap.objects.push_back({LIME, BoundingBox({(Vector3){ 16.0f, 2.5f, 0.0f }, (Vector3){1.0f, 5.0f, 32.0f}})});
    m_worldMap.objects.push_back({GOLD, BoundingBox({(Vector3){ 0.0f, 2.5f, 16.0f }, (Vector3){32.0f, 5.0f, 1.0f}})});

    for (int i = 0; i < MAX_COLUMNS; i++)
    {
        m_worldMap.objects.push_back({colors[i], BoundingBox({positions[i], (Vector3){2.0f, heights[i], 2.0f}})});
    }

    auto ptr = std::make_unique<Entity>(); // GetEntity
    ptr->SetError(1);
    m_entities[0] = std::move(ptr);

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
    m_entities[id] = std::forward<decltype(obj_ptr)>(obj_ptr);
    return id;
}
bool World::DelObject(uint32_t obj_id) {
    return m_entities.erase(obj_id);
}

// uint32_t World::AddPlayer(std::unique_ptr<Player>&& player_ptr, bool is_local) {
//     auto id = AddObject(std::move(player_ptr));
//     m_players.push_back(id);
// }

bool World::IsCollide(uint32_t obj_id) {

}

void World::WorldUpdate() {
    WorldPhysicsUpdate(); 
    WorldAnimeUpdate();
}



void World::WorldPhysicsUpdate() {
    for(auto& [id, ent]: m_entities) {
        ent->PhysicsUpdate();
    }
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
    return render_buffer;
}

uint32_t World::NewID() {
    static uint32_t current_id = 100;
    return current_id++;
}