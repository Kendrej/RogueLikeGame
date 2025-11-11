#pragma once
#include "Entity.h"
#include <memory>

#include "LivingEntity.h"

class Assets;

class Player : public LivingEntity {
public:
	Player(int playerId, uint32_t width, uint32_t height, float pos_x, float pos_y, int maxHp);
	void update(float dt) override;
};
