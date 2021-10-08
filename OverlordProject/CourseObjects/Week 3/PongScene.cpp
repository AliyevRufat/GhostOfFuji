#include "stdafx.h"
#include "PongScene.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "GameObject.h"
#include "Components.h"

PongScene::PongScene() : GameScene(L"PongScene")
{}

PongScene::~PongScene()
{
}

void PongScene::Initialize()
{
	const auto& gameContext = GetGameContext();
	//Camera
	auto campEmpty = new GameObject();
	auto cam = new CameraComponent();
	campEmpty->GetTransform()->Translate(0, 1000, 0);
	campEmpty->GetTransform()->Rotate(90, 0, 0);
	campEmpty->AddComponent(cam);
	AddChild(campEmpty);
	SetActiveCamera(cam);

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
	//
	std::shared_ptr<PxGeometry> cubeGeometry(new PxBoxGeometry(40 / 2, 40 / 2, 160 / 2));
	//right peddle
	RigidBodyComponent* pRigidBodyRightCube = new RigidBodyComponent();
	pRigidBodyRightCube->SetKinematic(true);
	m_pRightPeddle = new CubePrefab(40, 40, 160);
	m_pRightPeddle->AddComponent(pRigidBodyRightCube);
	m_pRightPeddle->GetTransform()->Translate(600, 0, 0);
	auto colliderCompRight = new ColliderComponent(cubeGeometry, *pPhysicsMaterial);
	m_pRightPeddle->AddComponent(colliderCompRight);
	AddChild(m_pRightPeddle);
	//left peddle
	RigidBodyComponent* pRigidBodyLeftCube = new RigidBodyComponent();
	pRigidBodyLeftCube->SetKinematic(true);
	m_pLeftPeddle = new CubePrefab(40, 40, 160, DirectX::XMFLOAT4(0, 0, 1, 1));
	m_pLeftPeddle->AddComponent(pRigidBodyLeftCube);
	m_pLeftPeddle->GetTransform()->Translate(-600, 0, 0);
	auto colliderCompLeft = new ColliderComponent(cubeGeometry, *pPhysicsMaterial);
	m_pLeftPeddle->AddComponent(colliderCompLeft);
	AddChild(m_pLeftPeddle);
	//
	std::shared_ptr<PxGeometry> wallsGeometry(new PxBoxGeometry(1280 / 2, 40 / 2, 20 / 2));
	//top wall
	RigidBodyComponent* pRigidBodyTopWall = new RigidBodyComponent(true);
	CubePrefab* topWall = new CubePrefab(0, 0, 0);
	topWall->AddComponent(pRigidBodyTopWall);
	topWall->GetTransform()->Translate(0, 0, 360);
	auto colliderCompTowWall = new ColliderComponent(wallsGeometry, *pPhysicsMaterial);
	topWall->AddComponent(colliderCompTowWall);
	AddChild(topWall);

	//bottom wall
	RigidBodyComponent* pRigidBodyBottomWall = new RigidBodyComponent(true);
	CubePrefab* bottomWall = new CubePrefab(0, 0, 0);
	bottomWall->AddComponent(pRigidBodyBottomWall);
	bottomWall->GetTransform()->Translate(0, 0, -360);
	auto colliderCompBottomWall = new ColliderComponent(wallsGeometry, *pPhysicsMaterial);
	bottomWall->AddComponent(colliderCompBottomWall);
	AddChild(bottomWall);

	//
	auto wallTriggerCallBack = [this](GameObject* trigger, GameObject* other, GameObject::TriggerAction action)
	{
		UNREFERENCED_PARAMETER(trigger);
		UNREFERENCED_PARAMETER(other);
		UNREFERENCED_PARAMETER(action);
		ResetScene();
	};
	//
	std::shared_ptr<PxGeometry> sideWallsGeometry(new PxBoxGeometry(20 / 2, 40 / 2, 690 / 2));
	//right wall
	RigidBodyComponent* pRigidBodyRightWall = new RigidBodyComponent(true);
	GameObject* rightWall = new CubePrefab(0, 0, 0);
	rightWall->AddComponent(pRigidBodyRightWall);
	rightWall->GetTransform()->Translate(640, 0, 0);
	auto colliderCompRightWall = new ColliderComponent(sideWallsGeometry, *pPhysicsMaterial);
	colliderCompRightWall->EnableTrigger(true);
	rightWall->SetOnTriggerCallBack(wallTriggerCallBack);
	rightWall->AddComponent(colliderCompRightWall);
	AddChild(rightWall);

	//left wall
	RigidBodyComponent* pRigidBodyLeftWall = new RigidBodyComponent(true);
	GameObject* leftWall = new CubePrefab(0, 0, 0);
	leftWall->AddComponent(pRigidBodyLeftWall);
	leftWall->GetTransform()->Translate(-640, 0, 0);
	auto colliderCompLeftWall = new ColliderComponent(sideWallsGeometry, *pPhysicsMaterial);
	colliderCompLeftWall->EnableTrigger(true);
	leftWall->SetOnTriggerCallBack(wallTriggerCallBack);
	leftWall->AddComponent(colliderCompLeftWall);
	AddChild(leftWall);

	//ball
	std::shared_ptr<PxGeometry> sphereGeometry(new PxSphereGeometry(10));
	m_pBall = new SpherePrefab(10);
	auto rbBall = new RigidBodyComponent();
	rbBall->SetConstraint(RigidBodyConstraintFlag::TransY, true);
	m_pBall->AddComponent(rbBall);
	auto colliderCompSphere = new ColliderComponent(sphereGeometry, *pPhysicsMaterial);
	m_pBall->AddComponent(colliderCompSphere);
	m_pBall->GetTransform()->Translate(0, 20, 0);
	AddChild(m_pBall);

	gameContext.pInput->AddInputAction(InputAction(0, InputTriggerState::Down, 'I'));
	gameContext.pInput->AddInputAction(InputAction(1, InputTriggerState::Down, 'K'));
	gameContext.pInput->AddInputAction(InputAction(2, InputTriggerState::Pressed, 'P'));
}

