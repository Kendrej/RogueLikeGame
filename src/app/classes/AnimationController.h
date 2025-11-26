#pragma once
#include <string>
#include <unordered_map>

class Assets;

enum class AnimationType {
	WalkRight,
	WalkLeft,
	IdleRight,
	IdleLeft,
	HurtRight,
	HurtLeft
};

class AnimationController {
public:
	AnimationController(Assets* assets,const int squareSize, const std::string walkRightPath, const int walkRightframeAmount,
						const std::string walkLeftPath, const int walkLeftframeAmount,
						const std::string idleRightPath, const int idlesRightframeAmount,
						const std::string idleLeftPath, const int idlesLeftframeAmount,
						const std::string hurtRightPath, const int hurtRightframeAmount,
						const std::string hurtLeftPath, const int hurtLeftframeAmount);
	~AnimationController() = default;

	void setCurrentAnimationType(AnimationType type) {
		if (currentAnimationType_ == type) {
			return;  // Ju¿ w tym stanie, nie resetuj!
		}
		currentAnimationType_ = type;
		currentFrameIndex_ = 0;
		frameTimer_ = 0.0f;
	}
	void update(float dt);
	
	int getCurrentFrameIconId() const {
		int baseIconId = getAnimationIconId(currentAnimationType_);
		return baseIconId + currentFrameIndex_;
	}
	AnimationType getCurrentAnimationType() const {
		return currentAnimationType_;
	}

private:
	void addWalkAnimation(Assets* assets, const int squareSize, const std::string walkRightPath, const std::string walkLeftPath);
	void addIdleAnimation(Assets* assets, const int squareSize, const std::string idleRightPath, const std::string idleLeftPath);
	void addHurtAnimation(Assets* assets, const int squareSize, const std::string hurtRightPath, const std::string hurtLeftPath);

	void setAnimationIconId(AnimationType type, int iconId) {
		animationIconIds_[type] = iconId;
	}
	int getAnimationIconId(AnimationType type) const {
		auto it = animationIconIds_.find(type);
		return (it != animationIconIds_.end()) ? it->second : -1;
	}
	bool isHurtAnimation() const {
		return currentAnimationType_ == AnimationType::HurtRight || currentAnimationType_ == AnimationType::HurtLeft;
	}


	Assets* assets_;
	int walkRightFrameAmount_;
	int walkLeftFrameAmount_;
	int idleRightFrameAmount_;
	int idleLeftFrameAmount_;
	int hurtRightFrameAmount_;
	int hurtLeftFrameAmount_;

	AnimationType currentAnimationType_ = AnimationType::IdleRight;
	std::unordered_map<AnimationType, int> animationIconIds_;

	int currentFrameIndex_ = 0;
	float frameTimer_ = 0.0f;
	float frameDuration_ = 0.1f;
};