#pragma once
#define NOMINMAX
#include "Player.h"
#include"Input.h"
#include "Input.h"
#include"MapChipField.h"
#include "MathUtilityForText.h"
#include"DebugText.h"
#include <algorithm>
#include <cassert>
#include <numbers>

void Player::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
	// NULLポインタチェック
	//	assert(model);
	model_= model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	viewProjection_ = viewProjection;
	// 引数の内容をメンバ変数に記録
	// 
	// textureHandle_=textureHandle;
}

void Player::Update() {

	InputMove();

	CollisionMapInfo collisionMapInfo = {};
	collisionMapInfo.move = velocity_;
	collisionMapInfo.landing = false;
	collisionMapInfo.wall = false;
	//衝突チェック
	CheckMapCollision(collisionMapInfo);
	//移動
	worldTransform_.translation_ += collisionMapInfo.move;

	if (collisionMapInfo.ceiling) {
		velocity_.y = 0;
	}
	if (collisionMapInfo.wall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
	//接地判定
	UpdateOnGround(collisionMapInfo);
	//旋回判定
	AnimateTurn();
	// 行列計算
	worldTransform_.UpdateMatrix();
};
void Player::Draw() {
	{ model_->Draw(worldTransform_, *viewProjection_); }
};
Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = { worldPos.x - kWidth / 2.0f,worldPos.y - kHeight / 2.0f,worldPos.z - kWidth / 2.0f };

	aabb.max = { worldPos.x + kWidth / 2.0f,worldPos.y + kHeight / 2.0f,worldPos.z - kWidth / 2.0f };

	return aabb;
}

void Player::OnCollision(const Enemy* enemy) {
	(void)enemy;
	isDead_ = true;
	//velocity_ += Vector3(0, kJumpAcceleration / 60.0f, 0);
}

