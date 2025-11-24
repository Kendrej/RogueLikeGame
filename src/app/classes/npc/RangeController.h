#pragma once
#include "INpcController.h"


class RangeController: public INpcController{
public:
    void update(Npc &npc, World& world, float dt) override;
};


