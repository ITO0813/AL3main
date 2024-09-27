#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include"Enemy.h"
#include "Input.h"
#include "MapChipField.h"
#include "Model.h"
#include "Player.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "skydome.h"
#include "CameraController.h"
#include "DeathParticles.h"
#include<list>
#include <DebugCamera.h>
#include<vector>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void GenerateBlocks();

	void CheckAllCollisions();

	void ChangePhase();


	void UpdateCamera();

	void UpdateBlocks();

	//デスフラグ
	bool isDead_ = false;

	//デスフラグのgetter
	bool IsDead()const { return isDead_; }

	//終了フラグ
	//デスフラグのgetter
	bool IsFinished() const { return finished_; }
private: // メンバ変数

	enum class Phase {
		kPlay,
		kDeath,

	};

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	/// テクスチャーハンドル
	uint32_t textureHandle_ = 0;


	// 自キャラ
	Player* player_ = nullptr;
	// 3Dモデル
	Model* model_ = nullptr;
	Model* modelPlayer_ = nullptr;
	Model* modelBlock_ = nullptr;
	skydome* Skydome_ = nullptr;
	Model* modelEnemy_ = nullptr;
	Model* modelDeathParticles_ = nullptr;
	// ワールドトランスフォーム
	WorldTransform wordTransform_;
	// ビュープロジェクション
	ViewProjection viewProjection_;
	// 縦横ブロックの配列
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
	WorldTransform worldTransformSkydome_;
	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;
	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;


	
	Model* modelSkydome_=nullptr;
	// マップチップフィールド
	MapChipField* mapChipField_=nullptr;
	CameraController* cameraController = nullptr;

	//Enemy* enemy_ = nullptr;
	

	std::list<Enemy*>enemies_;

	DeathParticles* deathParticles_ = nullptr;
	
	Phase phase_;
	bool finished_ = false;
};