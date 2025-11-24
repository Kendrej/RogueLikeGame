#include "AnimationController.h"
#include "Assets.h"

AnimationController::AnimationController(Assets* assets, const std::string walkPath, const int walkframeAmount, const std::string idlePath, const int idlesframeAmount) 
    : assets_(assets), walkFrameAmount_(walkframeAmount), idleFrameAmount_(idlesframeAmount)
{
	// Load Walk animation sprite sheet (assumes horizontal strip of frames)
	// Each frame is assumed to be square (100x100 pixels)
	int walkId = assets_->loadSpriteSheet(walkPath, walkframeAmount, 100, 100);
	setAnimationIconId(AnimationType::Walk, walkId);
	
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
		if (currentAnimationType_ == AnimationType::Walk) {
			frameAmount = walkFrameAmount_;
		}
		else if (currentAnimationType_ == AnimationType::Idle) {
			frameAmount = idleFrameAmount_;
		}
		if (currentFrameIndex_ >= frameAmount) {
			currentFrameIndex_ = 0;
		}
	}
}
