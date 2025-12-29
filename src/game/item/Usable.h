#pragma once
#include "Item.h"
#include "Item.h"

class Usable: public Item {
    Usable(int id, const std::string& n):Item(ItemType::Usable, id, n ) {}
    virtual ~Consumable() = default;
};