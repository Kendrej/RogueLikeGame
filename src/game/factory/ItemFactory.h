#pragma once
#include "game/item/Item.h"

class Assets;

enum class ItemId
{
    HealthPotion,
    SpeedPotion,
    StrengthPotion,
    Key,
    None
};

class ItemFactory {
public:
   static std::unique_ptr<Item> createItem(ItemId id, Assets* assets);
};
