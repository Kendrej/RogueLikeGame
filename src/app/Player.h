#pragma once
#include "game.h"

class Player : public Entity {
public:
	Player(int spriteId, uint32_t width, uint32_t height, float pos_x, float pos_y, float speed = 200.0f);
	void update(float dt) override;
private:
	float speed_ = 200.0f;
};

