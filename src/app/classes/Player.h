#pragma once
#include "entity.h"
#include <memory>

class Assets;

class Player : public Entity {
public:
	Player(int spriteId, uint32_t width, uint32_t height, float pos_x, float pos_y, float speed =200.0f);
	void update(float dt) override;
	static void spawnPlayer(std::unique_ptr<Player>& outPlayer, Assets* assets, const char* path, uint32_t width, uint32_t height, float posX, float posY);
private:
	float speed_ =200.0f;
};
