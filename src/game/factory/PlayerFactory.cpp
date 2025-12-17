#include "PlayerFactory.h"
#include "../entities/Player.h"
#include "game/world/World.h"

Player* PlayerFactory::createPlayer(World& world, ImVec2 pos)
{
    int   width  = 64;
    int   height = 64;
    int   maxHp  = 100;

    float maxSpeed = 300.0f;
    float accel    = 2000.0f;

    int   meleeDamage   = 5;
    float meleeCooldown = 0.7f;
    float meleeRange    = 80.0f;

    int   rangedDamage   = 10;
    float rangedCooldown = 0.7f;
    float rangedRange    = 200.0f;

    std::string animBasePath    = "assets/animations/soldier/";
    std::string animPrefix      = "Soldier";
    int         animSquareSize  = 100;

    int walkFrames  = 8;
    int idleFrames  = 6;
    int hurtFrames  = 4;
    int deathFrames = 4;

    int meleeAttackFrames   = 6;
    int meleeAttackTrigger  = 4;
    int rangedAttackFrames  = 9;
    int rangedAttackTrigger = 8;

    auto* player = new Player(-1, width, height, pos.x, pos.y, maxHp);

    player->setSolid(true);
    player->setMaxSpeed(maxSpeed);
    player->setAcceleration(accel);

    player->setMeleeDamage(meleeDamage);
    player->setMeleeCooldown(meleeCooldown);
    player->setMeleeRange(meleeRange);

    player->setRangedDamage(rangedDamage);
    player->setRangedCooldown(rangedCooldown);
    player->setRangedRange(rangedRange);

    player->createAnimationController(
        world.getAssets(), animSquareSize,
        animBasePath + animPrefix + "-Walk-right.png", walkFrames,
        animBasePath + animPrefix + "-Walk-left.png", walkFrames,
        animBasePath + animPrefix + "-Idle-right.png", idleFrames,
        animBasePath + animPrefix + "-Idle-left.png", idleFrames,
        animBasePath + animPrefix + "-Hurt-right.png", hurtFrames,
        animBasePath + animPrefix + "-Hurt-left.png", hurtFrames,
        animBasePath + animPrefix + "-Death-right.png", deathFrames,
        animBasePath + animPrefix + "-Death-left.png", deathFrames);

    player->createMeleeAttackAnimation(
        animSquareSize, meleeAttackTrigger,
        animBasePath + animPrefix + "-Attack-right.png", meleeAttackFrames,
        animBasePath + animPrefix + "-Attack-left.png", meleeAttackFrames);

    player->createRangedAttackAnimation(
        animSquareSize, rangedAttackTrigger,
        animBasePath + animPrefix + "-RangeAttack-right.png", rangedAttackFrames,
        animBasePath + animPrefix + "-RangeAttack-left.png", rangedAttackFrames);

    return player;
}
