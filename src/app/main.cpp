#include "engine/gfx/VulkanImGuiApp.h"

#include <string>

int main(int argc, char** argv)
{
    VulkanImGuiApp app;
    bool aiMode = false;
    int  aiPort = 5555;
    int  aiFrameSkip = 4;

    for (int i = 1; i < argc; ++i)
    {
        if (!argv[i])
            continue;

        const std::string arg = argv[i];

        if (arg == "--smoke")
        {
            
            return app.runSmokeTest();
        }

        if (arg == "--ai-mode")
        {
            aiMode = true;
        }
        else if (arg == "--port" && i + 1 < argc)
        {
            aiPort = std::stoi(argv[++i]);
        }
        else if (arg == "--frame-skip" && i + 1 < argc)
        {
            aiFrameSkip = std::stoi(argv[++i]);
        }
    }
    if (aiMode)
        app.enableAiMode(aiPort, aiFrameSkip);
    return app.run();
}
