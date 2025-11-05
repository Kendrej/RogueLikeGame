#include "entity.h"
#include "../../engine/gfx/Assets.h"

Entity::Entity(int spriteId, uint32_t width, uint32_t height, float pos_x, float pos_y)
	: spriteId(spriteId), width(width), height(height), pos{ pos_x, pos_y } {}

ImVec2 Entity::getPosition() const { return pos; }
uint32_t Entity::getWidth() const { return width; }
uint32_t Entity::getHeight() const { return height; }
int Entity::getSpriteId() const { return spriteId; }

void Entity::setPosition(float x, float y) {
	pos.x = x; pos.y = y;
}

void Entity::moveBy(float dx, float dy) {
	pos.x += dx; pos.y += dy;
}

void Entity::spawn(std::vector<std::unique_ptr<Entity>>& entities, Assets* assets,
	const char* path, uint32_t width, uint32_t height,
	float posX, float posY)
{
	int spriteId = assets->getOrLoad(path);
	entities.emplace_back(std::make_unique<Entity>(spriteId, width, height, posX, posY));
}

