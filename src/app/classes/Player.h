#pragma once
#include "Entity.h"
#include <memory>

#include "LivingEntity.h"

class Assets;
class World;
class Player : public LivingEntity {
public:
	Player(int playerId, uint32_t width, uint32_t height, float pos_x, float pos_y, int maxHp);
	void shoot(World& world);
private:
	ImVec2 facingDir{1.0f, 0.0f};

	float meleeRange_ = 6
};
