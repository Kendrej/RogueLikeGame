#include "GameSetup.h"
#include "World.h"
#include "Map.h"
#include <stdexcept>
#include <string>

void setupGame(World& world)
{
    world.addMap("assets/maps/map0.txt");
    world.addMap("assets/maps/map1.txt");

    world.setCurrentMapIndex(1);

    world.buildFromMap(
        "assets/design/wall.png",
        "assets/design/floor.png",
        "assets/design/door.png",
        64, 64
    );

    world.spawnPlayer(
        "assets/characters/hero.png",
        64, 64,
        100.0f, 100.0f, 100
    );
}
