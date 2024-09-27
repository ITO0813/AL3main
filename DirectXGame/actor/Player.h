#pragma once
#include"AABB.h"
#include "Model.h"
#include "Vector3.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include"AABB.h"
#include "CameraController.h"


class MapChipField;
class Enemy;

class Player {
public:
	//左右
	enum class LRDirection {
		kRight,
		kLeft,
	};
	
	//角
	enum Corner {
		kRighBottom,
		kLeftBottom,
		kRighTop,
		kLeftTop,

		kNumCorner
	};

	///< summary>
	/// 初期化
	///</summary>

	void Initialize(Model*model_, ViewProjection* viewProjection,const Vector3& position);
	//void Initialize(const Vector3& position, ViewProjection* viewProjection);
	///< summary>
	/// 更新
	///</summary>
	void Update();

	///< summary>
	/// 描画
	///</summary>

	void Draw();

	//ワールド座標取得
	Vector3 GetWorldPosition();

	AABB GetAABB();

	void OnCollision(const Enemy* enemy);
	//const Vector3& targetVelocity=target_->GetVelocity();

	 void SetMapChipField(MapChipField*mapChipField){mapChipField_=mapChipField;}
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const Vector3& GetVelocity() const { return velocity_; }
	bool IsDead()const { return isDead_; }

private:
	static inline const float kAcceleration = 0.1f;
	static inline const float kAttenuation = 0.05f;
	// ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 20.0f;
	// 重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.98f;
	static inline const float kAttenuationWall = 0.2f;
	static inline const float  kAttenuationLanding = 0.0f;
	static inline const float kLimitFallSpeed = 0.5f;
	static inline const float kLimitRunSpeed = 0.5f;
	static inline const float kTimeTurn = 0.3f;
	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 0.04f;
	static inline const float kGroundSearchHeight = 0.06f;


	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool wall = false;
		Vector3 move;
	};
	// モデル
	Model* model_ = nullptr;
	// ワールド変換データ
	WorldTransform worldTransform_;
	ViewProjection* viewProjection_ = nullptr;
	Vector3 velocity_ = {};
	bool onGround_ = true;
	LRDirection lrDirection_ = LRDirection::kRight;

	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間<秒>
	static const float Liner;
	// 最大落下速度(下方向)

	MapChipField* mapChipField_ = nullptr;
	// テクスチャーハンドル
	uint32_t textureHandle_ = 0u;
	
	bool isDead_ = false;


	void InputMove();
	void CheckMapCollision(CollisionMapInfo&info);
	void CheckMapCollisionUP(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);
	void UpdateOnGround(const CollisionMapInfo& info);
	void AnimateTurn();

	 Vector3 CornerPosition(const Vector3& center,Corner corner);
};