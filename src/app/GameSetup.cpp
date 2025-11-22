#include "GameSetup.h"
#include "World.h"
#include "Map.h"
#include <stdexcept>
#include <string>

void setupGame(World& world)
{
    Map map;
    if (!map.loadFromFile("assets/maps/map0.txt")) {
        throw std::runtime_error("Could not load map file!");
    }

    world.buildFromMap(
        map,
        "assets/design/wall.png",
        "assets/design/floor.png",
        "assets/design/door.png",
        64, 64
    );

    auto& player = world.spawnPlayer(
        "assets/characters/hero.png",
        64, 64,
        100.0f, 100.0f, 100
    );

}
