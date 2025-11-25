#include "Projectile.h"

void Projectile::update(float dt)
{
    lifeTime_ -= dt;
    if (lifeTime_ <= 0.0f) {
        dead_ = true;
        return;
    }
    moveBy(velocity_.x  *dt, velocity_.y * dt);
}
