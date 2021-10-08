#pragma once
#include "GameScene.h"
#include "Character.h"

class PlayerControllerScene final : public GameScene
{
public:
	PlayerControllerScene();
	virtual ~PlayerControllerScene() = default;

	PlayerControllerScene(const PlayerControllerScene& other) = delete;
	PlayerControllerScene(PlayerControllerScene&& other) noexcept = delete;
	PlayerControllerScene& operator=(const PlayerControllerScene& other) = delete;
	PlayerControllerScene& operator=(PlayerControllerScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};