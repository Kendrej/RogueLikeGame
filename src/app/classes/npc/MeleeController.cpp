
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

    ImVec2 npcCenter{
    npcPos.x + npc.getWidth() * 0.5f,
        npcPos.y + npc.getHeight() * 0.5f
    };

    ImVec2 playerCenter{
        playerPos.x + player->getWidth() * 0.5f,
        playerPos.y + player->getHeight() * 0.5f
    };
    float dist = distance(npcCenter, playerCenter);


    const float aggroRange  = 500.0f;
    const float maxSpeed    = 250.0f;
    const float accel       = 2000.0f;

    npc.setMaxSpeed(maxSpeed);
    npc.setAcceleration(accel);

    float attackRange = npc.getAttackRange();

    const float tolerance = 30.0f;

    //npc is out of aggro range so just doesnt move
    if ( dist > aggroRange) {
        npc.applyInput(ImVec2(0.0f, 0.0f));
        return;
    }
    // npc is too close so steps back
    if (dist < attackRange - tolerance) {
        ImVec2 dirBack = normalize({
        npcCenter.x - playerCenter.x,
            npcCenter.y - playerCenter.y
        });
        npc.applyInput(dirBack);
        return;
    }
    // npc in range, npc stays and hits player
    if (dist <= attackRange + tolerance ) {
        npc.applyInput(ImVec2(0.0f, 0.0f));

        if (npc.canAttack()) {
            player->takeDamage(npc.getAttackDamage());
            npc.startAttackCooldown();
        }
        return;
    }
    if (dist > 0.001f) {
        ImVec2 dir = normalize(ImVec2{
        playerCenter.x - npcCenter.x,
            playerCenter.y - npcCenter.y
        });
        npc.applyInput(dir);
    } else {
        npc.applyInput(ImVec2(0.0f, 0.0f));
    }
}
