#pragma once
#include <string>
#include <unordered_map>

class Assets;

enum class AnimationType {
	WalkRight,
	WalkLeft,
	Idle
};

class AnimationController {
public:
	AnimationController(Assets* assets, const std::string walkRightPath, const int walkRightframeAmount, const std::string walkLeftPath, const int walkLeftframeAmount, const std::string idlePath, const int idlesframeAmount);
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

private:
	void setAnimationIconId(AnimationType type, int iconId) {
		animationIconIds_[type] = iconId;
	}
	int getAnimationIconId(AnimationType type) const {
		auto it = animationIconIds_.find(type);
		return (it != animationIconIds_.end()) ? it->second : -1;
	}


	Assets* assets_;
	int walkRightFrameAmount_;
	int walkLeftFrameAmount_;
	int idleFrameAmount_;

	AnimationType currentAnimationType_ = AnimationType::Idle;
	std::unordered_map<AnimationType, int> animationIconIds_;

	int currentFrameIndex_ = 0;
	float frameTimer_ = 0.0f;
	float frameDuration_ = 0.1f;

};