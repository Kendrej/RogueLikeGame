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
    bool isAlive() const { return hp > 0; }
    void takeDamage(int dmg);
	void setDamaged(bool dmg) { damaged = dmg; }
	bool isDamaged() const { return damaged; }
    void heal(int amount);

    void setAttackDamage(int attackDamage_) {attackDamage = attackDamage_;}
    void setAttackCooldown(float attackCooldown_) {attackCooldown = attackCooldown_;}
    void setAttackRange(float attackRange_) {attackRange = attackRange_ ;}

    int getAttackDamage() const { return attackDamage ;}
    float getAttackCooldown() const { return attackCooldown ;}
    float getAttackRange() const { return attackRange ;}

    void resetAttackTimer() {attackTimer = 0.0f ;}
    bool canAttack() const {return attackTimer == 0.f; }

    void startAttackCooldown() {attackTimer = attackCooldown ;}

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
    int hp = 0;
    int maxHp;
	bool damaged = false;

    int attackDamage = 0;
    float attackCooldown = 0.f;
    float attackRange = 0.f;
    float attackTimer = 0.0f;


private:
    std::unique_ptr<AnimationController> animationController_{ nullptr };
};


