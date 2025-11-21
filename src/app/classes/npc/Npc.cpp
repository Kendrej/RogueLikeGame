#include "Npc.h"
#include "../World.h"

void Npc::setController(std::unique_ptr<INpcController> ctrl) {
    controller = std::move(ctrl);
}
void Npc::update(float dt) {
    if (controller && world_) {
        controller->update(*this, *world_, dt);

    }
    LivingEntity::update(dt);
}

