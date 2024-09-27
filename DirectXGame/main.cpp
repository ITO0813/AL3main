#include "Audio.h"
#include "DirectXCommon.h"
#include "GameScene.h"
#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include"TitleScene.h"
#include "WinApp.h"


	GameScene* gameScene = nullptr;
	TitleScene* titleScene = nullptr;

	enum class Scene {

		kUnknown = 0,

		kTitle,
		kGame,
	};

	Scene scene = Scene::kTitle;

	void ChangScene() {
		switch (scene)
		{
		case Scene::kTitle:
			if (titleScene->IsFinished()) {
				//シーン変更
				scene = Scene::kGame;
				//旧シーンの解放
				delete titleScene;
				titleScene = nullptr;

				gameScene = new GameScene;
				gameScene->Initialize();

			}
			break;
		case Scene::kGame:
			if (gameScene->IsFinished()) {
				//シーン変更
				scene = Scene::kTitle;
				//旧シーンの解放
				delete gameScene;
				gameScene = nullptr;

				titleScene = new TitleScene;
				titleScene->Initialize();

			}
			break;
		}


	}

	void UpdateScene() {
		switch (scene) {
		case Scene::kTitle:
			titleScene->Update();
			break;
		case Scene::kGame:
			gameScene->Update();
			break;

		}
	}

	void DrawScene() {
		switch (scene) {
		case Scene::kTitle:
			titleScene->Draw();
			break;
		case Scene::kGame:
			gameScene->Draw();
			break;

		}

	}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	// 汎用機能
	Input* input = nullptr;
	Audio* audio = nullptr;
	PrimitiveDrawer* primitiveDrawer = nullptr;


	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow();

	

	// DirectX初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

#pragma region 汎用機能初期化
	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	// 入力の初期化
	input = Input::GetInstance();
	input->Initialize();

	// オーディオの初期化
	audio = Audio::GetInstance();
	audio->Initialize();

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice());
	TextureManager::Load("white1x1.png");

	// スプライト静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 3Dモデル静的初期化
	Model::StaticInitialize();

	primitiveDrawer = PrimitiveDrawer::GetInstance();
	primitiveDrawer->Initialize();
#pragma endregion

	// ゲームシーンの初期化
	scene = Scene::kTitle;
	//scene = Scene::kTitle;
	titleScene = new TitleScene;
	titleScene->Initialize();
	gameScene = new GameScene();
	gameScene->Initialize();

	// メインループ
	while (true) {
		// メッセージ処理
		if (win->ProcessMessage()) {
			break;
		}

		// ImGui受付開始
		imguiManager->Begin();
		// 入力関連の毎フレーム処理
		input->Update();

//		titleScene->Update();
		

		//シーン切り替え
		ChangScene();
		////現在シーン更新
		UpdateScene();
		
		// ゲームシーンの毎フレーム処理
// 		gameScene->Update();
		
		// ImGui受付終了
		imguiManager->End();

		// 描画開始
		dxCommon->PreDraw();

		//titleScene->Draw();
		//////現在シーンの描画
		DrawScene();

		// ゲームシーンの描画
		//gameScene->Draw();
		// プリミティブ描画のリセット
		primitiveDrawer->Reset();
		// ImGui描画
		imguiManager->Draw();
		// 描画終了
		dxCommon->PostDraw();
	}

	// 各種解放
	delete titleScene;
	delete gameScene;
	// 3Dモデル解放
	Model::StaticFinalize();
	audio->Finalize();
	// ImGui解放
	imguiManager->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	return 0;
}