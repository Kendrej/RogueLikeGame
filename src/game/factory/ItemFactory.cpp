
#include "ItemFactory.h"

#include "game/item/consumable/HealthPotion.h"
#include "game/item/consumable/SpeedPotion.h"
#include "game/item/consumable/StrengthPotion.h"

std::unique_ptr<Item> ItemFactory::createItem(ItemId id) {
    switch (id) {
        case ItemId::HealthPotion:
            return std::make_unique<HealthPotion>();
        case ItemId::SpeedPotion:
            return std::make_unique<SpeedPotion>();
        case ItemId::StrengthPotion:
            return std::make_unique<StrengthPotion>();
    }
}
