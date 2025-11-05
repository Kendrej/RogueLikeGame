#pragma once
#include <vector>
#include <memory>

class Entity;
class Assets;
class Player;

void setupGameEntities(std::vector<std::unique_ptr<Entity>>& entities, Assets* assets, std::unique_ptr<Player>& outPlayer);
