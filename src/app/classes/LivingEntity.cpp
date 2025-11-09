#include "LivingEntity.h"
#include "cmath"
LivingEntity::LivingEntity(int entityId, uint32_t width, uint32_t height, float pos_x, float pos_y)
    : Entity(entityId, width, height, pos_x, pos_y) {}

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
        desiredDir = ImVec2(dir.x/len , dir.y/len);
    } else {
        desiredDir= ImVec2(0.0f, 0.0f);
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
    moveBy(velocity.x * dt, velocity.y * dt);
}







