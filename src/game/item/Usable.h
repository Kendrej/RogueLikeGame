#pragma once
#include "Item.h"

class Usable: public Item {
public:
    Usable(int id, const std::string& n):Item(ItemType::Usable, id, n ) {}
    virtual ~Usable() = default;

};