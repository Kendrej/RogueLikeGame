#include "RangeController.h"

#include "Player.h"
#include "World.h"
#include "MathUtils.h"
#include "Npc.h"
#include "Assets.h"

void RangeController::update(Npc &npc, World &world, float dt) {
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

    npc.setRangedDamage(5);
    npc.setRangedRange(300.0f);
    npc.setRangedCooldown(1.0f);

    npc.setMaxSpeed(maxSpeed);
    npc.setAcceleration(accel);

    float attackRange = npc.getRangedRange();
    const float tolerance = 30.0f;

    if ( dist > aggroRange) {
        npc.setState(Npc::State::Idle);
    } else if ( dist <= attackRange * 0.8f) {
        npc.setState(Npc::State::Kite);
    }
    else if (dist <= attackRange * 1.2f ) {
        npc.setState(Npc::State::Attack);
    }
    else {
        npc.setState(Npc::State::Chase);
    }

    ImVec2 dirToPlayer = normalize(ImVec2{ playerCenter.x - npcCenter.x, playerCenter.y - npcCenter.y});

    switch (npc.getState()) {
        case Npc::State::Idle: {
             npc.applyInput(ImVec2(0.0f, 0.0f));
            break;
        }

        case Npc::State::Kite: {
            ImVec2 stepBack = normalize(ImVec2{
               npcCenter.x - playerCenter.x,
                   npcCenter.y - playerCenter.y
               });
            npc.applyInput(stepBack);

            if (npc.canShoot()) {
                // Ustaw kierunek patrzenia w stronê gracza przed strzelaniem
                npc.setFacingDir(dirToPlayer);
                if (!npc.getAnimationController()) {
                    world.performRangedAttack(npc);
                }
                else if(!npc.isPerformingRangedAttack()){
                    npc.setIsPerformingRangedAttack(true);
                }
                npc.startRangedCooldown();
            }

            break;
        }

        case Npc::State::Attack: {

            npc.applyInput(ImVec2(0.0f, 0.0f));
            npc.setVelocity(ImVec2(0.0f, 0.0f));
            if (npc.canShoot()) {
                // Ustaw kierunek patrzenia w stronê gracza przed strzelaniem
                npc.setFacingDir(dirToPlayer);
                if (!npc.getAnimationController()) {
                    world.performRangedAttack(npc);
                }
                else if (!npc.isPerformingRangedAttack()) {
                    npc.setIsPerformingRangedAttack(true);
                }
                npc.startRangedCooldown();
            }
            break;
        }

        case Npc::State::Chase: {
            npc.applyInput(dirToPlayer);
            break;
        }
    }
}
