#include "stdafx.h"
#include "ExamGameMainMenuScene.h"

#include "Logger.h"
#include "SpriteRenderer.h"
#include "ContentManager.h"
#include "SpriteFont.h"
#include "TextRenderer.h"
#include "OverlordGame.h"
#include "SceneManager.h"
#include "SpriteComponent.h"
#include "GameObject.h"
#include "../OverlordProject/AudioManager.h"

ExamGameMainMenuScene::ExamGameMainMenuScene() : GameScene(L"ExamGameMainMenuScene")
{
}

void ExamGameMainMenuScene::Initialize()
{
	auto go = new GameObject();
	go->AddComponent(new SpriteComponent(L"./Resources/Textures/MainMenu.png", DirectX::XMFLOAT2(0.f, 0.f), DirectX::XMFLOAT4(1, 1, 1, 1)));
	AddChild(go);

	m_pFont = ContentManager::Load<SpriteFont>(L"Resources/SpriteFonts/MyConsolas.fnt");

	m_MenuOptions.push_back(L"Start");
	m_MenuOptions.push_back(L"Exit");

	GetGameContext().pInput->AddInputAction(
		InputAction((int)MenuControls::Up, InputTriggerState::Pressed, VK_UP));

	GetGameContext().pInput->AddInputAction(
		InputAction((int)MenuControls::Down, InputTriggerState::Pressed, VK_DOWN));

	GetGameContext().pInput->AddInputAction(
		InputAction((int)MenuControls::Select, InputTriggerState::Pressed, VK_RETURN));
}

void ExamGameMainMenuScene::Update()
{
	if (GetGameContext().pInput->IsActionTriggered((int)MenuControls::Up))
	{
		if (m_SelectedMenuOption == 0)
			m_SelectedMenuOption = m_MenuOptions.size() - 1;
		else m_SelectedMenuOption--;
	}
	else if (GetGameContext().pInput->IsActionTriggered((int)MenuControls::Down))
	{
		if (m_SelectedMenuOption == (int)m_MenuOptions.size() - 1)
			m_SelectedMenuOption = 0;
		else m_SelectedMenuOption++;
	}
	else if (GetGameContext().pInput->IsActionTriggered((int)MenuControls::Select))
	{
		switch (m_SelectedMenuOption)
		{
		case 0:
			SceneManager::GetInstance()->SetActiveGameScene(L"ExamGameScene");
			break;
		case 1:
			OverlordGame::EndGame();
			break;
		}
	}
}

void ExamGameMainMenuScene::Draw()
{
	if (m_pFont->GetFontName() != L"")
	{
		for (size_t i = 0; i < m_MenuOptions.size(); i++)
		{
			DirectX::XMFLOAT2 windowWH = TextRenderer::GetInstance()->GetWindowWidthHeight();
			if ((int)i == m_SelectedMenuOption)
				TextRenderer::GetInstance()->DrawText(m_pFont, m_MenuOptions[i], DirectX::XMFLOAT2(windowWH.x / 2 - 45, windowWH.y / 2 + (i * 60)), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Black));
			else TextRenderer::GetInstance()->DrawText(m_pFont, m_MenuOptions[i], DirectX::XMFLOAT2(windowWH.x / 2 - 45, windowWH.y / 2 + (i * 60)), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White));
		}
	}
}