#pragma once
#include <string>
#include <unordered_map>

class Assets;

enum class AnimationType
{
    WalkRight,
    WalkLeft,
    IdleRight,
    IdleLeft,
    HurtRight,
    HurtLeft,
    DeathRight,
    DeathLeft,
    MeleeAttackRight,
    MeleeAttackLeft,
    RangedAttackRight,
    RangedAttackLeft
};

class AnimationController
{
public:
    AnimationController(Assets* assets, const int squareSize, const std::string walkRightPath,
                        const int walkRightframeAmount, const std::string walkLeftPath, const int walkLeftframeAmount,
                        const std::string idleRightPath, const int idlesRightframeAmount,
                        const std::string idleLeftPath, const int idlesLeftframeAmount, const std::string hurtRightPath,
                        const int hurtRightframeAmount, const std::string hurtLeftPath, const int hurtLeftframeAmount,
                        const std::string deathRightPath, const int deathRightframeAmount,
                        const std::string deathLeftPath, const int deathLeftframeAmount);
    ~AnimationController() = default;

    void addMeleeAttackAnimation(const int squareSize, const int meleeAttackFrame,
                                 const std::string meleeAttackRightPath, const int meleeAttackRightFrameAmount,
                                 const std::string meleeAttackLeftPath, const int meleeAttackLeftFrameAmount);
    void addRangedAttackAnimation(const int squareSize, const int rangedAttackFrame,
                                  const std::string rangedAttackRightPath, const int rangedAttackRightFrameAmount,
                                  const std::string rangedAttackLeftPath, const int rangedAttackLeftFrameAmount);

    void setCurrentAnimationType(AnimationType type);

    void update(float dt);

    int getCurrentFrameIconId() const
    {
        int baseIconId = getAnimationIconId(currentAnimationType_);
        return baseIconId + currentFrameIndex_;
    }
    AnimationType getCurrentAnimationType() const
    {
        return currentAnimationType_;
    }
    bool isHurtAnimation() const
    {
        return currentAnimationType_ == AnimationType::HurtRight || currentAnimationType_ == AnimationType::HurtLeft;
    }
    bool isMeleeAttackAnimation() const
    {
        return currentAnimationType_ == AnimationType::MeleeAttackRight ||
               currentAnimationType_ == AnimationType::MeleeAttackLeft;
    }
    bool isRangedAttackAnimation() const
    {
        return currentAnimationType_ == AnimationType::RangedAttackRight ||
               currentAnimationType_ == AnimationType::RangedAttackLeft;
    }
    bool isFacingRight() const
    {
        return isRightFacing_;
    }
    bool isInAttackFrame() const
    {
        if (isRightFacing_)
        {
            return (currentFrameIndex_ == meleeAttackFrame_ - 1);
        }
        else
        {
            return (currentFrameIndex_ == meleeAttackLeftFrameAmount_ - meleeAttackFrame_);
        }
    }
    bool isInRangedAttackFrame() const
    {
        if (isRightFacing_)
        {
            return (currentFrameIndex_ == rangedAttackFrame_ - 1);
        }
        else
        {
            return (currentFrameIndex_ == rangedAttackLeftFrameAmount_ - rangedAttackFrame_);
        }
    }
    void setToIdle(bool facingRight);
    void setToWalkOrIdle(float x, float y, bool facingRight);
    void setToHurt(bool facingRight);
    void setToDeath(bool facingRight);
    void setToMeleeAttack(bool facingRight);
    void setToRangedAttack(bool facingRight);

private:
    void addWalkAnimation(const int squareSize, const std::string walkRightPath, const std::string walkLeftPath);
    void addIdleAnimation(const int squareSize, const std::string idleRightPath, const std::string idleLeftPath);
    void addHurtAnimation(const int squareSize, const std::string hurtRightPath, const std::string hurtLeftPath);
    void adddeathAnimation(const int squareSize, const std::string deathRightPath, const std::string deathLeftPath);

    void setAnimationIconId(AnimationType type, int iconId)
    {
        animationIconIds_[type] = iconId;
    }
    int getAnimationIconId(AnimationType type) const
    {
        auto it = animationIconIds_.find(type);
        return (it != animationIconIds_.end()) ? it->second : -1;
    }

    bool isDeathAnimation() const
    {
        return currentAnimationType_ == AnimationType::DeathRight || currentAnimationType_ == AnimationType::DeathLeft;
    }

    bool isDeathAnimationFinished() const;

    Assets* assets_;
    int     walkRightFrameAmount_;
    int     walkLeftFrameAmount_;
    int     idleRightFrameAmount_;
    int     idleLeftFrameAmount_;
    int     hurtRightFrameAmount_;
    int     hurtLeftFrameAmount_;
    int     deathRightFrameAmount_;
    int     deathLeftFrameAmount_;

    int  meleeAttackRightFrameAmount_;
    int  meleeAttackLeftFrameAmount_;
    int  meleeAttackFrame_;
    int  rangedAttackRightFrameAmount_;
    int  rangedAttackLeftFrameAmount_;
    int  rangedAttackFrame_;

    bool isRightFacing_ = true;

    AnimationType                          currentAnimationType_ = AnimationType::IdleRight;
    std::unordered_map<AnimationType, int> animationIconIds_;

    int   currentFrameIndex_ = 0;
    float frameTimer_        = 0.0f;
    float frameDuration_     = 0.1f;
};