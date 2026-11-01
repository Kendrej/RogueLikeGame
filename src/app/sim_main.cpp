// RogueLikeSim: gra bez okna i bez Vulkana, sterowana krok po kroku przez ZeroMQ.
#include <exception>
#include <iostream>
#include <string>

#include <imgui.h>

#include "ai/AiEnvironment.h"
#include "ai/AiServer.h"
#include "app/GameSetup.h"
#include "engine/gfx/Assets.h"
#include "game/world/World.h"

int main(int argc, char** argv)
{
    int   port        = 5555;
    int   frameSkip   = 4;
    float dt          = 1.0f / 60.0f;
    int   idleSeconds = 60;

    for (int i = 1; i + 1 < argc; i += 2)
    {
        const std::string arg = argv[i];
        if (arg == "--port")
            port = std::stoi(argv[i + 1]);
        else if (arg == "--frame-skip")
            frameSkip = std::stoi(argv[i + 1]);
        else if (arg == "--dt")
            dt = std::stof(argv[i + 1]);
        else if (arg == "--idle-timeout")
            idleSeconds = std::stoi(argv[i + 1]);
    }

    try
    {
        // World czyta pozycje myszy z ImGui przy strzale gracza, wiec kontekst musi istniec
        ImGui::CreateContext();

        Assets assets(Assets::Ctx{});
        World  world(&assets);
        world.setScreenBounds(1920.0f, 1080.0f);
        setupGame(world);

        AiEnvironment env(world);
        AiServer      server(port, frameSkip, dt);

        // Bez klienta przez idleSeconds proces sie konczy, zeby nie zostawal po przerwanym treningu
        while (true)
        {
            AiServer::Result result = server.poll(env, idleSeconds * 1000);
            if (result != AiServer::Result::Handled)
                break;
        }

        ImGui::DestroyContext();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
