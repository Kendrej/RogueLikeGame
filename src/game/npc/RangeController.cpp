#include "RangeController.h"

#include "utils/MathUtils.h"
#include "game/npc/Npc.h"
#include "game/entities/Player.h"
#include "game/world/World.h"
#include "game/entities/animation/AnimationController.h"


void RangeController::update(Npc& npc, World& world, float /*dt*/)
{
    Player* player = world.getPlayer();
    if (!player)
    {
        npc.applyInput(ImVec2(0.0f, 0.0f));
        return;
    }
    ImVec2 npcPos    = npc.getPosition();
    ImVec2 playerPos = player->getPosition();
    ImVec2 dirToPlayer = world.getDirToPlayer(&npc);
    ImVec2 npcCenter{npcPos.x + static_cast<float>(npc.getWidth()) * 0.5f, npcPos.y + static_cast<float>(npc.getHeight()) * 0.5f};
    ImVec2 playerCenter{playerPos.x + static_cast<float>(player->getWidth()) * 0.5f, playerPos.y + static_cast<float>(player->getHeight()) * 0.5f};
    float  dist = distance(npcCenter, playerCenter);

    float aggroRange = npc.getAggroRange();
    float attackRange = npc.getRangedRange();

    Npc::State currentState = npc.getState();

    if (dist > aggroRange)
    {
        npc.setState(Npc::State::Idle);
    }
    else if (currentState == Npc::State::Attack)
    {
        if (dist <= attackRange * 0.7f)
            npc.setState(Npc::State::Kite);
        else if (dist > attackRange * 1.3f)
            npc.setState(Npc::State::Chase);
    }
    else if (currentState == Npc::State::Kite)
    {
        if (dist > attackRange * 0.9f && dist <= attackRange * 1.2f)
            npc.setState(Npc::State::Attack);
        else if (dist > attackRange * 1.4f)
            npc.setState(Npc::State::Chase);
    }
    else if (currentState == Npc::State::Chase)
    {
        if (dist <= attackRange * 1.1f)
            npc.setState(Npc::State::Attack);
    }
    else if (currentState == Npc::State::Idle)
    {
        if (dist <= aggroRange * 0.95f)
            npc.setState(Npc::State::Chase);
    }


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
        npc.setFacingDir(dirToPlayer);

        if (npc.canShoot())
        {
            npc.setFacingDir(dirToPlayer);
            if (!npc.getAnimationController())
            {
                world.performRangedAttack(npc, dirToPlayer);
                npc.setIsPerformingRangedAttack(false);
            }
            else if (!npc.isPerformingRangedAttack())
            {
                npc.setAimLock(0.5f);
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
        npc.setFacingDir(dirToPlayer);

        if (npc.canShoot())
        {
            npc.setFacingDir(dirToPlayer);
            if (!npc.getAnimationController())
            {
                npc.setFacingDir(dirToPlayer);
                world.performRangedAttack(npc, dirToPlayer);
                npc.setIsPerformingRangedAttack(false);
            }
            else if (!npc.isPerformingRangedAttack())
            {
                npc.setAimLock(0.5f);
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
