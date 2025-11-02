#include "classes/entity.h"
#include "../engine/gfx/Assets.h"
#include <vector>
#include "GameSetup.h"
#include "classes/Player.h"


void setupGameEntities(std::vector<Entity*>& entities, Assets* assets, Player*& outPlayer)
{
    outPlayer = Player::spawn(assets, "assets/characters/hero.png", 64, 64, 256.0f, 256.0f);
    spawn(entities, assets, "assets/characters/angel.png", 64, 64, 400.0f, 256.0f);
    spawn(entities, assets, "assets/characters/angel.png", 64, 64, 400.0f, 400.0f);
}