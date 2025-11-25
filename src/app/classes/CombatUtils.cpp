#include "CombatUtils.h"
#include "World.h"
#include "MathUtils.h"
#include "LivingEntity.h"
#include <iostream>

void shootProjectile(World& world,LivingEntity& owner, const std::string& texturePath ,uint32_t width,
    uint32_t height,const ImVec2& startPos,const ImVec2& dir,float speed,float lifetime,int damage )
{
    if (dir.x == 0.0f && dir.y == 0.0f) return;

    ImVec2 dirNorm = normalize(dir);

    ImVec2 velocity { dirNorm.x * speed, dirNorm.y * speed};
    world.spawnProjectile(width, height, startPos.x, startPos.y, velocity, lifetime, damage, &owner,texturePath);
}
