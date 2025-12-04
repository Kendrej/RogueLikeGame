#include "GameSetup.h"

#include "game/world/Map.h"
#include "game/world/World.h"
#include "game/npc/NpcFactory.h"

#include <memory>
#include <stdexcept>
#include <string>

void setupGame(World& world)
{
    world.addMap("assets/maps/map0.txt");
    world.addMap("assets/maps/map1.txt");
    world.addMap("assets/maps/map2.txt");
    world.addMap("assets/maps/map3.txt");

    world.buildFromMap("assets/design/wall.png", "assets/design/floor.png", "assets/design/door.png", 64, 64);

    auto& player = world.spawnPlayer("assets/characters/hero.png", 64, 64, 100.0f,
                                     100.0f + World::UI_TOP_BAR_HEIGHT, // Add UI offset to initial spawn
                                     100);
    player.createAnimationController(
        world.getAssets(), 100, "assets/animations/soldier/Soldier-Walk-right.png", 8,
        "assets/animations/soldier/Soldier-Walk-left.png", 8, "assets/animations/soldier/Soldier-Idle-right.png", 6,
        "assets/animations/soldier/Soldier-Idle-left.png", 6, "assets/animations/soldier/Soldier-Hurt-right.png", 4,
        "assets/animations/soldier/Soldier-Hurt-left.png", 4, "assets/animations/soldier/Soldier-Death-right.png", 4,
        "assets/animations/soldier/Soldier-Death-left.png", 4);
    player.createMeleeAttackAnimation(100, 4, "assets/animations/soldier/Soldier-Attack-right.png", 6,
                                      "assets/animations/soldier/Soldier-Attack-left.png", 6);
    player.createRangedAttackAnimation(100, 8, "assets/animations/soldier/Soldier-RangeAttack-right.png", 9,
                                       "assets/animations/soldier/Soldier-RangeAttack-left.png", 9);

    world.spawnNpc(NpcType::Orc, {12 * 64.0f, 3 * 64.0f + World::UI_TOP_BAR_HEIGHT});
    world.spawnNpc(NpcType::Skeleton_Archer, {17 * 64.0f, 3 * 64.0f + World::UI_TOP_BAR_HEIGHT});
}
