#include "stdafx.h"
#include "PlayerControllerScene.h"

#include "Components.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "Prefabs.h"
#include "GameObject.h"

PlayerControllerScene::PlayerControllerScene() : GameScene(L"PlayerControllerScene") {}

void PlayerControllerScene::Initialize()
{
	GetPhysxProxy()->EnablePhysxDebugRendering(true);
	GetGameContext().pGameTime->ForceElapsedUpperbound(true);

	// Create PhysX ground plane
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto bouncyMaterial = physX->createMaterial(0, 0, 1);
	auto ground = new GameObject();
	ground->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<physx::PxGeometry> geom(new physx::PxPlaneGeometry());
	ground->AddComponent(new ColliderComponent(geom, *bouncyMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1)))));
	AddChild(ground);

	//Add player character
	auto character = new Character();
	AddChild(character);
}

void PlayerControllerScene::Update()
{
}

void PlayerControllerScene::Draw()
{
}