#pragma once
#pragma once
#include "GameScene.h"
#include <vector>

class TextureData;
class SpriteFont;

class ExamGameMainMenuScene final : public GameScene
{
	enum class MenuControls
	{
		Up,
		Down,
		Select
	};

public:
	ExamGameMainMenuScene();
	virtual ~ExamGameMainMenuScene() = default;

	ExamGameMainMenuScene(const ExamGameMainMenuScene& other) = delete;
	ExamGameMainMenuScene(ExamGameMainMenuScene&& other) noexcept = delete;
	ExamGameMainMenuScene& operator=(const ExamGameMainMenuScene& other) = delete;
	ExamGameMainMenuScene& operator=(ExamGameMainMenuScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:

	std::vector<std::wstring> m_MenuOptions;

	SpriteFont* m_pFont = nullptr;
	int m_SelectedMenuOption = 0;
};
