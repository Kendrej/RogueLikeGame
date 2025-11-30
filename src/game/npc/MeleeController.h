#pragma once
#include "INpcController.h"
class MeleeController : public INpcController
{
public:
    void update(Npc&, World& world, float dt) override;
};
