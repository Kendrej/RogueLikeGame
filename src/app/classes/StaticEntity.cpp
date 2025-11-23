#include "StaticEntity.h"

StaticEntity::StaticEntity(int entityId, uint32_t width, uint32_t height,
                           float pos_x, float pos_y, bool solid)
     : Entity(entityId, width, height, pos_x, pos_y)
{
    setSolid(solid);
}