#include "LivingEntity.h"
#include "AnimationController.h"
#include <cmath>
#include <iostream>

LivingEntity::LivingEntity(int entityId, uint32_t width, uint32_t height, float pos_x, float pos_y, int maxHp_)
    : Entity(entityId, width, height, pos_x, pos_y), maxHp(maxHp_), hp(maxHp_) {}

LivingEntity::~LivingEntity() = default;

ImVec2 LivingEntity::getVelocity() const {
    return velocity;
}

float LivingEntity::getMaxSpeed() const {
    return maxSpeed;
}

float LivingEntity::getAcceleration() const {
  return acceleration;
}

void LivingEntity::setVelocity(const ImVec2& v) {
    float speed = std::sqrt(v.x * v.x + v.y * v.y);
    if (speed > maxSpeed) {
  velocity = ImVec2(v.x * (maxSpeed/speed), v.y * (maxSpeed/speed));
    } else {
        velocity = v;
    }
}

void LivingEntity::setAcceleration(float acceleration) {
    this->acceleration = acceleration;
}

void LivingEntity::setMaxSpeed(float maxSpeed) {
    this->maxSpeed = maxSpeed;
}

void LivingEntity::applyInput(const ImVec2 &dir) {
    if (dir.x != 0.0f || dir.y != 0.0f) {
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        ImVec2 norm{ dir.x / len, dir.y / len };
        desiredDir = norm;
        facingDir = norm;
    } else {
        desiredDir = ImVec2(0.0f, 0.0f);
    }
}

void LivingEntity::update(float dt) {
    ImVec2 target{ desiredDir.x * maxSpeed, desiredDir.y * maxSpeed};

    ImVec2 delta {target.x - velocity.x, target.y - velocity.y};
    float step = acceleration * dt;
    float len2 = delta.x * delta.x + delta.y * delta.y;
    if (len2 > step * step) {
        float len = std::sqrt(len2);
     velocity.x += (delta.x / len) * step;
        velocity.y += (delta.y / len) * step;
    } else {
  velocity = target;
    }
    if (rangedTimer > 0.f) {
     rangedTimer -= dt;
        if (rangedTimer < 0.f) rangedTimer = 0.f;
    }

    if ( meleeTimer > 0.f ) {
        meleeTimer -= dt;
        if ( meleeTimer < 0.f) meleeTimer = 0.f;
    }
}

void LivingEntity::takeDamage(int dmg) {
    if (!isAlive()) return;

    hp -= dmg;
    if (hp < 0) hp = 0;
	damaged = true;
    std::cout << "[LivingEntity] entityId=" << this->getEntityId() << " took " << dmg << " dmg -> hp=" << hp << "/" << maxHp << std::endl;
}

void LivingEntity::heal(int amount) {
    if (!isAlive()) return;
    hp += amount;
    if (hp > maxHp) hp = maxHp;
    std::cout << "[LivingEntity] entityId=" << this->getEntityId() << " healed " << amount << " -> hp=" << hp << "/" << maxHp << std::endl;
}

AnimationController& LivingEntity::createAnimationController(Assets* assets, const int squareSize, const std::string& walkRightPath, int walkRightFrameAmount,
                                                            const std::string& walkLeftPath, int walkLeftFrameAmount,
                                                            const std::string& idleRightPath, int idleFrameRightAmount,
                                                            const std::string& idleLeftPath, int idleLeftFrameAmount,
                                                            const std::string hurtRightPath, const int hurtRightframeAmount,
                                                            const std::string hurtLeftPath, const int hurtLeftframeAmount) {
    animationController_ = std::make_unique<AnimationController>(assets,squareSize, walkRightPath, walkRightFrameAmount, walkLeftPath, walkLeftFrameAmount,
                                                                idleRightPath, idleFrameRightAmount, idleLeftPath, idleLeftFrameAmount, hurtRightPath, hurtRightframeAmount, hurtLeftPath, hurtLeftframeAmount);
    return *animationController_;
}
