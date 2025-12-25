#include "AnimationController.h"

#include "engine/gfx/Assets.h"

AnimationController::AnimationController(Assets* assets, const int squareSize, const std::string walkRightPath,
                                         const int walkRightframeAmount, const std::string walkLeftPath,
                                         const int walkLeftframeAmount, const std::string idleRightPath,
                                         const int idlesRightframeAmount, const std::string idleLeftPath,
                                         const int idlesLeftframeAmount, const std::string hurtRightPath,
                                         const int hurtRightframeAmount, const std::string hurtLeftPath,
                                         const int hurtLeftframeAmount, const std::string deathRightPath,
                                         const int deathRightframeAmount, const std::string deathLeftPath,
                                         const int deathLeftframeAmount)
    : assets_(assets), walkRightFrameAmount_(walkRightframeAmount), walkLeftFrameAmount_(walkLeftframeAmount),
      idleRightFrameAmount_(idlesRightframeAmount), idleLeftFrameAmount_(idlesLeftframeAmount),
      hurtRightFrameAmount_(hurtRightframeAmount), hurtLeftFrameAmount_(hurtLeftframeAmount),
      deathRightFrameAmount_(deathRightframeAmount), deathLeftFrameAmount_(deathLeftframeAmount)
{
    addWalkAnimation(squareSize, walkRightPath, walkLeftPath);

    addIdleAnimation(squareSize, idleRightPath, idleLeftPath);

    addHurtAnimation(squareSize, hurtRightPath, hurtLeftPath);

    adddeathAnimation(squareSize, deathRightPath, deathLeftPath);
}

void AnimationController::addWalkAnimation(const int squareSize, const std::string walkRightPath,
                                           const std::string walkLeftPath)
{
    int walkRightId = assets_->loadSpriteSheet(walkRightPath, walkRightFrameAmount_, squareSize, squareSize);
    setAnimationIconId(AnimationType::WalkRight, walkRightId);

    int walkLeftId = assets_->loadSpriteSheet(walkLeftPath, walkLeftFrameAmount_, squareSize, squareSize);
    setAnimationIconId(AnimationType::WalkLeft, walkLeftId);
}

void AnimationController::addIdleAnimation(const int squareSize, const std::string idleRightPath,
                                           const std::string idleLeftPath)
{
    int idleRightId = assets_->loadSpriteSheet(idleRightPath, idleRightFrameAmount_, squareSize, squareSize);
    setAnimationIconId(AnimationType::IdleRight, idleRightId);

    int idleLeftId = assets_->loadSpriteSheet(idleLeftPath, idleLeftFrameAmount_, squareSize, squareSize);
    setAnimationIconId(AnimationType::IdleLeft, idleLeftId);
}

void AnimationController::addHurtAnimation(const int squareSize, const std::string hurtRightPath,
                                           const std::string hurtLeftPath)
{
    int hurtRightId = assets_->loadSpriteSheet(hurtRightPath, hurtRightFrameAmount_, squareSize, squareSize);
    setAnimationIconId(AnimationType::HurtRight, hurtRightId);

    int hurtLeftId = assets_->loadSpriteSheet(hurtLeftPath, hurtLeftFrameAmount_, squareSize, squareSize);
    setAnimationIconId(AnimationType::HurtLeft, hurtLeftId);
}

void AnimationController::adddeathAnimation(const int squareSize, const std::string deathRightPath,
                                            const std::string deathLeftPath)
{
    int deathRightId = assets_->loadSpriteSheet(deathRightPath, deathRightFrameAmount_, squareSize, squareSize);
    setAnimationIconId(AnimationType::DeathRight, deathRightId);

    int deathLeftId = assets_->loadSpriteSheet(deathLeftPath, deathLeftFrameAmount_, squareSize, squareSize);
    setAnimationIconId(AnimationType::DeathLeft, deathLeftId);
}

