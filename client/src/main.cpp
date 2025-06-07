#include "raylib.h"
#include "rcamera.h"
#include "Game/Entity.h"
#include "Game/Player.h"
#include "Game/World.h"

#include "Renderer/Renderer.h"
#include "Input/Input.h"

#include "Network/networkapi.h"
#include "Network/Serialize.h"

#include "spdlog/spdlog.h"
#include <iostream>

#define MAX_COLUMNS 20

LocalPlayer player;
World world;
Renderer renderer;
NetworkHandle network = NcreateNetworkHandle(); // Network handle for client-side network operations
std::atomic<bool> g_isRunning{false}; 

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{

    // start(0, 0, 0);
    // serialization::test_entity_state_batch_serialization();

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

    player.SetPos({0,1.5,4});
    player.SetForward({1,0,0});

    Entity obs;
    obs.SetPos({-7, 12, 5});
    obs.SetForward({1, -1, 0});

    auto plid = world.AddEntity(std::move(std::make_unique<LocalPlayer>(player)));
    auto ob = world.AddEntity(std::move(std::make_unique<Entity>(obs)));
    world.SetLocalPlayer(plid);
    world.Attach();

    g_isRunning = true;

    Nstart(network, (char*)"127.0.0.1", 1077);
    Nsend(network, (char*)"Hello from client!");

    uint32_t localplaer = world.GetLocalPlayer();

    std::thread physical_thread([&] {
        spdlog::info("Physical Thread Start");
        while(g_isRunning.load(std::memory_order_relaxed)) {
            world.WorldUpdateFixed();
        }
    });

    while(!WindowShouldClose()) {
        renderer.Prepare(world.GetRenderState());
        renderer.Render();

        IM.CheckInput();
        auto input = IM.Pop();
        input.player_id = localplaer;
        if(input.sequence_number) {
            world.PushInput(input);
        } 

        if(IsKeyPressed(KEY_Q)) {
            auto data = world.GetUpdater();
            spdlog::debug("Updater size: {}", data.size());
            for(const auto& ent : data) {
                spdlog::debug("Entity ID: {}, Position: ({}, {}, {}), Forward: ({}, {}, {}), Velocity: ({}, {}, {}), Seq: {}",
                    ent.id,
                    ent.position.x, ent.position.y, ent.position.z,
                    ent.forward.x, ent.forward.y, ent.forward.z,
                    ent.velocity.x, ent.velocity.y, ent.velocity.z,
                    ent.seq_num
                );
            }
        }
    }

    g_isRunning.store(false, std::memory_order_relaxed);

    Nstop(network); 
    if(physical_thread.joinable())
        physical_thread.join();
    

    CloseWindow();        

    return 0;
}