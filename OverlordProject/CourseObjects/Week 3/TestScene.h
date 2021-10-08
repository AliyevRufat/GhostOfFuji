#pragma once
#include <GameScene.h>
class TestScene : public GameScene
{
public:
	TestScene();
	virtual ~TestScene() = default;

	TestScene(const TestScene& other) = delete;
	TestScene(TestScene&& other) noexcept = delete;
	TestScene& operator=(const TestScene& other) = delete;
	TestScene& operator=(TestScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};