void AnimationController::addMeleeAttackAnimation(const int squareSize, const int meleeAtackFrame,
                                                  const std::string meleeAttackRightPath,
                                                  const int         meleeAttackRightFrameAmount,
                                                  const std::string meleeAttackLeftPath,
                                                  const int         meleeAttackLeftFrameAmount)
{
    meleeAttackFrame_            = meleeAtackFrame;
    meleeAttackRightFrameAmount_ = meleeAttackRightFrameAmount;
    meleeAttackLeftFrameAmount_  = meleeAttackLeftFrameAmount;

    int meleeAttackRightId =
        assets_->loadSpriteSheet(meleeAttackRightPath, meleeAttackRightFrameAmount, squareSize, squareSize);
    setAnimationIconId(AnimationType::MeleeAttackRight, meleeAttackRightId);

    int meleeAttackLeftId =
        assets_->loadSpriteSheet(meleeAttackLeftPath, meleeAttackLeftFrameAmount, squareSize, squareSize);
    setAnimationIconId(AnimationType::MeleeAttackLeft, meleeAttackLeftId);
}
void AnimationController::addRangedAttackAnimation(const int squareSize, const int rangedAttackFrame,
                                                   const std::string rangedAttackRightPath,
                                                   const int         rangedAttackRightFrameAmount,
                                                   const std::string rangedAttackLeftPath,
                                                   const int         rangedAttackLeftFrameAmount)
{
    rangedAttackFrame_            = rangedAttackFrame;
    rangedAttackRightFrameAmount_ = rangedAttackRightFrameAmount;
    rangedAttackLeftFrameAmount_  = rangedAttackLeftFrameAmount;

    int rangedAttackRightId =
        assets_->loadSpriteSheet(rangedAttackRightPath, rangedAttackRightFrameAmount, squareSize, squareSize);
    setAnimationIconId(AnimationType::RangedAttackRight, rangedAttackRightId);

    int rangedAttackLeftId =
        assets_->loadSpriteSheet(rangedAttackLeftPath, rangedAttackLeftFrameAmount, squareSize, squareSize);
    setAnimationIconId(AnimationType::RangedAttackLeft, rangedAttackLeftId);
}

void AnimationController::update(float dt)
{
    int frameAmount = 0;
    frameTimer_ += dt;
    if (frameTimer_ >= frameDuration_)
    {
        frameTimer_ = 0.0f;
        if (isRightFacing_)
        {
            currentFrameIndex_++;
            frameAmount = 0;
            if (currentAnimationType_ == AnimationType::MeleeAttackRight)
            {
                frameAmount = meleeAttackRightFrameAmount_;
            }
            else if (currentAnimationType_ == AnimationType::RangedAttackRight)
            {
                frameAmount = rangedAttackRightFrameAmount_;
            }
            else if (currentAnimationType_ == AnimationType::WalkRight)
            {
                frameAmount = walkRightFrameAmount_;
            }
            else if (currentAnimationType_ == AnimationType::IdleRight)
            {
                frameAmount = idleRightFrameAmount_;
            }
            else if (currentAnimationType_ == AnimationType::HurtRight)
            {
                frameAmount = hurtRightFrameAmount_;
            }
            else if (currentAnimationType_ == AnimationType::DeathRight)
            {
                frameDuration_ = 0.2f;
                frameAmount    = deathRightFrameAmount_;
            }
            if (currentFrameIndex_ >= frameAmount)
            {
                if (isHurtAnimation() || isMeleeAttackAnimation() || isRangedAttackAnimation())
                {
                    setCurrentAnimationType(AnimationType::IdleRight);
                }
                else if (isDeathAnimation())
                {
                    currentFrameIndex_ = frameAmount - 1;
                }
                else
                {
                    currentFrameIndex_ = 0;
                }
            }
        }
        else
        {
            if (currentAnimationType_ == AnimationType::MeleeAttackLeft)
            {
                frameAmount = meleeAttackLeftFrameAmount_;
            }
            else if (currentAnimationType_ == AnimationType::RangedAttackLeft)
            {
                frameAmount = rangedAttackLeftFrameAmount_;
            }
            else if (currentAnimationType_ == AnimationType::WalkLeft)
            {
                frameAmount = walkLeftFrameAmount_;
            }
            else if (currentAnimationType_ == AnimationType::IdleLeft)
            {
                frameAmount = idleLeftFrameAmount_;
            }
            else if (currentAnimationType_ == AnimationType::HurtLeft)
            {
                frameAmount = hurtLeftFrameAmount_;
            }
            else if (currentAnimationType_ == AnimationType::DeathLeft)
            {
                frameDuration_ = 0.2f;
                frameAmount    = deathLeftFrameAmount_;
            }

            currentFrameIndex_--;

            if (currentFrameIndex_ < 0)
            {
                if (isHurtAnimation() || isMeleeAttackAnimation() || isRangedAttackAnimation())
                {
                    setCurrentAnimationType(AnimationType::IdleLeft);
                }
                else if (isDeathAnimation())
                {
                    currentFrameIndex_ = 0;
                }
                else
                {
                    currentFrameIndex_ = frameAmount - 1;
                }
            }
        }
    }
}

