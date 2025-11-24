#include "AnimationController.h"
#include "Assets.h"

AnimationController::AnimationController(Assets* assets, const std::string walkRightPath, const int walkRightframeAmount, const std::string walkLeftPath, const int walkLeftframeAmount,  const std::string idlePath, const int idlesframeAmount)
    : assets_(assets), walkRightFrameAmount_(walkRightframeAmount),walkLeftFrameAmount_(walkLeftframeAmount), idleFrameAmount_(idlesframeAmount)
{
	// Load Walk animation sprite sheet (assumes horizontal strip of frames)
	// Each frame is assumed to be square (100x100 pixels)
	int walkRightId = assets_->loadSpriteSheet(walkRightPath, walkRightframeAmount, 100, 100);
	setAnimationIconId(AnimationType::WalkRight, walkRightId);
	
	int walkLeftId = assets_->loadSpriteSheet(walkLeftPath, walkLeftframeAmount, 100, 100);
	setAnimationIconId(AnimationType::WalkLeft, walkLeftId);
	// Load Idle animation sprite sheet
	int idleId = assets_->loadSpriteSheet(idlePath, idlesframeAmount, 100, 100);
	setAnimationIconId(AnimationType::Idle, idleId);
}

void AnimationController::update(float dt) {
	frameTimer_ += dt;
	if (frameTimer_ >= frameDuration_) {
		frameTimer_ = 0.0f;
		currentFrameIndex_++;
		int frameAmount = 0;
		if (currentAnimationType_ == AnimationType::WalkRight) {
			frameAmount = walkRightFrameAmount_;
		}
		else if (currentAnimationType_ == AnimationType::WalkLeft) {
			frameAmount = walkLeftFrameAmount_;
		}
		else if (currentAnimationType_ == AnimationType::Idle) {
			frameAmount = idleFrameAmount_;
		}
		if (currentFrameIndex_ >= frameAmount) {
			currentFrameIndex_ = 0;
		}
	}
}
