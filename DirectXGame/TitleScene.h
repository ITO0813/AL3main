#pragma once

#include "Model.h"
#include"ViewProjection.h"
#include"WorldTransform.h"

class TitleScene {
public:
 
	~TitleScene();

	void Initialize();

	void Update();

	void Draw();
	//デスフラグのgetter
	bool IsFinished() const { return finished_; }

private:
	static inline const float kTimeTitleMove = 2.0f;

	ViewProjection viewProjection_;
	WorldTransform worldTransformTitle_;
	WorldTransform worldTransformPlayer_;

	Model* modelPlayer_ = nullptr;
	Model* modelTitle_ = nullptr;

	float counter_=0.0f;

	//終了フラグ
	bool finished_ = false;
};