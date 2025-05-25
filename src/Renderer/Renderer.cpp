#include "Renderer.h"
#include "spdlog/spdlog.h"

void Renderer::Prepare(RenderStateBuffer&& buffer) {
    m_backBuffer = std::forward<RenderStateBuffer>(buffer); // copy data
    std::swap(m_backBuffer, m_frontBuffer);
}

void Renderer::Render() {

    BeginDrawing();

        ClearBackground(RAYWHITE);

        auto& camera = m_frontBuffer.camera;
        BeginMode3D(camera);

        for(auto obj : m_frontBuffer.objects) {
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

        EndMode3D();

        // Draw info boxes

        DrawRectangle(600, 5, 195, 100, Fade(SKYBLUE, 0.5f));
        DrawRectangleLines(600, 5, 195, 100, BLUE);

        DrawText("Camera status:", 610, 15, 10, BLACK);
        DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z), 610, 60, 10, BLACK);
        Vector3 tmptar = camera.target;
        DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", tmptar.x, tmptar.y, tmptar.z), 610, 75, 10, BLACK);
        DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), 610, 90, 10, BLACK);

    EndDrawing();
}
