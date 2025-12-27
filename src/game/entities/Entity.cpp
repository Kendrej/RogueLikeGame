#include "Entity.h"

#include "engine/gfx/Assets.h"
#include "game/item/Item.h"

Entity::Entity(int entityId, uint32_t width, uint32_t height, float pos_x, float pos_y, bool solid)
    : entityId(entityId), width(width), height(height), solid_(solid), pos{pos_x, pos_y}
{
}

Entity::~Entity() = default;

ImVec2 Entity::getPosition() const
{
    return pos;
}
uint32_t Entity::getWidth() const
{
    return width;
}
uint32_t Entity::getHeight() const
{
    return height;
}

void Entity::setSize(uint32_t w, uint32_t h)
{
    width = w;
    height = h;
}

int Entity::getEntityId() const
{
    return entityId;
}

void Entity::setEntityId(int id)
{
    entityId = id;
}

bool Entity::isVisible() const
{
    return visible;
}

void Entity::setPosition(float x, float y)
{
    pos.x = x;
    pos.y = y;
}

void Entity::moveBy(float dx, float dy)
{
    pos.x += dx;
    pos.y += dy;
}

void Entity::setVisible(bool isVisible)
{
    visible = isVisible;
}

void Entity::setDroppingItem(std::unique_ptr<Item> itemToDrop) {
    itemToDrop_ = std::move(itemToDrop);
}

Item* Entity::getDroppingItem() {
    return itemToDrop_.get();
}

std::unique_ptr<Item> Entity::takeDroppingItem() {
    return std::move(itemToDrop_);
}
