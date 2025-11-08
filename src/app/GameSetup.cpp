#include "World.h"

void setupGame(World& world)
{
    world.spawnMap("assets/maps/map1.txt", "assets/design/wall.png", "assets/design/floor.png");
    world.spawnMap("assets/maps/map2.txt", "assets/design/wall.png", "assets/design/floor.png");
	world.setCurrentMapIndex(1);
    world.spawnPlayer("assets/characters/hero.png", 128, 128, 256.0f, 256.0f);
}
