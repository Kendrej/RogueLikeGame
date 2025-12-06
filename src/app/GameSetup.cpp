#include "GameSetup.h"

#include "game/world/World.h"
#include "../game/factory/NpcFactory.h"

void setupGame(World& world)
{
    world.addMapfromTmx("assets/maps/00.tmx");
    world.addMapfromTmx("assets/maps/01.tmx");
    world.addMapfromTmx("assets/maps/02.tmx");
    world.addMapfromTmx("assets/maps/03.tmx");

    world.buildFromTmxMap();

    world.spawnPlayer({100.0f, 100.0f + World::UI_TOP_BAR_HEIGHT});

    world.spawnNpc(NpcType::Orc, {12 * 64.0f, 3 * 64.0f + World::UI_TOP_BAR_HEIGHT});
    world.spawnNpc(NpcType::Skeleton_Archer, {17 * 64.0f, 3 * 64.0f + World::UI_TOP_BAR_HEIGHT});
}