void AnimationController::setToIdle(bool facingRight)
{
    if (facingRight)
        this->setCurrentAnimationType(AnimationType::IdleRight);
    else
        this->setCurrentAnimationType(AnimationType::IdleLeft);
}

void AnimationController::setToWalkOrIdle(float x, float y, bool facingRight)
{
    if (x != 0.0f || y != 0.0f)
    {
        if (facingRight)
            this->setCurrentAnimationType(AnimationType::WalkRight);
        else
            this->setCurrentAnimationType(AnimationType::WalkLeft);
    }
    else
    {
        this->setToIdle(facingRight);
    }
}

void AnimationController::setToHurt(bool facingRight)
{
    if (facingRight)
        this->setCurrentAnimationType(AnimationType::HurtRight);
    else
        this->setCurrentAnimationType(AnimationType::HurtLeft);
}

void AnimationController::setToDeath(bool facingRight)
{
    if (facingRight)
        this->setCurrentAnimationType(AnimationType::DeathRight);
    else
        this->setCurrentAnimationType(AnimationType::DeathLeft);
}

void AnimationController::setToMeleeAttack(bool facingRight)
{
    if (facingRight)
        this->setCurrentAnimationType(AnimationType::MeleeAttackRight);
    else
        this->setCurrentAnimationType(AnimationType::MeleeAttackLeft);
}

void AnimationController::setToRangedAttack(bool facingRight)
{
    if (facingRight)
        this->setCurrentAnimationType(AnimationType::RangedAttackRight);
    else
        this->setCurrentAnimationType(AnimationType::RangedAttackLeft);
}

void AnimationController::setCurrentAnimationType(AnimationType type)
{
    if (currentAnimationType_ == type)
    {
        return;
    }
    isRightFacing_        = (type == AnimationType::WalkRight || type == AnimationType::IdleRight ||
                      type == AnimationType::HurtRight || type == AnimationType::DeathRight ||
                      type == AnimationType::MeleeAttackRight || type == AnimationType::RangedAttackRight);
    currentAnimationType_ = type;
    if (isRightFacing_)
    {
        currentFrameIndex_ = 0;
    }
    else
    {
        if (type == AnimationType::WalkLeft)
        {
            currentFrameIndex_ = walkLeftFrameAmount_ - 1;
        }
        else if (type == AnimationType::IdleLeft)
        {
            currentFrameIndex_ = idleLeftFrameAmount_ - 1;
        }
        else if (type == AnimationType::HurtLeft)
        {
            currentFrameIndex_ = hurtLeftFrameAmount_ - 1;
        }
        else if (type == AnimationType::DeathLeft)
        {
            currentFrameIndex_ = deathLeftFrameAmount_ - 1;
        }
        else if (type == AnimationType::MeleeAttackLeft)
        {
            currentFrameIndex_ = meleeAttackLeftFrameAmount_ - 1;
        }
        else if (type == AnimationType::RangedAttackLeft)
        {
            currentFrameIndex_ = rangedAttackLeftFrameAmount_ - 1;
        }
    }
    frameTimer_ = 0.0f;
}