#include "entity.h"
#include "../../engine/gfx/Assets.h"

Entity::Entity(int entityId, uint32_t width, uint32_t height, float pos_x, float pos_y)
	: entityId(entityId), width(width), height(height), pos{ pos_x, pos_y } {}

ImVec2 Entity::getPosition() const { return pos; }
uint32_t Entity::getWidth() const { return width; }
uint32_t Entity::getHeight() const { return height; }
int Entity::getEntityId() const { return entityId; }
bool Entity::isVisible() const { return visible; }

void Entity::setPosition(float x, float y) {
	pos.x = x; pos.y = y;
}

void Entity::moveBy(float dx, float dy) {
	pos.x += dx; pos.y += dy;
}

void Entity::setVisible(bool visible) {
	this->visible = visible;
}

