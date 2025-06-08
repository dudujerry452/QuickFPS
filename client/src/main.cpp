#include "raylib.h"
#include "rcamera.h"
#include "Game/Entity.h"
#include "Game/Player.h"
#include "Game/World.h"

#include "Renderer/Renderer.h"
#include "Input/Input.h"

#include "Network/networkapi.h"
#include "Serialize/Serialize.h"

#include "spdlog/spdlog.h"
#include <iostream>

#define MAX_COLUMNS 20

World* g_world;
Renderer renderer;
NetworkHandle network = NcreateNetworkHandle(); // Network handle for client-side network operations
std::atomic<bool> g_isRunning{false}; 

void LocalInit() {
    g_world = new World();

    Entity obs;
    obs.SetPos({-7, 12, 5});
    obs.SetForward({1, -1, 0});

    auto ob = g_world->AddEntity(std::move(std::make_unique<Entity>(obs)));
    g_world->InitLocalPlayer(500);
    g_world->Attach();

    auto player = std::make_unique<Player>();
    g_world->AddEntity(std::move(player));
    auto player2 = std::make_unique<Player>();
    g_world->AddEntity(std::move(player2));

    g_world->WorldUpdate();
    auto updater = g_world->GetUpdater();
    spdlog::debug("front size = {}", updater.size()); 
    auto binary = serialization::serialize(updater);
    auto bac = serialization::deserialize(*binary);
    auto batch =  std::get<std::vector<util::EntityState>>(*bac);

    spdlog::debug("back size = {}", batch.size());
    
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{

    // start(0, 0, 0);
    serialization::test_input_state_serialization();

    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
    spdlog::debug("spdlog has been on. ");   
    
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "QuickFPS");
    DisableCursor();                    
    SetTargetFPS(60);                   
    //--------------------------------------------------------------------------------------

    Nstart(network, (char*)"127.0.0.1", 1077);
    uint32_t ret = PtryHandshake(network, g_world);

    // -------Local Operation -----------------
    if(ret != 0) {
        spdlog::info("Failed to handshake with server, use local init");
        LocalInit(); 
    } else {
        spdlog::info("Handshake with server success");
        Nset_handler(network, HandleNetworkMessage);
        // LocalInit(); // temp
    }

    auto& world = *g_world;
    g_isRunning = true;
    uint32_t localplaer = world.GetLocalPlayer();
    std::thread physical_thread([&] {
        spdlog::info("Physical Thread Start");
        while(g_isRunning.load(std::memory_order_relaxed)) {
            world.WorldUpdateFixed();
        }
    });

    std::vector<util::EntityState> updater; // debug
    int hasupdater = 0;

    while(!WindowShouldClose()) {
        renderer.Prepare(world.GetRenderState());
        renderer.Render();

        IM.CheckInput();
        auto input = IM.Pop();
        input.player_id = localplaer;
        if(input.sequence_number) {
            if(localplaer)
                world.PushInput(input);
            auto bs = serialization::serialize(input);
            if(!bs.has_value()) {
                spdlog::error("Failed to serialize input");
            }
            NsendBytes(network, std::move(bs.value()));
        } 

        if(IsKeyPressed(KEY_Q)) {
            updater = world.GetUpdater();
            spdlog::debug("Updater size: {}", updater.size());
            for(const auto& ent : updater) {
                spdlog::debug("Entity ID: {}, Position: ({}, {}, {}), Forward: ({}, {}, {}), Velocity: ({}, {}, {}), Seq: {}",
                    ent.id,
                    ent.position.x, ent.position.y, ent.position.z,
                    ent.forward.x, ent.forward.y, ent.forward.z,
                    ent.velocity.x, ent.velocity.y, ent.velocity.z,
                    ent.seq_num
                );
            }
            hasupdater = 1;
        }
        if(IsKeyPressed(KEY_F)) {
            if(hasupdater) {
                world.PrepareState(updater); 
            }

        }
        if(IsKeyPressed(KEY_G)) {
            EnableCursor();
        }
        if(IsKeyPressed(KEY_H)) {
            DisableCursor();
        }
    }

    g_isRunning.store(false, std::memory_order_relaxed);

    if(physical_thread.joinable())
        physical_thread.join();
    
    CloseWindow();        

    // --------------------------
    
    Nstop(network); 
    if(g_world) {
        delete g_world;
        g_world = nullptr;
    }

    return 0;
}