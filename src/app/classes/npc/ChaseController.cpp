
#include "ChaseController.h"
#include "Npc.h"
#include "Player.h"
#include "world.h"
#include <cmath>


void ChaseController::update(Npc &npc, World &world, float dt) {
    Player* player = world.getPlayer();
    if (!player) {
        npc.applyInput(ImVec2(0.0f, 0.0f));
    }

    ImVec2 npcPos = npc.getPosition();
    ImVec2 playerPos = player->getPosition();

    float dx = playerPos.x - npcPos.x;
    float dy = playerPos.y - npcPos.y;

    const float aggrorange = 600.0f;
    float dist2 = dx*dx + dy*dy;
    if (dist2 > aggrorange * aggrorange ) {
        npc.applyInput(ImVec2(0.0f,0.0f));
        return;
    }

    npc.setMaxSpeed(300.0f);
    npc.setAcceleration(2000.0f);
    ImVec2 dir{dx, dy};
    npc.applyInput(dir);

}
