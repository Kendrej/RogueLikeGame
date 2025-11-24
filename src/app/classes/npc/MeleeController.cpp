
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

    if ( dist > aggroRange) {
        npc.setState(Npc::State::Idle);
    }
    else if (dist <= attackRange + tolerance ) {
       npc.setState(Npc::State::Attack);
    }
    else if (dist > 0.001f) {
        npc.setState(Npc::State::Chase);
    }

    switch (npc.getState()) {
        case Npc::State::Idle:
            npc.applyInput(ImVec2(0.0f, 0.0f));
            break;
        case Npc::State::Attack:
            npc.applyInput(ImVec2(0.0f, 0.0f));

            if (npc.canAttack()) {
                player->takeDamage(npc.getAttackDamage());
                npc.startAttackCooldown();
            }
            break;
        case Npc::State::Chase:
            ImVec2 dir = normalize(ImVec2{
        playerCenter.x - npcCenter.x,
            playerCenter.y - npcCenter.y
        });
            npc.applyInput(dir);
            break;
    }


}
