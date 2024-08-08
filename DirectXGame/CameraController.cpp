#include "CameraController.h"
#include "Mymath.h"
#include "Player.h"
#include <algorithm>

void CameraController::Initialize()
{
	viewProjection_.Initialize();
}

void CameraController::Update()
{
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	const Vector3& targetVelocity = target_->GetVelocity();
	destination_ = targetWorldTransform.translation_ + targetVelocity * kVelocityBias + targetOffset_;
	viewProjection_.translation_ = Leap(viewProjection_.translation_, destination_, kInterpolationRate);
	viewProjection_.translation_.x = std::clamp(viewProjection_.translation_.x, targetWorldTransform.translation_.x + targetmargin.left, targetWorldTransform.translation_.x + targetmargin.right);
	viewProjection_.translation_.y = std::clamp(viewProjection_.translation_.y, targetWorldTransform.translation_.y + targetmargin.bottom, targetWorldTransform.translation_.y + targetmargin.top);
	viewProjection_.translation_.x = std::clamp(viewProjection_.translation_.x, movablearea_.left, movablearea_.right);
	viewProjection_.translation_.y = std::clamp(viewProjection_.translation_.y, movablearea_.bottom, movablearea_.top);

	viewProjection_.UpdateMatrix();
}

void CameraController::Reset()
{
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	viewProjection_.translation_ = targetWorldTransform.translation_ + targetOffset_;
}

