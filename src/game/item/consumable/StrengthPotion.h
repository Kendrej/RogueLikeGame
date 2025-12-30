#pragma once
#include "game/item/Consumable.h"
#include "game/entities/LivingEntity.h"

class StrengthPotion : public Consumable
{
public:
    int damageBoost = 10;

    StrengthPotion() : Consumable(-1, "Strength Potion") {}

    bool use(LivingEntity& target) override
    {
        if (!target.isAlive())
            return false;

        target.setMeleeDamage(target.getMeleeDamage() + damageBoost);
        target.setRangedDamage(target.getRangedDamage() + damageBoost);
        return true;
    }
};
