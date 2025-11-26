#pragma once
#include "Entity.h"
#include <memory>
#include <string>

class AnimationController;

class LivingEntity: public Entity {
public:
    LivingEntity(int entityId, uint32_t width, uint32_t height, float pos_x, float pos_y, int maxHp_);

    ~LivingEntity(); // Declare destructor

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
    void heal(int amount);

    bool isAlive() const { return hp > 0; }
    void takeDamage(int dmg);
	void setDamaged(bool dmg) { damaged = dmg; }
	bool isDamaged() const { return damaged; }

    // RANGED
    void setRangedDamage(int rangedDamage_) { rangedDamage = rangedDamage_;}
    void setRangedCooldown(float rangedCooldown_) { rangedCooldown = rangedCooldown_;}
    void setRangedRange(float rangedRange_) {rangedRange = rangedRange_ ;}
    // resetRangedCooldown() { rangedTimer = 0.0f ;}
    void startRangedCooldown() { rangedTimer = rangedCooldown ;}


    int getRangedDamage() const { return rangedDamage ;}
    float getRangedCooldown() const { return rangedCooldown ;}
    float getRangedRange() const { return rangedRange ;}
    bool canShoot() const { return rangedTimer <= 0.0f ;}

    // melee
    void setMeleeDamage(int meleeDamage_) { meleeDamage = meleeDamage_;}
    void setMeleeCooldown(float meleeCooldown_) { meleeCooldown = meleeCooldown_;}
    void setMeleeRange(float meleeRange_) { meleeRange = meleeRange_ ;}
    //void resetMeleeCooldown() { meleeTimer = 0.0f ;}
    void startMeleeCooldown() { meleeTimer = meleeCooldown ;}

    int getMeleeDamage() const { return meleeDamage ;}
    float getMeleeCooldown() const { return meleeCooldown ;}
    float getMeleeRange() const { return meleeRange ;}
    bool canMelee() const {return meleeTimer <= 0.0f; }


    ImVec2 getDesiredDir() const { return desiredDir; }
    ImVec2 getFacingDir() const { return facingDir; }

    AnimationController& createAnimationController(Assets* assets,const int squareSize,
        const std::string& walkRightPath, int walkRightFrameAmount,
        const std::string& walkLeftPath, int walkLeftFrameAmount,
        const std::string& idleRightPath, int idleFrameRightAmount,
        const std::string& idleLeftPath, int idleLeftFrameAmount,
        const std::string hurtRightPath, const int hurtRightframeAmount,
        const std::string hurtLeftPath, const int hurtLeftframeAmount);

    AnimationController* getAnimationController() const {
        return animationController_.get();
    }
protected:
    ImVec2 velocity{ 0.0f, 0.0f };
    float maxSpeed = 5.0f;
    float acceleration = 0.5f;
    ImVec2 desiredDir{0.0f, 0.0f};
    ImVec2 facingDir{1.0f, 0.0f};
    int hp = 0;
    int maxHp;
	bool damaged = false;

    int rangedDamage = 0;
    float rangedCooldown = 0.f;
    float rangedRange = 0.f;
    float rangedTimer = 0.0f;

    int meleeDamage = 0;
    float meleeCooldown = 0.f;
    float meleeRange = 0.f;
    float meleeTimer = 0.0f;


private:
    std::unique_ptr<AnimationController> animationController_{ nullptr };
};
