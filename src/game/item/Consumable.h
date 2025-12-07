#pragma once
#include "Item.h"

class LivingEntity;

class Consumable : public Item
{
public:
    Consumable(int id, const std::string& n) : Item(ItemType::Consumable, id, n) {}
    virtual ~Consumable() = default;


    virtual bool use(LivingEntity& target) = 0;
};