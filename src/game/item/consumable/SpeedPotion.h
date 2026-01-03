#pragma once

#include "game/item/Consumable.h"
#include "game/entities/LivingEntity.h"

class SpeedPotion : public Consumable
{
public:

    SpeedPotion() : Consumable(-1, "Speed Potion") {}

    bool use(LivingEntity& target) override
    {
        if (!target.isAlive()) return false;

        float prevSpeed = target.getMaxSpeed();
        float prevAcceleration = target.getAcceleration();

        target.setMaxSpeed(prevSpeed + speedBoost);
        target.setAcceleration(prevAcceleration + accelerationBoost);

        target.addTimedEffect(duration, [&target, prevSpeed, prevAcceleration]() {
            if (target.isAlive()) {
                target.setMaxSpeed(prevSpeed);
                target.setAcceleration(prevAcceleration);
            }
        });

        return true;
    }
private:
    float speedBoost = 150.0f;
    float accelerationBoost = 500.0f;
    float duration = 10.0f;

};