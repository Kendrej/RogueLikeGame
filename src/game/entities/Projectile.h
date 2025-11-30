#pragma once
#include "Entity.h"
class LivingEntity;

class Projectile : public Entity
{
public:
    Projectile(int textureId, uint32_t width, uint32_t height, float pos_x, float pos_y, ImVec2 velocity,
               float lifeTime, int damage, LivingEntity* owner)
        : Entity(textureId, width, height, pos_x, pos_y), velocity_(velocity), lifeTime_(lifeTime), damage_(damage),
          owner_(owner)
    {
        setSolid(false);
    }

    void update(float dt) override;

    int getDamage() const
    {
        return damage_;
    }
    bool isDead() const
    {
        return dead_;
    }
    LivingEntity* getOwner() const
    {
        return owner_;
    }
    void kill()
    {
        dead_ = true;
    }

private:
    int           damage_ = 1;
    ImVec2        velocity_{0.0f, 0.0f};
    float         lifeTime_ = 1.0f;
    LivingEntity* owner_    = nullptr;
    bool          dead_     = false;
};
