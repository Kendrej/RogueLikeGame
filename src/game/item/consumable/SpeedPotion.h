#pragma once
#include "game/item/Consumable.h"
#include "game/entities/LivingEntity.h"

class SpeedPotion : public Consumable
{
public:
    float speedBoost = 150.0f;
    float accelerationBoost = 500.0f;

    SpeedPotion() : Consumable(2, "Speed Potion") {}

    bool use(LivingEntity& target) override
    {
        if (!target.isAlive())
            return false;

        target.setMaxSpeed(target.getMaxSpeed() + speedBoost);
        target.setAcceleration(target.getAcceleration() + accelerationBoost);
        return true;
    }
};