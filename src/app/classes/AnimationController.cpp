#include "AnimationController.h"
#include "Assets.h"

AnimationController::AnimationController(Assets* assets,const int squareSize, const std::string walkRightPath, const int walkRightframeAmount,
										const std::string walkLeftPath, const int walkLeftframeAmount,
										const std::string idleRightPath, const int idlesRightframeAmount,
										const std::string idleLeftPath, const int idlesLeftframeAmount)
    : assets_(assets), walkRightFrameAmount_(walkRightframeAmount),walkLeftFrameAmount_(walkLeftframeAmount), idleRightFrameAmount_(idlesRightframeAmount), idleLeftFrameAmount_(idlesLeftframeAmount)
{
	// Load Walk animation sprite sheet (assumes horizontal strip of frames)
	// Each frame is assumed to be square (100x100 pixels)
	int walkRightId = assets_->loadSpriteSheet(walkRightPath, walkRightframeAmount, squareSize, squareSize);
	setAnimationIconId(AnimationType::WalkRight, walkRightId);
	
	int walkLeftId = assets_->loadSpriteSheet(walkLeftPath, walkLeftframeAmount, squareSize, squareSize);
	setAnimationIconId(AnimationType::WalkLeft, walkLeftId);
	// Load Idle animation sprite sheet
	int idleRightId = assets_->loadSpriteSheet(idleRightPath, idlesRightframeAmount, squareSize, squareSize);
	setAnimationIconId(AnimationType::IdleRight, idleRightId);

	int idleLeftId = assets_->loadSpriteSheet(idleLeftPath, idlesLeftframeAmount, squareSize, squareSize);
	setAnimationIconId(AnimationType::IdleLeft, idleLeftId);
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
		else if (currentAnimationType_ == AnimationType::IdleRight) {
			frameAmount = idleRightFrameAmount_;
		}
		else if( currentAnimationType_ == AnimationType::IdleLeft) {
			frameAmount = idleLeftFrameAmount_;
		}
		if (currentFrameIndex_ >= frameAmount) {
			currentFrameIndex_ = 0;
		}
	}
}
