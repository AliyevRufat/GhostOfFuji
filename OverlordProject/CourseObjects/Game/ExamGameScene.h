#pragma once
#include "GameScene.h"
#include "PickUp.h"
#include "Entity.h"
#include "Enemy.h"

class Character;
class TextureData;
class SpriteFont;
class PostProcessingDeath;
class PostProcessingGreenPulse;
class PostProcessingYellowPulse;
class PostProcessingBluePulse;

class ExamGameScene final : public GameScene
{
public:

	ExamGameScene();
	virtual ~ExamGameScene() = default;

	ExamGameScene(const ExamGameScene& other) = delete;
	ExamGameScene(ExamGameScene&& other) noexcept = delete;
	ExamGameScene& operator=(const ExamGameScene& other) = delete;
	ExamGameScene& operator=(ExamGameScene&& other) noexcept = delete;
	//Others
	void AddPickUp(Pickup* pickup);
private:

	enum class MenuControls
	{
		Up,
		Down,
		Select,
		Paused
	};

	//Draws
	void Draw() override;
	void PauseMenuDraw();
	void GameOverMenuDraw();
	void WinMenuDraw();
	//Updates
	void Update() override;
	void CameraUpdate();
	void EntitiesAttack();
	void EnemiesAttack();
	void PauseMenuUpdate();
	void GameOverMenuUpdate();
	void WinMenuUpdate();
	//Initializers
	void Initialize() override;
	void MakeLevelBoxCollider(int x, int y, int z, const DirectX::XMFLOAT3& pos);
	void CallBackLambdaInit();
	void CameraInit();
	void EnemiesInit();
	void EntitiesAndPickUpsInit(bool isRestart);
	void MaterialsInit();
	void PauseMenuInit();
	void GameOverMenuInit();
	void WinMenuInit();
	//Others
	void RestartLevel();
	void PauseOrUnPauseAllTheObjects(bool isPause);
	void DrawHUD();
	bool AreAllEnemiesDead();
	void PostProcessingUpdate();
	//--------------------------------------Datamembers------------------------
	float m_TotalPitch, m_TotalYaw;
	//Cam
	GameObject* m_pCameraCarrier = nullptr;
	CameraComponent* m_pCamera = nullptr;
	//Player
	Character* m_pCharacter = nullptr;
	GameObject* m_CallBackTriggerCollider = nullptr;
	//Interactables and enemies
	std::vector<Enemy*> m_pEnemies;
	std::vector<Entity*> m_pEntities;
	std::vector<Pickup*> m_pPickUps;
	//Pause and Game over Menus
	std::vector<std::wstring> m_PauseMenuOptions;
	std::vector<std::wstring> m_GameOverMenuOptions;
	std::vector<std::wstring> m_WinMenuOptions;
	SpriteFont* m_pFont = nullptr;
	int m_SelectedMenuOption = 0;
	bool m_IsGamePaused;
	//post proc
	PostProcessingDeath* m_pPostProcDeath;
	PostProcessingGreenPulse* m_pPostProcGreenPulse;
	PostProcessingYellowPulse* m_pPostProcYellowPulse;
	PostProcessingBluePulse* m_pPostProcBluePulse;
	float m_GreenPulseCurrentTime;
	float m_YellowPulseCurrentTime;
	float m_BluePulseCurrentTime;
	const float m_PostProcPulseMaxTime;
	//pause and game over menu png's
	GameObject* m_PauseMenuPng;
	GameObject* m_GameOverMenuPng;
	GameObject* m_WinMenuPng;
	SpriteFont* m_pHUDFont;
	bool m_AllEnemiesAreDead;
};