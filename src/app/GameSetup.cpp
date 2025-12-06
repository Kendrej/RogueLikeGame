#include "GameSetup.h"

#include "game/world/World.h"
#include "../game/factory/NpcFactory.h"

void setupGame(World& world)
{
    world.addMapfromTmx("assets/maps/map.tmx");

    world.addMap("assets/maps/map0.txt");
    world.addMap("assets/maps/map1.txt");
    world.addMap("assets/maps/map2.txt");
    world.addMap("assets/maps/map3.txt");

    world.buildFromTmxMap();
    //world.buildFromMap("assets/design/wall.png", "assets/design/floor.png", "assets/design/door.png", 64, 64);

    world.spawnPlayer({100.0f, 100.0f + World::UI_TOP_BAR_HEIGHT});

    world.spawnNpc(NpcType::Orc, {12 * 64.0f, 3 * 64.0f + World::UI_TOP_BAR_HEIGHT});
    world.spawnNpc(NpcType::Skeleton_Archer, {17 * 64.0f, 3 * 64.0f + World::UI_TOP_BAR_HEIGHT});
}
