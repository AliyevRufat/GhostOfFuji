#pragma once
#include <GameScene.h>
#include "CubePrefab.h"
#include "SpherePrefab.h"

class PongScene : public GameScene
{
public:
	PongScene();
	virtual ~PongScene();

	PongScene(const PongScene& other) = delete;
	PongScene(PongScene&& other) noexcept = delete;
	PongScene& operator=(const PongScene& other) = delete;
	PongScene& operator=(PongScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void ResetScene();
	//
	SpherePrefab* m_pBall = nullptr;
	CubePrefab* m_pRightPeddle = nullptr;
	CubePrefab* m_pLeftPeddle = nullptr;

	bool m_CanPlay = true;
	bool m_ResetBallForce = false;
};