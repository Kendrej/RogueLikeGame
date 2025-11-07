#include "classes/entity.h"
#include "../engine/gfx/Assets.h"
#include <vector>
#include "GameSetup.h"
#include "classes/Player.h"
#include <memory>
#include <Map.h>

#include "World.h"

void setupGameEntities(World& world, Map& map, Assets& assets)
{
    world.spawnPlayer("assets/characters/hero.png", 128, 128, 256.0f, 256.0f);
}
