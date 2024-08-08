#pragma once
#include<ViewProjection.h>
#include "Player.h"

class Player;

class CameraController {
public:

	struct Rect {
		float left = 0.0f;
		float right = 0.0f;
		float bottom = 0.0f;
		float top = 0.0f;
	};

	void Initialize();

	void Update();

	void Reset();
	void SetTarget(Player* target) { target_ = target; }
	void SetMovableArea(const Rect& area) { movablearea_ = area; }

	const ViewProjection& GetViewProjection() const { return viewProjection_; }

private:

	ViewProjection viewProjection_;
	Player* target_ = nullptr;
	Rect movablearea_ = { 0,100,0,100 };
	Vector3 destination_;
	Vector3 targetOffset_ = { 0,0,-15.0f };
	static inline const Rect targetmargin = { -9.0f,9.0f,-5.0f,5.0f };
	static inline const float kInterpolationRate = 0.1f;
	static inline const float kVelocityBias = 30.0f;
	//void Viewprojection();
};