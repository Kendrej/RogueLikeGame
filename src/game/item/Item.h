#pragma once
#include <string>
#include <utility>
#include "game/entities/Entity.h"



class Item : public Entity
{
public:
    enum class ItemType
    {
        Consumable,
        Equipment,
        Usable
    };

    const ItemType type;
    const std::string name;

    Item(ItemType t, int iconId, std::string n)
        : Entity(iconId, 16, 16, 0.0f, 0.0f,false, EntityType::Item), type(t), name(std::move(n))
    {
    }

    Item(ItemType t, int iconId, std::string n, float pos_x, float pos_y, uint32_t w = 16, uint32_t h = 16)
        : Entity(iconId, w, h, pos_x, pos_y), type(t), name(std::move(n))
    {
    }

    ~Item() override = default;

    ItemType getType() const { return type; }
    int getIconId() const { return getEntityId(); }
    const std::string& getName() const { return name; }

    void setIconId(int id) { setEntityId(id); }

private:
};
