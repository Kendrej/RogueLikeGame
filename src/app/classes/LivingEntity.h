#pragma once
#include "Entity.h"

class LivingEntity: public Entity {
public:
    LivingEntity(int entityId, uint32_t width, uint32_t height, float pos_x, float pos_y, int maxHp_);

    virtual ~LivingEntity() = default;

    void update(float dt) override;
    ImVec2 getVelocity() const;
    float getMaxSpeed() const;
    float getAcceleration() const;

    void setVelocity(const ImVec2 &velocity);
    void setMaxSpeed(float maxSpeed);
    void setAcceleration(float acceleration);
    void applyInput(const ImVec2& dir);

    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }
    bool isAlive() const { return hp > 0; }
    void takeDamage(int dmg);
    void heal(int amount);

    void setAttackDamage(int attackDamage_) {attackDamage = attackDamage;}
    void setAttackCooldown(float attackCooldown_) {attackCooldown = attackCooldown;}
    void setAttackRange(float attackRange_) {attackRange = attackRange_ ;}

    int getAttackDamage() const { return attackDamage ;}
    float getAttackCooldown() const { return attackCooldown ;}
    float getAttackRange() const { return attackCooldown ;}

    void resetAttackTimer() {attackTimer = 0.0f ;}
    bool canAttack() const {return attackTimer <= 0.f; }
protected:
    ImVec2 velocity{ 0.0f, 0.0f };
    float maxSpeed = 5.0f;
    float acceleration = 0.5f;
    ImVec2 desiredDir{0.0f, 0.0f};
    int hp = 0;
    int maxHp;

    int attackDamage =5;
    float attackCooldown = 0.5f;
    float attackRange = 40.f;
    float attackTimer = 0.0f;


private:

};