void PongScene::Update()
{
	if (m_ResetBallForce)
	{
		m_pBall->GetComponent<RigidBodyComponent>()->GetPxRigidBody()->setLinearVelocity(physx::PxVec3(0, 0, 0));
		m_pBall->GetComponent<RigidBodyComponent>()->GetPxRigidBody()->setAngularVelocity(physx::PxVec3(0, 0, 0));
		m_ResetBallForce = false;
		m_CanPlay = true;
	}
	//
	const auto& gameContext = GetGameContext();
	float deltaTime = gameContext.pGameTime->GetElapsed();
	const float moveSpeed = 300.0f;

	//right peddle movement
	auto rightPeddlePosition = m_pRightPeddle->GetTransform()->GetWorldPosition();

	if (GetAsyncKeyState(VK_UP))
	{
		rightPeddlePosition.z += moveSpeed * deltaTime;
	}
	else if (GetAsyncKeyState(VK_DOWN))
	{
		rightPeddlePosition.z -= moveSpeed * deltaTime;
	}

	Clamp<float>(rightPeddlePosition.z, 270.0f, -270.0f);
	m_pRightPeddle->GetTransform()->Translate(rightPeddlePosition);

	//left peddle movement
	auto leftPeddlePosition = m_pLeftPeddle->GetTransform()->GetWorldPosition();

	if (gameContext.pInput->IsActionTriggered(0))
	{
		leftPeddlePosition.z += moveSpeed * deltaTime;
	}
	else if (gameContext.pInput->IsActionTriggered(1))
	{
		leftPeddlePosition.z -= moveSpeed * deltaTime;
	}

	Clamp<float>(leftPeddlePosition.z, 270.0f, -270.0f);
	m_pLeftPeddle->GetTransform()->Translate(leftPeddlePosition);

	if (!m_ResetBallForce && m_CanPlay && gameContext.pInput->IsActionTriggered(2))
	{
		m_CanPlay = false;
		m_pBall->GetComponent<RigidBodyComponent>()->AddForce(physx::PxVec3(0.7f, 0.0f, 0.3f) * 1000, physx::PxForceMode::eIMPULSE);
	}
}

void PongScene::Draw()
{
}

void PongScene::ResetScene()
{
	m_pBall->GetTransform()->Translate(0, 20, 0);
	m_pRightPeddle->GetTransform()->Translate(600, 0, 0);
	m_pLeftPeddle->GetTransform()->Translate(-600, 0, 0);
	//clearforce
	m_ResetBallForce = true;
}