
#include "MeleeController.h"
#include "Npc.h"
#include "Player.h"
#include "World.h"
#include "MathUtils.h"

void MeleeController::update(Npc &npc, World &world, float dt) {
    Player* player = world.getPlayer();
    if (!player) {
        npc.applyInput(ImVec2(0.0f, 0.0f));
        return;
    }

    ImVec2 npcPos = npc.getPosition();
    ImVec2 playerPos = player->getPosition();

    float dist = distance(npcPos, playerPos);

    //chasing parameters
    const float aggroRange  = 500.0f;
    const float maxSpeed    = 250.0f;
    const float accel       = 2000.0f;

    float attackRange = npc.getAttackRange();

    if (dist > aggroRange ) {
        npc.applyInput(ImVec2(0.0f, 0.0f));
        return;
    }

    npc.setMaxSpeed(maxSpeed);
    npc.setAcceleration((accel));

   if (dist <= attackRange) {
       npc.applyInput(ImVec2(0.0f, 0.0f));

       if (npc.canAttack()) {
           player->takeDamage(npc.getAttackDamage());
           npc.startAttackCooldown();
       }
       return;
   }
    if (dist > 0.001f) {
        ImVec2 dir = normalize(ImVec2(playerPos.x - npcPos.x,
                              playerPos.y - npcPos.y));
        npc.applyInput(dir);
    } else {
        npc.applyInput(ImVec2(0.0f, 0.0f));
    }

}
