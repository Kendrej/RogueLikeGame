#include "ItemFactory.h"

#include "game/item/consumable/HealthPotion.h"
#include "game/item/consumable/SpeedPotion.h"
#include "game/item/consumable/StrengthPotion.h"
#include "engine/gfx/Assets.h"

std::unique_ptr<Item> ItemFactory::createItem(ItemId id, Assets* assets) {
    if (!assets) return nullptr;

    std::unique_ptr<Item> item;
    switch (id) {
        case ItemId::HealthPotion:
            item = std::make_unique<HealthPotion>();
            break;
        case ItemId::SpeedPotion:
            item = std::make_unique<SpeedPotion>();
            break;
        case ItemId::StrengthPotion:
            item = std::make_unique<StrengthPotion>();
            break;
        case ItemId::None:
        default:
            return nullptr;
    }

    int iconId = -1;
    switch (id) {
        case ItemId::HealthPotion: iconId = assets->getOrLoadIcon("assets/items/consumable/HealthPotion.png"); break;
        case ItemId::SpeedPotion: iconId = assets->getOrLoadIcon("assets/items/consumable/SpeedPotion.png"); break;
        case ItemId::StrengthPotion: iconId = assets->getOrLoadIcon("assets/items/consumable/StrengthPotion.png"); break;
        default: break;
    }
    if (iconId >= 0) item->setIconId(iconId);
    item->setSize(32, 32);

    return item;
}
