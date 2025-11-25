#include "GameSetup.h"
#include "World.h"
#include "Map.h"
#include <stdexcept>
#include <string>
#include <memory>
#include "npc/MeleeController.h"
#include "npc/RangeController.h"

void setupGame(World& world)
{
    world.addMap("assets/maps/map0.txt");
    world.addMap("assets/maps/map1.txt");
    world.addMap("assets/maps/map2.txt");
    world.addMap("assets/maps/map3.txt");

    world.buildFromMap(
        "assets/design/wall.png",
        "assets/design/floor.png",
        "assets/design/door.png",
        64, 64
    );

    auto& player = world.spawnPlayer(
        "assets/characters/hero.png",
        64, 64,
        100.0f, 100.0f + World::UI_TOP_BAR_HEIGHT,  // Add UI offset to initial spawn
        100
    );
	player.createAnimationController(world.getAssets(),100, "assets/animations/Soldier-Walk-right.png", 8, "assets/animations/Soldier-Walk-left.png", 8,
                                    "assets/animations/Soldier-Idle-right.png", 6, "assets/animations/Soldier-Idle-left.png", 6);

    auto& npc1 = world.spawnNpc("assets/characters/angel.png",
        64, 64,
        180.0f, 180.0f + World::UI_TOP_BAR_HEIGHT,  // Add UI offset to NPC spawn
        100, std::make_unique<MeleeController>(), 70.0f);
    npc1.createAnimationController(world.getAssets(),100, "assets/animations/Orc-Walk-right.png", 8, "assets/animations/Orc-Walk-left.png", 8,
                                    "assets/animations/Orc-Idle-right.png", 6, "assets/animations/Orc-Idle-left.png", 6);

    auto& npc2 = world.spawnNpc("assets/characters/hero.png",
        64, 64,
        180.0f, 180.0f + World::UI_TOP_BAR_HEIGHT,  // Add UI offset to NPC spawn
        100, std::make_unique<RangeController>(), 300.0f);
	npc2.createAnimationController(world.getAssets(), 96, "assets/animations/Slime-Walk-right.png", 8, "assets/animations/Slime-Walk-left.png", 8,
                                    "assets/animations/Slime-Idle-right.png", 6, "assets/animations/Slime-Idle-left.png", 6);
}
