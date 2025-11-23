#pragma once
class World;
class Npc;

class INpcController {
public:
    virtual ~INpcController() = default;
    virtual void update(Npc& npc, World& world, float dt) = 0;
};


