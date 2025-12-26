#pragma once
#include "game/item/Item.h"

class StrengthPotion;
class SpeedPotion;
class HealthPotion;

enum class ItemId
{
    HealthPotion,
    SpeedPotion,
    StrengthPotion

};

class ItemFactory {
public:
    std::unique_ptr<Item> createItem(ItemId id);
};


