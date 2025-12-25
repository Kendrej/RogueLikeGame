#pragma once
#include "Entity.h"

#include <memory>
#include <string>
#include "animation/AnimationController.h"
class AnimationController;

class LivingEntity : public Entity
{
public:
    LivingEntity(int entityId, uint32_t width, uint32_t height, float pos_x, float pos_y, int maxHp_);

    ~LivingEntity(); // Declare destructor

    void update(float dt) override;

    ImVec2 getVelocity() const;
    float  getMaxSpeed() const;
    float  getAcceleration() const;

    void setVelocity(const ImVec2& velocity);
    void setMaxSpeed(float maxSpeed);
    void setAcceleration(float acceleration);
    void applyInput(const ImVec2& dir);

    int getHp() const
    {
        return hp;
    }
    int getMaxHp() const
    {
        return maxHp;
    }
    void setHp(int newHp) {
        if (newHp > maxHp){
            this->maxHp = newHp;
            this->hp = newHp;
        }
        else {
            this->hp = newHp;
        }
    }
    void heal(int amount);

    bool isAlive() const
    {
        return hp > 0;
    }
    void takeDamage(int dmg);
    void setDamaged(bool dmg)
    {
        damaged = dmg;
    }
    bool isDamaged() const
    {
        return damaged;
    }

    // RANGED
    void setRangedDamage(int rangedDamage_)
    {
        rangedDamage = rangedDamage_;
    }
    void setRangedCooldown(float rangedCooldown_)
    {
        rangedCooldown = rangedCooldown_;
    }
    void setRangedRange(float rangedRange_)
    {
        rangedRange = rangedRange_;
    }
    // resetRangedCooldown() { rangedTimer = 0.0f ;}
    void startRangedCooldown()
    {
        rangedTimer = rangedCooldown;
    }
    void setIsPerformingRangedAttack(bool val)
    {
        isPerformingRanged = val;
    }

    int getRangedDamage() const
    {
        return rangedDamage;
    }
    float getRangedCooldown() const
    {
        return rangedCooldown;
    }
    float getRangedRange() const
    {
        return rangedRange;
    }
    bool canShoot() const
    {
        return rangedTimer <= 0.0f;
    }
    bool isPerformingRangedAttack() const
    {
        return isPerformingRanged;
    }
    // melee
    void setMeleeDamage(int meleeDamage_)
    {
        meleeDamage = meleeDamage_;
    }
    void setMeleeCooldown(float meleeCooldown_)
    {
        meleeCooldown = meleeCooldown_;
    }
    void setAimLock(float time) {
        aimLockTimer = time;
    }
    void setMeleeRange(float meleeRange_)
    {
        meleeRange = meleeRange_;
    }
    // void resetMeleeCooldown() { meleeTimer = 0.0f ;}
    void startMeleeCooldown()
    {
        meleeTimer = meleeCooldown;
    }
    void setIsPerformingMeleeAttack(bool val)
    {
        isPerformingMelee = val;
    }

    int getMeleeDamage() const
    {
        return meleeDamage;
    }
    float getMeleeCooldown() const
    {
        return meleeCooldown;
    }
    float getMeleeRange() const
    {
        return meleeRange;
    }
    bool canMelee() const
    {
        return meleeTimer <= 0.0f;
    }
    bool isPerformingMeleeAttack() const
    {
        return isPerformingMelee;
    }

    ImVec2 getDesiredDir() const
    {
        return desiredDir;
    }
    ImVec2 getFacingDir() const
    {
        return facingDir;
    }
    void setFacingDir(const ImVec2& dir)
    {
        facingDir = dir;
    }

    AnimationController& createAnimationController(Assets* assets, const int squareSize,
                                                   const std::string& walkRightPath, int walkRightFrameAmount,
                                                   const std::string& walkLeftPath, int walkLeftFrameAmount,
                                                   const std::string& idleRightPath, int idleFrameRightAmount,
                                                   const std::string& idleLeftPath, int idleLeftFrameAmount,
                                                   const std::string hurtRightPath, const int hurtRightframeAmount,
                                                   const std::string hurtLeftPath, const int hurtLeftframeAmount,
                                                   const std::string deathRightPath, const int deathRightframeAmount,
                                                   const std::string deathLeftPath, const int deathLeftframeAmount);

    void createMeleeAttackAnimation(const int squareSize, const int meleeAttackFrame,
                                    const std::string& meleeAttackRightPath, const int meleeAttackRightFrameAmount,
                                    const std::string& meleeAttackLeftPath, const int meleeAttackLeftFrameAmount);

    void createRangedAttackAnimation(const int squareSize, const int rangedAttackFrame,
                                     const std::string& rangedAttackRightPath, const int rangedAttackRightFrameAmount,
                                     const std::string& rangedAttackLeftPath, const int rangedAttackLeftFrameAmount);

    AnimationController* getAnimationController() const
    {
        return animationController_.get();
    }

    // Sprite scale for rendering (independent of hitbox)
    float getSpriteScale() const { return spriteScale_; }
    void setSpriteScale(float scale) { spriteScale_ = scale; }

protected:
    ImVec2 velocity{0.0f, 0.0f};
    float  maxSpeed     = 5.0f;
    float  acceleration = 0.5f;
    ImVec2 desiredDir{0.0f, 0.0f};
    ImVec2 facingDir{1.0f, 0.0f};
    int    hp = 0;
    int    maxHp;
    bool   damaged = false;

    int   rangedDamage   = 0;
    float rangedCooldown = 0.f;
    float rangedRange    = 0.f;
    float rangedTimer    = 0.0f;

    int   meleeDamage   = 0;
    float meleeCooldown = 0.f;
    float meleeRange    = 0.f;
    float meleeTimer    = 0.0f;
    float aimLockTimer  = 0.0f;

    float spriteScale_ = 1.0f; // Visual scale for sprite rendering

private:
    bool                                 isPerformingMelee  = false;
    bool                                 isPerformingRanged = false;
    std::unique_ptr<AnimationController> animationController_{nullptr};
};
