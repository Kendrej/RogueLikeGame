#pragma once
#include "Entity.h"
#include "LivingEntity.h"
#include "game/item/Inventory.h"

#include <memory>

class Assets;
class World;

enum class AttackMode
{
    Melee,
    Ranged
};
class Player : public LivingEntity
{
public:
    Player(int playerId, uint32_t width, uint32_t height, float pos_x, float pos_y, int maxHp);
    void shoot(World& world);

    void setAttackMode(AttackMode attack_mode_)
    {
        attackMode = attack_mode_;
    }
    AttackMode getAttackMode() const
    {
        return attackMode;
    }

    void toggleAttackMode();
    bool isMeleeMode() const
    {
        return attackMode == AttackMode::Melee;
    }
    bool isRangedMode() const
    {
        return attackMode == AttackMode::Ranged;
    }

    Inventory& getInventory() { return inventory_; }
    const Inventory& getInventory() const { return inventory_; }

private:
    AttackMode attackMode = AttackMode::Melee;
    Inventory inventory_;
};
