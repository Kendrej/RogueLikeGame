#include "Player.h"

#include "game/world/World.h"

Player::Player(int playerId, uint32_t width, uint32_t height, float pos_x, float pos_y, int maxHp)
    : LivingEntity(playerId, width, height, pos_x, pos_y, maxHp), inventory_(10)
{
}

void Player::toggleAttackMode()
{
    if (attackMode == AttackMode::Melee)
    {
        attackMode = AttackMode::Ranged;
    }
    else
    {
        attackMode = AttackMode::Melee;
    }
}
