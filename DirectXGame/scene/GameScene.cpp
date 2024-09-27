#include "GameScene.h"
#include "TextureManager.h"
#include "ImGuiManager.h"
#include "AxisIndicator.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {

delete sprite_;
delete model_;
delete debugCamera_;

}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.png");

	//スプライトの生成
	sprite_ = Sprite::Create(textureHandle_,{100,50});

	model_ = Model::Create();
	worldTransform_.Initialize();
    viewProjection_.Initialize();

	soundDataHandle_ = audio_->LoadWave("mokugyo.wav");
	audio_->PlayWave(soundDataHandle_);

	voiceHandle_ = audio_->PlayWave(soundDataHandle_,true);

	debugCamera_ = new DebugCamera(1280, 720);

	
	AxisIndicator::GetInstance()->SetVisible(true);
	
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());

}

void GameScene::Update() {

	Vector2 position = sprite_->GetPosition();
	position.x += 2.0f;
	position.y += 1.0f;

	sprite_->SetPosition(position);

	if (input_->TriggerKey(DIK_SPACE)) {
		audio_->StopWave(voiceHandle_);
	}
	
#ifdef _DEBUG	
	ImGui::Begin("Debug1");

	ImGui::InputFloat3("inputFloat3", inputFloat3);

	ImGui::SliderFloat3("inputFloat3", inputFloat3, 0.0f, 1.0f);

	ImGui::Text("Kamata Tarou %d.%d.%d", 2050, 12, 31);

	ImGui::End();

	ImGui::ShowDemoWindow();
#endif

	debugCamera_->Update();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	model_->Draw(worldTransform_,viewProjection_,textureHandle_);
	model_->Draw(worldTransform_, debugCamera_->GetViewProjection(), textureHandle_);
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
    sprite_->Draw();
	// スプライト描画後処理
	Sprite::PostDraw();
	



#pragma endregion
}
