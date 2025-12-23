#pragma once
#include "Item.h"

#include <memory>
#include <vector>

class Consumable;
class LivingEntity;

class Inventory
{
public:
    explicit Inventory(int capacity);

    bool addItem(std::unique_ptr<Item> item);
    bool useItem(int slot, LivingEntity& target);
    void removeItem(int slot);
    Item* getItem(int slot) const;

    int getItemCount() const { return static_cast<int>(items_.size()); }
    const std::vector<std::unique_ptr<Item>>& getItems() const { return items_; }
    int getInventoryCapacity() { return items_.size(); }

private:
    std::vector<std::unique_ptr<Item>> items_;
};

