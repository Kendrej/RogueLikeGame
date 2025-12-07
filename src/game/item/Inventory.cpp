#include "Inventory.h"
#include "Consumable.h"
#include "game/entities/LivingEntity.h"

void Inventory::addItem(std::unique_ptr<Item> item)
{
    if (static_cast<int>(items_.size()) >= MAX_SLOTS)
        return;

    items_.push_back(std::move(item));
}

bool Inventory::useItem(int slot, LivingEntity& target)
{
    if (slot < 0 || slot >= static_cast<int>(items_.size()))
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
    if (slot < 0 || slot >= static_cast<int>(items_.size()))
        return;

    items_.erase(items_.begin() + slot);
}

Item* Inventory::getItem(int slot) const
{
    if (slot < 0 || slot >= static_cast<int>(items_.size()))
        return nullptr;

    return items_[slot].get();
}
