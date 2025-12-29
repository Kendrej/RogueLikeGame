#pragma once
#include "game/item/Item.h"

class StrengthPotion;
class SpeedPotion;
class HealthPotion;
class Assets;

enum class ItemId
{
    HealthPotion,
    SpeedPotion,
    StrengthPotion,
    None
};

class ItemFactory {
public:
   static std::unique_ptr<Item> createItem(ItemId id, Assets* assets);
};
