#pragma once
#include "../LivingEntity.h"
#include <memory>
#include "INpcController.h"
class World;
class Npc: public LivingEntity{
public:
    Npc(int NpcId, uint32_t width, uint32_t height, float pos_x, float pos_y, int maxHp):
    LivingEntity(NpcId, width, height, pos_x, pos_y, maxHp){}

    enum class State { Idle, Chase , Attack};

    void setController(std::unique_ptr<INpcController> ctrl);
    void update(float dt) override;
    void setWorld(World* w) { world_ = w;}
private:
    std::unique_ptr<INpcController> controller;
    State state = State::Idle;
    World* world_ = nullptr;
};


