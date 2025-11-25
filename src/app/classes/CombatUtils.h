#pragma once
#include <cstdint>
#include <string>
#include "imgui.h"

class World;
class LivingEntity;

void shootProjectile(World& world,LivingEntity& owner,const std::string& texturePath ,uint32_t width,uint32_t height,
    const ImVec2& startPos,const ImVec2& dir,float speed,float lifetime,int damage);
