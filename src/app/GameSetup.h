#pragma once
#include <vector>

class Entity;
class Assets;
class Player;

void setupGameEntities(std::vector<Entity*>& entities, Assets* assets, Player*& outPlayer);
