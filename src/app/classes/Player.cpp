#include "Player.h"
#include "CombatUtils.h"
#include "World.h"

Player::Player(int playerId, uint32_t width, uint32_t height, float pos_x, float pos_y, int maxHp)
	: LivingEntity(playerId, width, height, pos_x, pos_y, maxHp){
    setMaxSpeed(300.0f);
    setAcceleration(2000.0f);
}

void Player::shoot(World &world) {
	const std::string projTexture = "assets/design/Arrow01.png" ;
	const uint32_t projW = 32;
	const uint32_t projH = 32;
	const float projSpeed = 600.0f;
	const float projLifetime = 3.0f;

	const float spawnOffset = getWidth() * 0.5f + 10.0f;

	ImVec2 pos = getPosition();
	ImVec2 playerCenter{ pos.x + 0.5f * getWidth(), pos.y + 0.5f * getHeight() };
    ImVec2 dir = getDesiredDir();
    if (dir.x != 0.0f || dir.y != 0.0f) {
        facingDir = dir;
    }

	ImVec2 spawnPos{
		playerCenter.x + facingDir.x * spawnOffset,
		playerCenter.y + facingDir.y * spawnOffset
	};

	shootProjectile(world, *this, projTexture, projW, projH, spawnPos, facingDir, projSpeed, projLifetime, getRangedDamage());
}
