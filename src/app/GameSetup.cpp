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
	player.createAnimationController(world.getAssets(),100, 
                                    "assets/animations/Soldier-Walk-right.png", 8, "assets/animations/Soldier-Walk-left.png", 8,
                                    "assets/animations/Soldier-Idle-right.png", 6, "assets/animations/Soldier-Idle-left.png", 6,
                                    "assets/animations/Soldier-Hurt-right.png", 4, "assets/animations/Soldier-Hurt-left.png", 4,
                                    "assets/animations/Soldier-Death-right.png", 4, "assets/animations/Soldier-Death-left.png", 4);

    auto& npc1 = world.spawnNpc("assets/characters/angel.png",
        64, 64,
        780.0f, 180.0f + World::UI_TOP_BAR_HEIGHT,  // Add UI offset to NPC spawn
        10, std::make_unique<MeleeController>());
    npc1.createAnimationController(world.getAssets(),100,
                                    "assets/animations/Orc-Walk-right.png", 8, "assets/animations/Orc-Walk-left.png", 8,
                                    "assets/animations/Orc-Idle-right.png", 6, "assets/animations/Orc-Idle-left.png", 6,
                                    "assets/animations/Orc-Hurt-right.png", 4, "assets/animations/Orc-Hurt-left.png", 4,
                                    "assets/animations/Orc-Death-right.png", 4, "assets/animations/Orc-Death-left.png", 4);

    auto& npc2 = world.spawnNpc("assets/characters/hero.png",
        64, 64,
        1080.0f, 180.0f + World::UI_TOP_BAR_HEIGHT,  // Add UI offset to NPC spawn
        10, std::make_unique<RangeController>());
    npc2.setRangedRange(300.0f);
    npc2.createAnimationController(world.getAssets(), 100,
        "assets/animations/SkeletonArcher-Walk-right.png", 8, "assets/animations/SkeletonArcher-Walk-left.png", 8,
        "assets/animations/SkeletonArcher-Idle-right.png", 6, "assets/animations/SkeletonArcher-Idle-left.png", 6,
        "assets/animations/SkeletonArcher-Hurt-right.png", 4, "assets/animations/SkeletonArcher-Hurt-left.png", 4,
        "assets/animations/SkeletonArcher-Death-right.png", 4, "assets/animations/SkeletonArcher-Death-left.png", 4);

}
