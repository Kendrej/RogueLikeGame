#include "AnimationController.h"
#include "Assets.h"

AnimationController::AnimationController(Assets* assets,const int squareSize, const std::string walkRightPath, const int walkRightframeAmount,
										const std::string walkLeftPath, const int walkLeftframeAmount,
										const std::string idleRightPath, const int idlesRightframeAmount,
										const std::string idleLeftPath, const int idlesLeftframeAmount,
										const std::string hurtRightPath, const int hurtRightframeAmount,
										const std::string hurtLeftPath, const int hurtLeftframeAmount)
	: assets_(assets), walkRightFrameAmount_(walkRightframeAmount), walkLeftFrameAmount_(walkLeftframeAmount), idleRightFrameAmount_(idlesRightframeAmount), idleLeftFrameAmount_(idlesLeftframeAmount),
		hurtRightFrameAmount_(hurtRightframeAmount), hurtLeftFrameAmount_(hurtLeftframeAmount)
{
	addWalkAnimation(assets, squareSize, walkRightPath, walkLeftPath);
	
	addIdleAnimation(assets, squareSize, idleRightPath, idleLeftPath);	

	addHurtAnimation(assets, squareSize, hurtRightPath, hurtLeftPath);
}

void AnimationController::addWalkAnimation(Assets* assets, const int squareSize, const std::string walkRightPath, const std::string walkLeftPath) {
	int walkRightId = assets_->loadSpriteSheet(walkRightPath, walkRightFrameAmount_, squareSize, squareSize);
	setAnimationIconId(AnimationType::WalkRight, walkRightId);

	int walkLeftId = assets_->loadSpriteSheet(walkLeftPath, walkLeftFrameAmount_, squareSize, squareSize);
	setAnimationIconId(AnimationType::WalkLeft, walkLeftId);
}

void AnimationController::addIdleAnimation(Assets* assets, const int squareSize, const std::string idleRightPath, const std::string idleLeftPath) {
	int idleRightId = assets_->loadSpriteSheet(idleRightPath, idleRightFrameAmount_, squareSize, squareSize);
	setAnimationIconId(AnimationType::IdleRight, idleRightId);

	int idleLeftId = assets_->loadSpriteSheet(idleLeftPath, idleLeftFrameAmount_, squareSize, squareSize);
	setAnimationIconId(AnimationType::IdleLeft, idleLeftId);
}

void AnimationController::addHurtAnimation(Assets* assets, const int squareSize, const std::string hurtRightPath, const std::string hurtLeftPath) {
	int hurtRightId = assets_->loadSpriteSheet(hurtRightPath, hurtRightFrameAmount_, squareSize, squareSize);
	setAnimationIconId(AnimationType::HurtRight, hurtRightId);

	int hurtLeftId = assets_->loadSpriteSheet(hurtLeftPath, hurtLeftFrameAmount_, squareSize, squareSize);
	setAnimationIconId(AnimationType::HurtLeft, hurtLeftId);
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
		else if (currentAnimationType_ == AnimationType::HurtRight) {
			frameAmount = hurtRightFrameAmount_;
		}
		else if (currentAnimationType_ == AnimationType::HurtLeft) {
			frameAmount = hurtLeftFrameAmount_;
		}
		
		if (currentFrameIndex_ >= frameAmount) {
			if (isHurtAnimation()) {
				if(currentAnimationType_ == AnimationType::HurtRight) {
					setCurrentAnimationType(AnimationType::IdleRight);
				}
				else {
					setCurrentAnimationType(AnimationType::IdleLeft);
				}
			}
			else {
				currentFrameIndex_ = 0;
			}
		}
	}
}
