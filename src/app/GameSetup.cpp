#include "GameSetup.h"
#include <iostream>
#include "game/world/World.h"
#include "../game/factory/NpcFactory.h"
#include "game/factory/ItemFactory.h"


void setupGame(World& world)
{
    world.setCurrentMapLevel(0);
    world.addMapfromTmx("assets/maps/00.tmx");
    world.addMapfromTmx("assets/maps/01.tmx");
    world.addMapfromTmx("assets/maps/02.tmx");
    world.addMapfromTmx("assets/maps/03.tmx");
    world.addMapfromTmx("assets/maps/04.tmx");
    world.setCurrentMapLevel(1);
    world.addMapfromTmx("assets/maps/10.tmx");
    world.addMapfromTmx("assets/maps/11.tmx");
    world.setCurrentMapLevel(0);


    world.buildFromTmxMap();

    auto& player = world.spawnPlayer({200.0f, 200.0f + World::UI_TOP_BAR_HEIGHT});

    //world.givePlayerItem(ItemId::Key);
    //world.givePlayerConsumable(World::ConsumableType::StrengthPotion);
    //world.givePlayerConsumable(World::ConsumableType::SpeedPotion);

    auto& npc = world.spawnNpc(NpcType::Orc, {12 * 64.0f, 3 * 64.0f + World::UI_TOP_BAR_HEIGHT});
    npc.setGivingItem(ItemId::None);
    auto& npc2 = world.spawnNpc(NpcType::Skeleton_Archer, {17 * 64.0f, 3 * 64.0f + World::UI_TOP_BAR_HEIGHT});
    npc2.setGivingItem(ItemId::None);
}

void restartGame(World& world) {
    world.reset();
    setupGame(world);
}