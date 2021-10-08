//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "MinionScene.h"

MinionScene::MinionScene(void) :
GameScene(L"MinionScene")
{}

void MinionScene::Initialize()
{
	Logger::LogInfo(L"Welcome, humble Minion!");
}

void MinionScene::Update()
{
}

void MinionScene::Draw()
{
}
