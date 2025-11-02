#pragma once
#include <imgui.h>
#include <cstdint>
#include <vector>

class Assets;

class Entity {
public:
	Entity(int spriteId, uint32_t width, uint32_t height, float pos_x, float pos_y);
	virtual ~Entity() = default;

	ImVec2 getPosition() const;
	uint32_t getWidth() const;
	uint32_t getHeight() const;
	int getSpriteId() const;

	void setPosition(float x, float y);

	virtual void update(float dt) {}
	void moveBy(float dx, float dy);
private:
	int spriteId;
	uint32_t width = 0;
	uint32_t height = 0;
	ImVec2 pos{ 0.0f, 0.0f };
	bool visible = true;
};

Entity* spawn(std::vector<Entity*>& entities, Assets* assets,
	const char* path, uint32_t width, uint32_t height,
	float posX, float posY);