void Player::InputMove() {


	// 移動入力
	if (onGround_) {
		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

			// 左右加速
			Vector3 acceleration = {};

			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration/60.0f;

				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;

					// 旋回開始時の角度
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					// 旋回タイマー

					turnTimer_ = kTimeTurn;
				}
			}
			else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				// 右移動中の左入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x -= kAcceleration / 60.0f;

				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					// 旋回開始時の角度
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					// 旋回タイマー
					turnTimer_ = kTimeTurn;
				}

			}
			// 加速/減速
			velocity_+= acceleration;
			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
			
			
		}
		else {
			// 非入力時は移動減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}
		//ほぼ0にする場合
		if (std::abs(velocity_.x) <= 0.0001f) {
			velocity_.x = 0.0f;
		}

		if (Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ初速
			velocity_ += Vector3(0, kJumpAcceleration/60.0f, 0);
			/*velocity_.x += 0;
			velocity_.y += kJumpAcceleration;
			velocity_.z += 0;*/
		}

	}
	else {
		// 落下速度
		velocity_ += Vector3(0, -kGravityAcceleration / 60.0f, 0);
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

}
	void Player::CheckMapCollision(CollisionMapInfo & info) {
		CheckMapCollisionUP(info);
		CheckMapCollisionDown(info);
		CheckMapCollisionRight(info);
		CheckMapCollisionLeft(info);
	}
	void Player::CheckMapCollisionUP(CollisionMapInfo& info) {
		if (info.move.y <= 0) {
			return;
		}
		std::array<Vector3, kNumCorner>positionsNew;
		for (uint32_t i = 0; i < positionsNew.size(); ++i) {
			positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
		}

		MapChipType mapChipType;
		MapChipType mapChipTypeNext;
		//真上の当たり判定
		bool hit = false;
		//左上点
		MapChipField::IndexSet indexSet;
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);

		if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
			hit = true;
		}
		//右上点
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRighTop]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);

		if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
			hit = true;
		}

		if (hit) {
			//現在座標が壁の外か判定
			MapChipField::IndexSet  indexSetNow;
			indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
			if (indexSetNow.yIndex != indexSet.yIndex) {
				indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, +kHeight / 2.0f, 0));
				MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
				info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
				info.ceiling = true;
			}
		}

	}

	void Player::CheckMapCollisionDown(CollisionMapInfo& info) {
		// 下降あり？
		if (info.move.y >= 0) {
			return;
		}

		std::array<Vector3, kNumCorner> positionsNew;

		for (uint32_t i = 0; i < positionsNew.size(); ++i) {
			positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
		}

		MapChipType mapChipType;
		MapChipType mapChipTypeNext;
		bool hit = false;
		// 真下の当たり判定を行う
		// 左下点の判定
		MapChipField::IndexSet indexSet;
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
		if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
			hit = true;
		}
		// 右下点の判定
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRighBottom]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
		if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
			hit = true;
		}

		// ブロックにヒット？
		if (hit) {
			// 現在座標が壁の外か判定
			MapChipField::IndexSet indexSetNow;
			indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, -kHeight / 2.0f, 0));
			if (indexSetNow.yIndex != indexSet.yIndex) {
				// めり込みを排除する方向に移動量を設定する
				indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, -kHeight / 2.0f, 0));
				MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
				info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
				info.landing = true;
			}
		}
	}


	void Player::CheckMapCollisionRight(CollisionMapInfo& info) {
		if (info.move.x <= 0) {
			return;
		}
		std::array<Vector3, kNumCorner>positionsNew;

		for (uint32_t i = 0; i < positionsNew.size(); ++i) {
			positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
		}
		MapChipType mapChipType;
		MapChipType mapChipTypeNext;
		//真上の当たり判定
		bool hit = false;

		//右上点
		MapChipField::IndexSet indexSet;
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRighTop]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex-1, indexSet.yIndex);

		if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
			hit = true;
		}
		//右下点
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRighBottom]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex-1, indexSet.yIndex);
		if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
			hit = true;
		}

		if (hit) {
			//現在座標が壁の外か判定
			MapChipField::IndexSet  indexSetNow;
			indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3( +kWidth / 2.0f, 0,0));
			if (indexSetNow.xIndex != indexSet.xIndex) {
				indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_+ info.move + Vector3( +kWidth / 2.0f, 0,0));
				MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
				info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
				info.wall = true;
			}
		}
	}

	void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
		if (info.move.x >= 0) {
			return;
		}
		std::array<Vector3, kNumCorner>positionsNew;

		for (uint32_t i = 0; i < positionsNew.size(); ++i) {
			positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
		}
		MapChipType mapChipType;
		MapChipType mapChipTypeNext;
		//真上の当たり判定
		bool hit = false;

		//左上点
		MapChipField::IndexSet indexSet;
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex+1, indexSet.yIndex);

		if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
			hit = true;
		}
		//右上点
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex+1, indexSet.yIndex);
		if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
			hit = true;
		}


		if (hit) {
			//現在座標が壁の外か判定
			MapChipField::IndexSet  indexSetNow;
			indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3( -kWidth / 2.0f, 0,0));
			if (indexSetNow.xIndex != indexSet.xIndex) {
				indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0,0));
				MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
				info.move.x = std::min(0.0f, rect.right - worldTransform_.translation_.x + (kWidth / 2.0f + kBlank));
				info.wall = true;
			}
		}
	}

	void Player::UpdateOnGround(const CollisionMapInfo& info) {
		if (onGround_) {
			//ジャンプ可能
			if (velocity_.y > 0.0f) {
				onGround_ = false;
			}
			else {
				std::array<Vector3, kNumCorner>positionNew;
				for (uint32_t i = 0; i < positionNew.size(); i++) {
					positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
				}
				bool ground = false;

				MapChipType mapChipType;
				//左側の判定
				MapChipField::IndexSet indexSet;
				indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom] + Vector3(0, -kGroundSearchHeight, 0));
				mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
				if (mapChipType == MapChipType::kBlock) {
					ground = true;
				}
				indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRighBottom] + Vector3(0, -kGroundSearchHeight, 0));
				mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
				if (mapChipType == MapChipType::kBlock) {
					ground = true;
				}
				if (!ground) {
					DebugText::GetInstance()->ConsolePrintf("jump");
					onGround_ = false;
				}
			}
		}
		else {
			if (info.landing) {
				velocity_.x *= (1.0f - kAttenuationLanding);
				velocity_.y = 0.0f;

				DebugText::GetInstance()->ConsolePrintf("onGround");
				onGround_ = true;
			}
		}

	}


	void Player::AnimateTurn() {
		if (turnTimer_ > 0.0f) {
			turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);
			float destinationRotationYTable[] = { std::numbers::pi_v<float> / 2.0f,std::numbers::pi_v<float>*3.0f / 2.0f };
			float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
			worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
		}
	}
		/*velocity_.x += 0;
		velocity_.y += -kGravityAcceleration;
		velocity_.z += 0;*/

		
		// 地面との当たり判定
	//	// 下降中？
	//	if (velocity_.y < 0) {
	//		// Y座標が地面以下になったら着地
	//		if (worldTransform_.translation_.y <= 2.0f) {
	//			landing_ = true;
	//		}
	//	}

	//// 移動
	//worldTransform_.translation_.x += velocity_.x;
	//worldTransform_.translation_.y += velocity_.y;
	//worldTransform_.translation_.z += velocity_.z;

	//// 接地判定
	//if (onGround_) {
	//	// ジャンプ開始
	//	if (velocity_.y > 0.0f) {
	//		// 空中状態に移行
	//		onGround_ = false;
	//	}
	//}
	//else {
	//	// 着地
	//	if (landing_) {
	//		// めり込み排斥
	//		worldTransform_.translation_.y = 2.0f;
	//		// 摩擦で横方向速度が減衰する
	//		velocity_.x *= (1.0f - kAttenuation);
	//		// 下方向速度をリセット
	//		velocity_.y = 0.0f;
	//		// 接地状態に移行
	//		onGround_ = true;
	//	}
	//}

	//// 行列を定数バッファに転送
	//worldTransform_.TransferMatrix();


Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[] = {
		{+kWidth / 2.0f,-kHeight / 2.0f,0},
		{-kWidth / 2.0f,-kHeight / 2.0f,0},
		{+kWidth / 2.0f,+kHeight / 2.0f,0},
		{-kWidth / 2.0f,+kHeight / 2.0f,0}
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}