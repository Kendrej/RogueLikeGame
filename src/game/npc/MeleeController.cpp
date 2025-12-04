#include "MeleeController.h"

#include "utils/MathUtils.h"
#include "game/npc/Npc.h"
#include "game/entities/Player.h"
#include "game/world/World.h"


void MeleeController::update(Npc& npc, World& world, float dt)
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

    ImVec2 dirToPlayer = normalize(ImVec2{playerCenter.x - npcCenter.x, playerCenter.y - npcCenter.y});

    const float aggroRange = 500.0f;
    const float maxSpeed   = 250.0f;
    const float accel      = 2000.0f;

    npc.setMeleeRange(80.0f);
    npc.setMeleeDamage(5);
    npc.setMeleeCooldown(1.0f);
    npc.setMaxSpeed(maxSpeed);
    npc.setAcceleration(accel);

    float attackRange = npc.getMeleeRange();

    Npc::State currentState = npc.getState();

    if (dist > aggroRange)
    {
        npc.setState(Npc::State::Idle);
    }
    else if (currentState == Npc::State::Attack)
    {
        if (dist > attackRange * 1.3f)
            npc.setState(Npc::State::Chase);
    }
    else if (currentState == Npc::State::Chase)
    {
        if (dist <= attackRange * 0.9f)
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

    case Npc::State::Attack:
    {
        npc.applyInput(ImVec2(0.0f, 0.0f));
        npc.setVelocity(ImVec2(0.0f, 0.0f));
        if (npc.canMelee())
        {
            if (!npc.getAnimationController())
            {
                world.performMeleeAttack(npc);
            }
            else
            {
                npc.setIsPerformingMeleeAttack(true);
            }
            npc.startMeleeCooldown();
        }
        break;
    }

    case Npc::State::Chase:
    {
        npc.applyInput(dirToPlayer);
        break;
    }

    case Npc::State::Kite:
    {
        // Melee NPC nie używa kite state, ale dodajemy dla kompletności
        npc.setState(Npc::State::Chase);
        break;
    }
    }
}
