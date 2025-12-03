#include "RangeController.h"

#include "utils/MathUtils.h"
#include "game/npc/Npc.h"
#include "game/entities/Player.h"
#include "game/world/World.h"


void RangeController::update(Npc& npc, World& world, float dt)
{
    Player* player = world.getPlayer();
    if (!player)
    {
        npc.applyInput(ImVec2(0.0f, 0.0f));
        return;
    }
    ImVec2 npcPos    = npc.getPosition();
    ImVec2 playerPos = player->getPosition();

    ImVec2 npcCenter{npcPos.x + static_cast<float>(npc.getWidth()) * 0.5f, npcPos.y + static_cast<float>(npc.getHeight()) * 0.5f};

    ImVec2 playerCenter{playerPos.x + static_cast<float>(player->getWidth()) * 0.5f, playerPos.y + static_cast<float>(player->getHeight()) * 0.5f};
    float  dist = distance(npcCenter, playerCenter);

    const float aggroRange = 500.0f;
    const float maxSpeed   = 250.0f;
    const float accel      = 2000.0f;

    npc.setRangedDamage(5);
    npc.setRangedRange(300.0f);
    npc.setRangedCooldown(1.0f);

    npc.setMaxSpeed(maxSpeed);
    npc.setAcceleration(accel);

    float       attackRange = npc.getRangedRange();

    if (dist > aggroRange)
    {
        npc.setState(Npc::State::Idle);
    }
    else if (dist <= attackRange * 0.8f)
    {
        npc.setState(Npc::State::Kite);
    }
    else if (dist <= attackRange * 1.2f)
    {
        npc.setState(Npc::State::Attack);
    }
    else
    {
        npc.setState(Npc::State::Chase);
    }

    ImVec2 dirToPlayer = normalize(ImVec2{playerCenter.x - npcCenter.x, playerCenter.y - npcCenter.y});

    switch (npc.getState())
    {
    case Npc::State::Idle:
    {
        npc.applyInput(ImVec2(0.0f, 0.0f));
        break;
    }

    case Npc::State::Kite:
    {
        ImVec2 stepBack = normalize(ImVec2{npcCenter.x - playerCenter.x, npcCenter.y - playerCenter.y});

        const float checkDist = 50.0f;
        ImVec2 avoidDir = findAvoidanceDirection(npc, world, stepBack, checkDist);
        npc.applyInput(avoidDir);

        if (npc.canShoot())
        {
            npc.setFacingDir(dirToPlayer);
            if (!npc.getAnimationController())
            {
                world.performRangedAttack(npc);
            }
            else if (!npc.isPerformingRangedAttack())
            {
                npc.setIsPerformingRangedAttack(true);
            }
            npc.startRangedCooldown();
        }

        break;
    }

    case Npc::State::Attack:
    {

        npc.applyInput(ImVec2(0.0f, 0.0f));
        npc.setVelocity(ImVec2(0.0f, 0.0f));
        if (npc.canShoot())
        {
            npc.setFacingDir(dirToPlayer);
            if (!npc.getAnimationController())
            {
                world.performRangedAttack(npc);
            }
            else if (!npc.isPerformingRangedAttack())
            {
                npc.setIsPerformingRangedAttack(true);
            }
            npc.startRangedCooldown();
        }
        break;
    }

    case Npc::State::Chase:
    {
        const float checkDist = 50.0f;
        ImVec2 avoidDir = findAvoidanceDirection(npc, world, dirToPlayer, checkDist);
        npc.applyInput(avoidDir);
        break;
    }
    }
}
