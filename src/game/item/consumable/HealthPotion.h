#pragma once
#include "game/item/Consumable.h"
#include "game/entities/LivingEntity.h"

class HealthPotion : public Consumable
{
public:
    int healAmount = 20;

    HealthPotion() : Consumable(-1, "Health Potion") {}

    bool use(LivingEntity& target) override
    {
        if (!target.isAlive() || target.getHp() >= target.getMaxHp())
            return false;

        target.heal(healAmount);
        return true;
    }
};