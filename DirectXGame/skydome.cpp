#include"cassert"
#include"skydome.h"

void skydome::Initialize(Model* model, ViewProjection* viewProjection) {
//NULLポインタチェック
	assert(model);
	worldTransform_.Initialize();

	//引数の内容をメンバ変数に記録
	model_=model;
	//textureHandle_=textureHandle;
	viewProjection_= viewProjection;
}

void skydome::Update(){
}
void skydome::Draw(){

	model_->Draw(worldTransform_,*viewProjection_);

}