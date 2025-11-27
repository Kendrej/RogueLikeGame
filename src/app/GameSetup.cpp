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
                                    "assets/animations/soldier/Soldier-Walk-right.png", 8, "assets/animations/soldier/Soldier-Walk-left.png", 8,
                                    "assets/animations/soldier/Soldier-Idle-right.png", 6, "assets/animations/soldier/Soldier-Idle-left.png", 6,
                                    "assets/animations/soldier/Soldier-Hurt-right.png", 4, "assets/animations/soldier/Soldier-Hurt-left.png", 4,
                                    "assets/animations/soldier/Soldier-Death-right.png", 4, "assets/animations/soldier/Soldier-Death-left.png", 4);
    player.createMeleeAttackAnimation(100, "assets/animations/soldier/Soldier-Attack-right.png", 6, "assets/animations/soldier/Soldier-Attack-left.png", 6);
	player.createRangedAttackAnimation(100, "assets/animations/soldier/Soldier-RangeAttack-right.png", 9, "assets/animations/soldier/Soldier-RangeAttack-left.png", 9);

    auto& npc1 = world.spawnNpc("assets/characters/angel.png",
        64, 64,
        780.0f, 180.0f + World::UI_TOP_BAR_HEIGHT,  // Add UI offset to NPC spawn
        10, std::make_unique<MeleeController>());
    npc1.createAnimationController(world.getAssets(),100,
                                    "assets/animations/orc/Orc-Walk-right.png", 8, "assets/animations/orc/Orc-Walk-left.png", 8,
                                    "assets/animations/orc/Orc-Idle-right.png", 6, "assets/animations/orc/Orc-Idle-left.png", 6,
                                    "assets/animations/orc/Orc-Hurt-right.png", 4, "assets/animations/orc/Orc-Hurt-left.png", 4,
                                    "assets/animations/orc/Orc-Death-right.png", 4, "assets/animations/orc/Orc-Death-left.png", 4);
    npc1.createMeleeAttackAnimation(100, "assets/animations/orc/Orc-Attack-right.png", 6, "assets/animations/orc/Orc-Attack-left.png", 6);

    auto& npc2 = world.spawnNpc("assets/characters/hero.png",
        64, 64,
        1080.0f, 180.0f + World::UI_TOP_BAR_HEIGHT,  // Add UI offset to NPC spawn
        10, std::make_unique<RangeController>());
    npc2.setRangedRange(300.0f);
    npc2.createAnimationController(world.getAssets(), 100,
        "assets/animations/skeletonArcher/SkeletonArcher-Walk-right.png", 8, "assets/animations/skeletonArcher/SkeletonArcher-Walk-left.png", 8,
        "assets/animations/skeletonArcher/SkeletonArcher-Idle-right.png", 6, "assets/animations/skeletonArcher/SkeletonArcher-Idle-left.png", 6,
        "assets/animations/skeletonArcher/SkeletonArcher-Hurt-right.png", 4, "assets/animations/skeletonArcher/SkeletonArcher-Hurt-left.png", 4,
        "assets/animations/skeletonArcher/SkeletonArcher-Death-right.png", 4, "assets/animations/skeletonArcher/SkeletonArcher-Death-left.png", 4);
	npc2.createRangedAttackAnimation(100, "assets/animations/skeletonArcher/SkeletonArcher-RangeAttack-right.png", 9, "assets/animations/skeletonArcher/SkeletonArcher-RangeAttack-left.png", 9);
}
