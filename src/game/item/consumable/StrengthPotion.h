#pragma once
#include "game/item/Consumable.h"
#include "game/entities/LivingEntity.h"

class StrengthPotion : public Consumable
{
public:
    int damageBoost = 100;

    StrengthPotion() : Consumable(-1, "Strength Potion") {}

    bool use(LivingEntity& target) override
    {
        if (!target.isAlive())
            return false;


        int prevMeleeDamage = target.getMeleeDamage();
        int prevRangedDamage = target.getRangedDamage();
        target.addTimedEffect(duration, [&target, prevMeleeDamage, prevRangedDamage]() {
           target.setMeleeDamage(prevMeleeDamage);
            target.setRangedDamage(prevRangedDamage);
        });
        target.setMeleeDamage(prevMeleeDamage + damageBoost);
        target.setRangedDamage(prevRangedDamage + damageBoost);
        return true;
    }
private:
    float duration = 10.0f;


};

