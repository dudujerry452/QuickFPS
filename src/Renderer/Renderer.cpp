#include "Renderer.h"
#include "spdlog/spdlog.h"



void Renderer::Prepare(RenderStateBuffer&& buffer) {
    *m_backBuffer = std::forward<RenderStateBuffer>(buffer); // copy data
    {
        std::lock_guard<std::mutex> lock(m_swapMutex);
        std::swap(m_backBuffer, m_frontBuffer);
        i_consumed = 0;
    }
}

void Renderer::Render() {

    {
        std::lock_guard<std::mutex> lock(m_swapMutex);
        if(i_consumed) return; // 如果数据已经被消费了则不动
        BeginDrawing();

            ClearBackground(RAYWHITE);

            auto& camera = m_frontBuffer->camera;
            // spdlog::debug("camera.x = {}, y = {}, z = {}", camera.target.x, camera.target.y, camera.target.z);
            BeginMode3D(camera);

            for(auto& obj : m_frontBuffer->objects) {
                DrawCube(obj.colisionBoxes.min, 
                obj.colisionBoxes.max.x, 
                obj.colisionBoxes.max.y,
                obj.colisionBoxes.max.z,
                obj.color
                );
                DrawCubeWires(obj.colisionBoxes.min, 
                obj.colisionBoxes.max.x, 
                obj.colisionBoxes.max.y,
                obj.colisionBoxes.max.z,
                MAROON
                );

            }

            // for(auto ent : m_frontBuffer->entities) {
            //     DrawLine3D(ent.GetPos(), ent.GetPos() + ent.GetForward(), RED);
            // }

            EndMode3D();

            // Draw info boxes

            DrawRectangle(600, 5, 195, 100, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(600, 5, 195, 100, BLUE);

            DrawText("Camera status:", 610, 15, 10, BLACK);
            DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z), 610, 60, 10, BLACK);
            Vector3 tmptar = camera.target;
            DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", tmptar.x, tmptar.y, tmptar.z), 610, 75, 10, BLACK);
            DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), 610, 90, 10, BLACK);

            DrawText(TextFormat("FPS: %3d", GetFPS()), 5, 5, 10, BLACK);

        EndDrawing();
    }
}
