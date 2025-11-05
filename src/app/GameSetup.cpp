#include "classes/entity.h"
#include "../engine/gfx/Assets.h"
#include <vector>
#include "GameSetup.h"
#include "classes/Player.h"
#include <memory>

void setupGameEntities(std::vector<std::unique_ptr<Entity>>& entities, Assets* assets, std::unique_ptr<Player>& outPlayer)
{
    Player::spawnPlayer(outPlayer, assets, "assets/characters/hero.png",128,128,256.0f,256.0f);
    Entity::spawn(entities, assets, "assets/characters/angel.png",64,64,1856,906);
    Entity::spawn(entities, assets, "assets/characters/angel.png",64,64,400.0f,400.0f);
}