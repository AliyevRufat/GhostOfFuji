#include "stdafx.h"
#include "TestScene.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "GameObject.h"
#include "Components.h"
#include "SpherePrefab.h"

TestScene::TestScene(void) : GameScene(L"TestScene")
{}

void TestScene::Initialize()
{
	//Create PhysX ground plane
	using namespace physx;
	GetPhysxProxy()->EnablePhysxDebugRendering(true);
	PxPhysics* const pPhysics = PhysxManager::GetInstance()->GetPhysics();
	PxMaterial* const pPhysicsMaterial = pPhysics->createMaterial(0.f, 0.f, 1.f);
	std::shared_ptr<PxGeometry> geom(new PxPlaneGeometry());

	GameObject* const pGround = new GameObject();
	pGround->AddComponent(new RigidBodyComponent(true));
	pGround->AddComponent(new ColliderComponent(geom, *pPhysicsMaterial, PxTransform(PxQuat(DirectX::XM_PIDIV2, PxVec3(0, 0, 1)))));
	AddChild(pGround);

	//Sphere
	SpherePrefab* pSphere = new SpherePrefab();
	std::shared_ptr<PxGeometry> sphereGeometry(new PxSphereGeometry(1.f));
	//
	pSphere->GetTransform()->Translate(0.f, 5.f, 0.f);
	//1. Option one --> prefer over option 2 coz getcomponent in option two is heavy for performance
	RigidBodyComponent* pRigidBody = new RigidBodyComponent();
	pRigidBody->SetCollisionGroup(CollisionGroupFlag::Group0);
	pRigidBody->SetCollisionIgnoreGroups(static_cast<CollisionGroupFlag>(uint32_t(CollisionGroupFlag::Group1) | uint32_t(CollisionGroupFlag::Group2)));
	pSphere->AddComponent(pRigidBody);
	pSphere->AddComponent(new ColliderComponent(sphereGeometry, *pPhysicsMaterial));
	AddChild(pSphere);

	pSphere = new SpherePrefab();
	pSphere->GetTransform()->Translate(0.f, 10.f, 0.f);
	pSphere->AddComponent(new RigidBodyComponent());
	//2. Option two
	pRigidBody = pSphere->GetComponent<RigidBodyComponent>();
	pRigidBody->SetCollisionGroup(CollisionGroupFlag::Group1);
	pSphere->AddComponent(new ColliderComponent(sphereGeometry, *pPhysicsMaterial));
	AddChild(pSphere);

	pSphere = new SpherePrefab();
	pSphere->GetTransform()->Translate(0.f, 20.f, 0.f);
	pSphere->AddComponent(new RigidBodyComponent());
	pRigidBody = pSphere->GetComponent<RigidBodyComponent>();
	pRigidBody->SetCollisionGroup(CollisionGroupFlag::Group2);
	pSphere->AddComponent(new ColliderComponent(sphereGeometry, *pPhysicsMaterial));
	AddChild(pSphere);

	const GameContext& gameContext = GetGameContext();
	gameContext.pInput->AddInputAction(InputAction(0, InputTriggerState::Pressed, 'R'));
}

void TestScene::Update()
{
	const GameContext& gameContext = GetGameContext();
	if (gameContext.pInput->IsActionTriggered(0))
	{
		Logger::LogInfo(L"Pressed R");
	}
}

void TestScene::Draw()
{
}