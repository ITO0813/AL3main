#include "Model.h"
#include "WorldTransform.h"
#include "viewProjection.h"


class skydome{
public:

void Initialize(Model* model,ViewProjection* viewProjection);



void Update();


void Draw();

private:
	//ワールド変換データ
	WorldTransform worldTransform_;
	//モデル
	Model* model_=nullptr;
	//テクスチャーハンドル
	uint32_t textureHandle_=0u;
	ViewProjection* viewProjection_=nullptr;
};