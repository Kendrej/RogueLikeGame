#include "Player.h"

Player::Player(int playerId, uint32_t width, uint32_t height, float pos_x, float pos_y, int maxHp)
	: LivingEntity(playerId, width, height, pos_x, pos_y, maxHp){
    setMaxSpeed(300.0f);
    setAcceleration(2000.0f);
}

