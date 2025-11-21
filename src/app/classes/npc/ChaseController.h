#pragma once
#include "INpcController.h"
class ChaseController: public INpcController {
public:
    void update(Npc&, World& world, float dt) override;
};


