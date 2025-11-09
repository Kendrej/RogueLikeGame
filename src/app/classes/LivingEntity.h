#pragma once
#include "Entity.h"

class LivingEntity: public Entity {
public:
    LivingEntity(int entityId, uint32_t width, uint32_t height, float pos_x, float pos_y);

    virtual ~LivingEntity() = default;

    void update(float dt) override;
    ImVec2 getVelocity() const;
    float getMaxSpeed() const;
    float getAcceleration() const;

    void setVelocity(const ImVec2 &velocity);
    void setMaxSpeed(float maxSpeed);
    void setAcceleration(float acceleration);

    void applyInput(const ImVec2& dir);
protected:
    ImVec2 velocity{ 0.0f, 0.0f };
    float maxSpeed = 5.0f;
    float acceleration = 0.5f;
    ImVec2 desiredDir{0.0f, 0.0f};
private:

};


