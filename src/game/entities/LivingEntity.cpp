#include "LivingEntity.h"

#include "game/entities/animation/AnimationController.h"

#include <cmath>
#include <iostream>

#include "game/item/Item.h"

LivingEntity::LivingEntity(int entityId, uint32_t width, uint32_t height, float pos_x, float pos_y, int maxHp_, EntityType type)
    : Entity(entityId, width, height, pos_x, pos_y, true, type), maxHp(maxHp_), hp(maxHp_)
{
}

LivingEntity::~LivingEntity() = default;

ImVec2 LivingEntity::getVelocity() const
{
    return velocity;
}

float LivingEntity::getMaxSpeed() const
{
    return maxSpeed;
}

float LivingEntity::getAcceleration() const
{
    return acceleration;
}

void LivingEntity::setVelocity(const ImVec2& v)
{
    float speed = std::sqrt(v.x * v.x + v.y * v.y);
    if (speed > maxSpeed)
    {
        velocity = ImVec2(v.x * (maxSpeed / speed), v.y * (maxSpeed / speed));
    }
    else
    {
        velocity = v;
    }
}

void LivingEntity::setAcceleration(float newAcceleration)
{
    acceleration = newAcceleration;
}

void LivingEntity::setMaxSpeed(float newMaxSpeed)
{
    maxSpeed = newMaxSpeed;
}

void LivingEntity::applyInput(const ImVec2& dir)
{
    if ( !isAlive())
        return;

    if (dir.x != 0.0f || dir.y != 0.0f)
    {
        float  len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        ImVec2 norm{dir.x / len, dir.y / len};
        desiredDir = norm;
    }
    else
    {
        desiredDir = ImVec2(0.0f, 0.0f);
    }
    if (aimLockTimer <= 0.0f) {
        if (desiredDir.x != 0.0f || desiredDir.y != 0.0f)
        facingDir  = desiredDir;
    }
}

void LivingEntity::update(float dt)
{
    if (!isAlive())
        return;

    ImVec2 target{desiredDir.x * maxSpeed, desiredDir.y * maxSpeed};

    ImVec2 delta{target.x - velocity.x, target.y - velocity.y};
    float  step = acceleration * dt;
    float  len2 = delta.x * delta.x + delta.y * delta.y;
    if (len2 > step * step)
    {
        float len = std::sqrt(len2);
        velocity.x += (delta.x / len) * step;
        velocity.y += (delta.y / len) * step;
    }
    else
    {
        velocity = target;
    }
    if (rangedTimer > 0.f)
    {
        rangedTimer -= dt;
        if (rangedTimer < 0.f)
            rangedTimer = 0.f;
    }

    if (meleeTimer > 0.f)
    {
        meleeTimer -= dt;
        if (meleeTimer < 0.f)
            meleeTimer = 0.f;
    }
    if (aimLockTimer > 0.f)
    {
        aimLockTimer -= dt;
        if (aimLockTimer < 0.f)
            aimLockTimer = 0.f;
    }

    for (auto it = activeEffects_.begin(); it != activeEffects_.end(); )
    {
        it->duration -= dt;
        if (it->duration <= 0.0f)
        {
            if (it->onExpire)
                it->onExpire();
            it = activeEffects_.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void LivingEntity::takeDamage(int dmg)
{
    if (!isAlive())
        return;

    hp -= dmg;
    if (hp <= 0)
    {
        hp = 0;
        Die();
    }
    damaged = true;
}

void LivingEntity::Die() {
    isDead_ = true;
}


void LivingEntity::heal(int amount)
{
    if (!isAlive())
        return;
    hp += amount;
    if (hp > maxHp)
        hp = maxHp;
}

AnimationController& LivingEntity::createAnimationController(
    Assets* assets, const int squareSize, const std::string& walkRightPath, int walkRightFrameAmount,
    const std::string& walkLeftPath, int walkLeftFrameAmount, const std::string& idleRightPath,
    int idleFrameRightAmount, const std::string& idleLeftPath, int idleLeftFrameAmount, const std::string hurtRightPath,
    const int hurtRightframeAmount, const std::string hurtLeftPath, const int hurtLeftframeAmount,
    const std::string deathRightPath, const int deathRightframeAmount, const std::string deathLeftPath,
    const int deathLeftframeAmount)
{
    animationController_ = std::make_unique<AnimationController>(
        assets, squareSize, walkRightPath, walkRightFrameAmount, walkLeftPath, walkLeftFrameAmount, idleRightPath,
        idleFrameRightAmount, idleLeftPath, idleLeftFrameAmount, hurtRightPath, hurtRightframeAmount, hurtLeftPath,
        hurtLeftframeAmount, deathRightPath, deathRightframeAmount, deathLeftPath, deathLeftframeAmount);
    return *animationController_;
}

void LivingEntity::createMeleeAttackAnimation(const int squareSize, const int meleeAttackFrame,
                                              const std::string& meleeAttackRightPath,
                                              const int          meleeAttackRightFrameAmount,
                                              const std::string& meleeAttackLeftPath,
                                              const int          meleeAttackLeftFrameAmount)
{
    animationController_->addMeleeAttackAnimation(squareSize, meleeAttackFrame, meleeAttackRightPath,
                                                  meleeAttackRightFrameAmount, meleeAttackLeftPath,
                                                  meleeAttackLeftFrameAmount);
}

void LivingEntity::createRangedAttackAnimation(const int squareSize, const int rangedAttackFrame,
                                               const std::string& rangedAttackRightPath,
                                               const int          rangedAttackRightFrameAmount,
                                               const std::string& rangedAttackLeftPath,
                                               const int          rangedAttackLeftFrameAmount)
{
    animationController_->addRangedAttackAnimation(squareSize, rangedAttackFrame, rangedAttackRightPath,
                                                   rangedAttackRightFrameAmount, rangedAttackLeftPath,
                                                   rangedAttackLeftFrameAmount);
}