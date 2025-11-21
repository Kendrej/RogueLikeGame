#pragma once
#include "Entity.h"

class StaticEntity : public Entity {
public:
    StaticEntity(int entityId, uint32_t width, uint32_t height,
                 float pos_x, float pos_y, bool solid);
};
