#pragma once
#include <vector>

class Entity;
class Assets;
class Player;

void setupGameEntities(std::vector<Entity*>& entities, Assets* assets, Player*& outPlayer);

Entity* spawn(std::vector<Entity*>& entities, Assets* assets, const char* path, uint32_t width, uint32_t height, float posX, float posY);
