#pragma once
#define NOMINMAX
#include"DeathParticles.h"
#include"MathUtilityForText.h"
#include<cassert>
#include <algorithm>

void DeathParticles::Initialize(Model*model, ViewProjection* viewProjection, const Vector3& position) {

	assert(model);

	model_ = model;
	for (auto& worldTransform : worldTransforms_) {
		worldTransform. Initialize();
		worldTransform.translation_ = position;
	}
	viewProjection_ = viewProjection;
	objectColor_.Initialize();
	color_ = { 1,1,1,1 };
}

void DeathParticles::Update() {
	counter_ += 1.0f / 60.0f;
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		isFinished_ = true;
	}


	for (uint32_t i = 0; i < worldTransforms_.size(); ++i) {
		Vector3 velocity = { kSpeed,0,0 };
		float angle = kAngleUnit * i;
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);
		velocity = Transform(velocity, matrixRotation);
		worldTransforms_[i].translation_ += velocity;
	}
	for (auto& worldTransform : worldTransforms_) {
		worldTransform.UpdateMatrix();
	}

	color_.w = std::max(0.0f, 1.0f - counter_ / kDuration);
	//色変更オブジェクトに色の数値を設定する
	objectColor_.SetColor(color_);
	//色変更オブジェクトをVRAMに転送
	objectColor_.TransferMatrix();
}

void DeathParticles::Draw() {
	//終了なら何もしない
	if (isFinished_) {
		return;
	}
	for (auto& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform, *viewProjection_, &objectColor_);
	}
}