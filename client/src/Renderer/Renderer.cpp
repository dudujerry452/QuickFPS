#include "Renderer.h"
#include "spdlog/spdlog.h"

void RenderAABB(util::BoundingBox box, Color color) {
    Vector3 min = box.min;
    Vector3 max = box.max;
    DrawCubeV(Vector3Scale(Vector3Add(min, max), 0.5f), Vector3Subtract(max, min), color);
    DrawCubeWiresV(Vector3Scale(Vector3Add(min, max), 0.5f), Vector3Subtract(max, min), MAROON);
}



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
            BeginMode3D(camera);

            for(auto& obj : m_frontBuffer->objects) {
                RenderAABB(obj.colisionBoxes, 
                    (Color){obj.color.r, obj.color.g, obj.color.b, obj.color.a});
            }
            for(auto& ent : m_frontBuffer->entities) {
                if(ent.IsError()) continue;
                DrawLine3D(ent.GetPos(), ent.GetPos() + ent.GetForward(), RED);
                if(ent.GetType() == Entity::EntityType::LocalPlayer) {
                    RenderAABB(ent.GetGlobalBoundingBox(), BLUE);
                }
            }


            EndMode3D();

            // Draw info boxes
            for(auto& ent : m_frontBuffer->entities) {
                if(ent.IsError()) continue;
                // DrawLine3D(ent.GetPos(), ent.GetPos() + ent.GetForward(), RED);
                if(ent.GetType() == Entity::EntityType::LocalPlayer) {
                    DrawText(
                        TextFormat(
                            "Velocity: %06.3f", Vector3Length(ent.GetVelocity())
                        ), 5, 25, 10, BLACK);
                }
            }

            DrawRectangle(600, 5, 195, 100, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(600, 5, 195, 100, BLUE);

            DrawText("Camera status:", 610, 15, 10, BLACK);
            DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z), 610, 60, 10, BLACK);
            Vector3 tmptar = camera.target;
            DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", tmptar.x, tmptar.y, tmptar.z), 610, 75, 10, BLACK);
            DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), 610, 90, 10, BLACK);

            DrawText(TextFormat("FPS: %3d", GetFPS()), 5, 5, 10, BLACK);
            DrawText(TextFormat("PFPS: %3d", m_frontBuffer->pfps), 5, 15, 10, BLACK);

        EndDrawing();
    }
}
