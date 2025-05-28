/*******************************************************************************************
*
*   raylib [core] example - 3d camera first person
*
*   Example complexity rating: [★★☆☆] 2/4
*
*   Example originally created with raylib 1.3, last time updated with raylib 1.3
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2015-2025 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "rcamera.h"
#include "Game/Entity.h"
#include "Game/Player.h"
#include "Game/World.h"

#include "Renderer/Renderer.h"

#include "spdlog/spdlog.h"
#include <iostream>

#define MAX_COLUMNS 20

std::atomic<bool> g_isRunning{false}; 

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{

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

    LocalPlayer player;
    player.SetPos({0,2,4});
    player.SetForward({0,1,0});

    World world;
    auto plid = world.AddEntity(std::move(std::make_unique<LocalPlayer>(player)));
    world.SetLocalPlayer(plid);
    world.Attach();

    Renderer renderer;
    g_isRunning = true;

    std::mutex mtx;
    std::condition_variable cv;

    std::thread physical_thread([&] {
        spdlog::info("Physical Thread Start");
        while(g_isRunning.load(std::memory_order_relaxed)) {
            world.WorldUpdate();
        }
    });

    // Main game loop
    // while (!WindowShouldClose())        // Detect window close button or ESC key
    // {
    //     IM.CheckInput();
    //     world.WorldPhysicsUpdate();
    //     world.WorldAnimeUpdate();
    //     renderer.Prepare(world.GetRenderState());
    //     renderer.Render();
    // }
    //--------------------------------------------------------------------------------------

    while(!WindowShouldClose()) {
        IM.CheckInput();
        renderer.Prepare(world.GetRenderState());
        renderer.Render();
    }
    g_isRunning.store(false, std::memory_order_relaxed);
    physical_thread.join();

    CloseWindow();        

    return 0;
}