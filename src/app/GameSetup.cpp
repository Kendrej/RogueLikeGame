#include "GameSetup.h"
#include "World.h"
#include "Map.h"
#include <stdexcept>
#include <string>

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

    world.spawnPlayer(
        "assets/characters/hero.png",
        64, 64,
        100.0f, 100.0f + World::UI_TOP_BAR_HEIGHT,  // Add UI offset to initial spawn
        100, "assets/animations/Soldier-Walk-right.png", 8, "assets/animations/Soldier-Walk-left.png", 8, "assets/animations/Soldier-Idle.png", 6
    );

    auto& npc = world.spawnNpc("assets/characters/angel.png",
        64, 64,
        180.0f, 180.0f + World::UI_TOP_BAR_HEIGHT,  // Add UI offset to NPC spawn
        100);
}
