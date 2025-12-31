#include "Inventory.h"
#include "Consumable.h"
#include "game/entities/LivingEntity.h"
#include "game/factory/ItemFactory.h"
Inventory::Inventory(int capacity) {
    items_.resize(capacity);
}


bool Inventory::addItem(std::unique_ptr<Item> item) {
    if (!item) return false;

    for (int i =0; i < items_.size(); i++) {
        if (items_[i] == nullptr) {
            items_[i] = std::move(item);
            return true;
        }
    }
    return false;
}

bool Inventory::useItem(int slot, LivingEntity& target)
{
    if (slot < 0 || slot >= items_.size())
        return false;

    Item* item = items_[slot].get();
    if (!item)
        return false;

    if (item->getType() == Item::ItemType::Consumable)
    {
        auto* consumable = dynamic_cast<Consumable*>(item);
        if (consumable && consumable->use(target))
        {
            removeItem(slot);
            return true;
        }
    }

    return false;
}

void Inventory::removeItem(int slot)
{
    if (slot < 0 || slot >= items_.size())
        return;

    items_[slot].reset();
}

Item* Inventory::getItem(int slot) const
{
    if (slot < 0 || slot >= items_.size())
        return nullptr;

    return items_[slot].get();
}
