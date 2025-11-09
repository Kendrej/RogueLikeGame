#include "GameSetup.h"
#include "World.h"
#include "Map.h"
#include <stdexcept>
#include <string>

void setupGame(World& world)
{
    // 1) Wczytaj plik mapy
    Map map;
    if (!map.loadFromFile("assets/maps/map1.txt")) {
        throw std::runtime_error("Could not load map file!");
    }

    // 2) Zbuduj świat z mapy (tile-y)
    world.buildFromMap(
        map,
        "assets/design/wall.png",
        "assets/design/floor.png",
        64, 64 // wymiary kafelka
    );

    // 3) Spawn Playera
    auto& player = world.spawnPlayer(
        "assets/characters/hero.png",
        64, 64,
        100.0f, 100.0f // start pos
    );

    // 4) Można dorzucić przykładowego NPC
   // world.spawnTile("assets/characters/npc.png", 64, 64, 300.0f, 300.0f);
}
