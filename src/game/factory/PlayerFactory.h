#pragma once
#include <imgui.h>

class Player;
class World;

class PlayerFactory
{
public:
    static Player* createPlayer(World& world, ImVec2 pos);
};

