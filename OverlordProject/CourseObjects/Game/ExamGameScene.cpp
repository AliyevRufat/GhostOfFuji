#include "stdafx.h"
#include "ExamGameScene.h"
#include "Components.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "Prefabs.h"
#include "GameObject.h"
#include "Character.h"
#include "ContentManager.h"
#include "SpriteFont.h"
#include "TextRenderer.h"
#include "SceneManager.h"
#include "HealthComponent.h"
#include "../OverlordProject/Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "../OverlordProject/Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"
#include "../OverlordProject/Materials/DiffuseMaterial.h"
#include "../OverlordProject/ParticleEffectManager.h"
#include "../OverlordProject/PostProcessingDeath.h"
#include "../OverlordProject/PostProcessingGreenPulse.h"
#include "../OverlordProject/PostProcessingYellowPulse.h"
#include "../OverlordProject/PostProcessingBluePulse.h"
#include "../OverlordProject/AudioManager.h"
#include "OverlordGame.h"

ExamGameScene::ExamGameScene()
	: GameScene(L"ExamGameScene")
	, m_pCamera(nullptr)
	, m_TotalPitch(0)
	, m_TotalYaw(0)
	, m_pEnemies{}
	, m_pEntities{}
	, m_IsGamePaused{ false }
	, m_pPostProcDeath{ nullptr }
	, m_pPostProcGreenPulse{ nullptr }
	, m_pPostProcYellowPulse{ nullptr }
	, m_pPostProcBluePulse{ nullptr }
	, m_GreenPulseCurrentTime{ 0.0f }
	, m_YellowPulseCurrentTime{ 0.0f }
	, m_BluePulseCurrentTime{ 0.0f }
	, m_PostProcPulseMaxTime{ 1.0f }
	, m_PauseMenuPng{}
	, m_GameOverMenuPng{}
	, m_WinMenuPng{}
	, m_pHUDFont{ nullptr }
	, m_AllEnemiesAreDead{ false }
{
}

void ExamGameScene::Initialize()
{
	AddChild(AudioManager::GetInstance());
	AudioManager::GetInstance()->Play(AudioManager::SoundId::Ambient);
	const auto gameContext = GetGameContext();
	gameContext.pShadowMapper->SetLight({ 0,500,0 }, { 0.0f,-0.1f,0.0f });
	GetPhysxProxy()->EnablePhysxDebugRendering(false);
	GetGameContext().pGameTime->ForceElapsedUpperbound(true);
	{
		m_pHUDFont = ContentManager::Load<SpriteFont>(L"Resources/SpriteFonts/Consolas_32.fnt");
		m_pFont = ContentManager::Load<SpriteFont>(L"Resources/SpriteFonts/MyConsolas.fnt");
		MaterialsInit();
		PauseMenuInit();
		GameOverMenuInit();
		WinMenuInit();
		{
			//-----------------------------------------------------------------PLANE-----------------------------------------------------
			auto physX = PhysxManager::GetInstance()->GetPhysics();
			auto bouncyMaterial = physX->createMaterial(0, 0, 1);
			auto ground = new GameObject();
			ground->AddComponent(new RigidBodyComponent(true));
			std::shared_ptr<physx::PxGeometry> geom(new physx::PxPlaneGeometry());
			ground->AddComponent(new ColliderComponent(geom, *bouncyMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1)))));
			AddChild(ground);
		}
		{
			//------------------------------------------------------LEVEL COLLISIONS
			MakeLevelBoxCollider(20, 20, 3000, DirectX::XMFLOAT3(-300, 0, -200));
			MakeLevelBoxCollider(30, 20, 80, DirectX::XMFLOAT3(-270, 0, 160));
			MakeLevelBoxCollider(70, 20, 120, DirectX::XMFLOAT3(-230, 0, 370));
			MakeLevelBoxCollider(100, 20, 130, DirectX::XMFLOAT3(-180, 0, 540));
			MakeLevelBoxCollider(300, 20, 10, DirectX::XMFLOAT3(-15, 0, -210));
			MakeLevelBoxCollider(20, 20, 300, DirectX::XMFLOAT3(280, 0, -40));
			MakeLevelBoxCollider(300, 20, 330, DirectX::XMFLOAT3(500, 0, 540));
			MakeLevelBoxCollider(800, 20, 100, DirectX::XMFLOAT3(520, 0, 1760));
			MakeLevelBoxCollider(1000, 20, 130, DirectX::XMFLOAT3(1440, 0, 950));
			MakeLevelBoxCollider(300, 20, 300, DirectX::XMFLOAT3(1200, 0, 2100));
			MakeLevelBoxCollider(75, 20, 75, DirectX::XMFLOAT3(-225, 0, 1600));
			MakeLevelBoxCollider(20, 20, 550, DirectX::XMFLOAT3(2400, 0, 1590));
			MakeLevelBoxCollider(450, 20, 30, DirectX::XMFLOAT3(1950, 0, 2050));
		}
	}
	//-----------------------------------------------------------------PLAYER, INTERACTABLES AND ENEMIES-----------------------------------------------------
	m_pCharacter = new Character();
	m_pCharacter->AddComponent(new HealthComponent(100));
	m_pCharacter->SetTag(L"Player");
	AddChild(m_pCharacter);
	//
	CameraInit();
	EnemiesInit();
	CallBackLambdaInit();
	EntitiesAndPickUpsInit(false);
	//sky
	GameObject* skyPlane = new GameObject();

	auto modelComp = new ModelComponent(L"Resources/Meshes/Plane.ovm");
	modelComp->SetMaterial(77);
	skyPlane->AddComponent(modelComp);

	skyPlane->GetTransform()->Translate(0, 0, 2500);
	skyPlane->GetTransform()->Scale(50, 50, 50);
	skyPlane->GetTransform()->Rotate(0, 0, 90);

	AddChild(skyPlane);
	//sky
	skyPlane = new GameObject();

	modelComp = new ModelComponent(L"Resources/Meshes/Plane.ovm");
	modelComp->SetMaterial(77);
	skyPlane->AddComponent(modelComp);

	skyPlane->GetTransform()->Translate(0, 0, -2000);
	skyPlane->GetTransform()->Scale(50, 50, 50);
	skyPlane->GetTransform()->Rotate(0, 0, 90);

	AddChild(skyPlane);
	//sky
	skyPlane = new GameObject();

	modelComp = new ModelComponent(L"Resources/Meshes/Plane.ovm");
	modelComp->SetMaterial(77);
	skyPlane->AddComponent(modelComp);

	skyPlane->GetTransform()->Translate(-2500, 0, 0);
	skyPlane->GetTransform()->Scale(50, 50, 50);
	skyPlane->GetTransform()->Rotate(0, 90, 90);

	AddChild(skyPlane);
	//sky
	skyPlane = new GameObject();

	modelComp = new ModelComponent(L"Resources/Meshes/Plane.ovm");
	modelComp->SetMaterial(77);
	skyPlane->AddComponent(modelComp);

	skyPlane->GetTransform()->Translate(2500, 0, 0);
	skyPlane->GetTransform()->Scale(50, 50, 50);
	skyPlane->GetTransform()->Rotate(0, 90, 90);

	AddChild(skyPlane);
}

void ExamGameScene::Update()
{
	if (m_AllEnemiesAreDead)
	{
		m_WinMenuPng->SetIsVisible(true);
		WinMenuUpdate();
	}
	PostProcessingUpdate();
	//particles
	ParticleEffectManager::GetInstance()->Update(m_GameContext.pGameTime->GetElapsed());
	//if player dead
	if (m_pCharacter->GetIsDead())
	{
		m_GameOverMenuPng->SetIsVisible(true);
		GameOverMenuUpdate();
		return;
	}
	//if pressed pause
	if (GetGameContext().pInput->IsActionTriggered((int)MenuControls::Paused) && !m_pCharacter->GetIsDead())
	{
		AudioManager::GetInstance()->Play(AudioManager::SoundId::Pause);

		if (m_IsGamePaused)
		{
			m_IsGamePaused = false;
			m_PauseMenuPng->SetIsVisible(false);
			PauseOrUnPauseAllTheObjects(false);
		}
		else
		{
			m_IsGamePaused = true;
			m_PauseMenuPng->SetIsVisible(true);
			PauseOrUnPauseAllTheObjects(true);
		}
	}
	if (m_IsGamePaused)
	{
		PauseMenuUpdate();
		return;
	}
	//player attack enemy
	EnemiesAttack();
	//update player pos for enemy
	for (auto& enemy : m_pEnemies)
	{
		enemy->SetPlayerPos(m_pCharacter->GetTransform()->GetPosition());
	}
	//collision pos update
	m_CallBackTriggerCollider->GetTransform()->Translate(m_pCharacter->GetTransform()->GetPosition());
	//cam update
	CameraUpdate();
	//
	EntitiesAttack();
}

void ExamGameScene::CameraUpdate()
{
	//set player character directions and the camera rotation to the cameras directions
	m_pCharacter->SetForwardAndRightDirections(m_pCamera->GetTransform()->GetForward(), m_pCamera->GetTransform()->GetRight());
	//m_pCharacter->SetCameraForward(m_pCameraCarrier->GetTransform()->GetForward());
	//set camera carrier gameobjects position
	m_pCameraCarrier->GetTransform()->Translate(m_pCharacter->GetTransform()->GetPosition());
	//update rotation of the camera carrier with mouse
	auto look = DirectX::XMFLOAT2(0, 0);
	if (InputManager::IsMouseButtonDown(VK_LBUTTON))
	{
		const auto mouseMove = InputManager::GetMouseMovement();
		look.x = static_cast<float>(mouseMove.x);
		look.y = static_cast<float>(mouseMove.y);

		if (look.x == 0 && look.y == 0)
		{
			look = InputManager::GetThumbstickPosition(false);
		}

		m_TotalYaw += look.x * 10.f * m_GameContext.pGameTime->GetElapsed();
		m_TotalPitch += look.y * 10.f * m_GameContext.pGameTime->GetElapsed();

		Clamp(m_TotalPitch, 26.0f, -26.0f);
	}
	m_pCameraCarrier->GetTransform()->Rotate(m_TotalPitch, m_TotalYaw, 0);
}

bool ExamGameScene::AreAllEnemiesDead()
{
	for (auto& enemy : m_pEnemies)
	{
		if (enemy && !enemy->GetIsDead())
		{
			return false;
		}
	}
	return true;
}

void ExamGameScene::EnemiesAttack()
{
	m_AllEnemiesAreDead = AreAllEnemiesDead();

	if (m_pCharacter->IsAttacking())
	{
		for (auto& enemy : m_pEnemies)
		{
			if (enemy->GetIsCollidingPlayer() && enemy->GetIsAttackable() && !enemy->GetDeleteLater())
			{
				enemy->GetComponent<HealthComponent>()->Damage(50);
				enemy->SetIsAttackable(false);
			}
		}
	}
	else
	{
		for (auto& enemy : m_pEnemies)
		{
			if (!enemy->GetDeleteLater())
			{
				enemy->SetIsAttackable(true);
			}
		}
	}
}

void ExamGameScene::EntitiesAttack()
{
	if (m_pCharacter->IsAttacking())
	{
		for (auto& entity : m_pEntities)
		{
			if (entity->GetIsCollidingPlayer() && entity->GetIsAttackable() && !entity->GetDeleteLater())
			{
				entity->GetComponent<HealthComponent>()->Damage(50);
				entity->SetIsAttackable(false);
			}
		}
	}
	else
	{
		for (auto& entity : m_pEntities)
		{
			if (!entity->GetDeleteLater())
			{
				entity->SetIsAttackable(true);
			}
		}
	}
}

void ExamGameScene::Draw()
{
	DrawHUD();
	//if player dead
	if (m_AllEnemiesAreDead)
	{
		WinMenuDraw();
		return;
	}
	if (m_pCharacter->GetIsDead())
	{
		GameOverMenuDraw();
		return;
	}
	if (m_IsGamePaused)
	{
		PauseMenuDraw();
		return;
	}
}

void ExamGameScene::CallBackLambdaInit()
{
	using namespace DirectX;
	using namespace physx;
	PxPhysics* const pxPhysX = PhysxManager::GetInstance()->GetPhysics();
	PxMaterial* const pPhysicsMaterial = pxPhysX->createMaterial(0.f, 0.f, 1.f);

	auto characterCallBacks = [this](GameObject* trigger, GameObject* other, GameObject::TriggerAction action)
	{
		if (trigger->GetTag() == L"Player")
		{
			if (other->GetTag() == L"DestroyableEntity")
			{
				if (action == GameObject::TriggerAction::ENTER)
				{
					//enter collision
					Entity* entity = static_cast<Entity*>(other);
					if (entity)
					{
						entity->SetIsCollidingPlayer(true);
					}
				}
				else
				{
					//exit collision
					Entity* entity = static_cast<Entity*>(other);
					if (entity)
					{
						entity->SetIsCollidingPlayer(false);
					}
				}
			}
			else if (other->GetTag() == L"Pickup")
			{
				if (action == GameObject::TriggerAction::ENTER)
				{
					//enter collision
					Pickup* pickup = static_cast<Pickup*>(other);
					pickup->OnPickUp(static_cast<Character*>(trigger->GetParent()));

					switch (pickup->GetPickupType())
					{
					case Pickup::PickupType::health:
						m_pPostProcGreenPulse = new PostProcessingGreenPulse();
						SceneManager::GetInstance()->GetActiveScene()->AddPostProcessingEffect(m_pPostProcGreenPulse);
						AudioManager::GetInstance()->Play(AudioManager::SoundId::PickupHealth);
						break;
					case Pickup::PickupType::score:
						m_pPostProcYellowPulse = new PostProcessingYellowPulse();
						SceneManager::GetInstance()->GetActiveScene()->AddPostProcessingEffect(m_pPostProcYellowPulse);
						AudioManager::GetInstance()->Play(AudioManager::SoundId::PickupScore);
						break;
					case Pickup::PickupType::throwable:
						m_pPostProcBluePulse = new PostProcessingBluePulse();
						SceneManager::GetInstance()->GetActiveScene()->AddPostProcessingEffect(m_pPostProcBluePulse);
						AudioManager::GetInstance()->Play(AudioManager::SoundId::PickupThrowable);
						break;
					}
				}
			}
			else if (other->GetTag() == L"Enemy")
			{
				if (action == GameObject::TriggerAction::ENTER)
				{
					//enter collision
					Enemy* enemy = static_cast<Enemy*>(other);
					if (enemy)
					{
						enemy->SetIsCollidingPlayer(true);
					}
				}
				else
				{
					//enter collision
					Enemy* enemy = static_cast<Enemy*>(other);
					if (enemy)
					{
						enemy->SetIsCollidingPlayer(false);
					}
				}
			}
		}
	};

	//gameobject with rigidbody component and collision component that acts as a trigger
	using namespace physx;
	std::shared_ptr<PxGeometry> capsuleGeometry(new PxCapsuleGeometry(6, 2));
	RigidBodyComponent* pRb = new RigidBodyComponent(false);

	m_CallBackTriggerCollider = new CubePrefab(0, 0, 0);
	m_CallBackTriggerCollider->SetTag(m_pCharacter->GetTag());
	m_CallBackTriggerCollider->AddComponent(pRb);
	m_CallBackTriggerCollider->GetTransform()->Translate(0, 0, 50);
	auto colliderCompCapsule = new ColliderComponent(capsuleGeometry, *pPhysicsMaterial);
	colliderCompCapsule->EnableTrigger(true);
	m_CallBackTriggerCollider->SetOnTriggerCallBack(characterCallBacks);
	m_CallBackTriggerCollider->AddComponent(colliderCompCapsule);
	m_pCharacter->AddChild(m_CallBackTriggerCollider);
}

void ExamGameScene::CameraInit()
{
	DirectX::XMFLOAT3 cameraOffset = DirectX::XMFLOAT3(0, 10, -25);
	auto camEmpty = new GameObject();
	auto cam = new CameraComponent();
	camEmpty->GetTransform()->Translate(cameraOffset);
	camEmpty->GetTransform()->Rotate(20, 0, 0);
	camEmpty->AddComponent(cam);
	m_pCamera = cam;
	//
	m_pCameraCarrier = new GameObject();
	m_pCameraCarrier->GetTransform()->Translate(m_pCharacter->GetTransform()->GetPosition());
	m_pCameraCarrier->AddChild(camEmpty);
	AddChild(m_pCameraCarrier);
	m_pCamera->SetActive();
	//
	m_pCharacter->SetForwardAndRightDirections(m_pCamera->GetTransform()->GetForward(), m_pCamera->GetTransform()->GetRight());
	//
}

void ExamGameScene::EnemiesInit()
{
	auto enemy = new Enemy(DirectX::XMFLOAT3(2000, 0, 1670), L"Resources/Animations/Ovm/Ninja.ovm", 8, 20, m_pCharacter);
	enemy->SetTag(L"Enemy");
	AddChild(enemy);
	m_pEnemies.push_back(enemy);
	//
	enemy = new Enemy(DirectX::XMFLOAT3(2000, 0, 1530), L"Resources/Animations/Ovm/Ninja.ovm", 8, 20, m_pCharacter);
	enemy->SetTag(L"Enemy");
	AddChild(enemy);
	m_pEnemies.push_back(enemy);
	//
	enemy = new Enemy(DirectX::XMFLOAT3(1900, 0, 1600), L"Resources/Animations/Ovm/Ninja.ovm", 8, 20, m_pCharacter);
	enemy->SetTag(L"Enemy");
	AddChild(enemy);
	m_pEnemies.push_back(enemy);
	//
	enemy = new Enemy(DirectX::XMFLOAT3(1980, 0, 1340), L"Resources/Animations/Ovm/Ninja.ovm", 8, 20, m_pCharacter);
	enemy->SetTag(L"Enemy");
	AddChild(enemy);
	m_pEnemies.push_back(enemy);
	//
	enemy = new Enemy(DirectX::XMFLOAT3(1800, 0, 1150), L"Resources/Animations/Ovm/Ninja.ovm", 8, 20, m_pCharacter);
	enemy->SetTag(L"Enemy");
	AddChild(enemy);
	m_pEnemies.push_back(enemy);
	//
	enemy = new Enemy(DirectX::XMFLOAT3(1450, 0, 1450), L"Resources/Animations/Ovm/Ninja.ovm", 8, 20, m_pCharacter);
	enemy->SetTag(L"Enemy");
	AddChild(enemy);
	m_pEnemies.push_back(enemy);
	//
	enemy = new Enemy(DirectX::XMFLOAT3(700, 0, 1540), L"Resources/Animations/Ovm/Ninja.ovm", 8, 20, m_pCharacter);
	enemy->SetTag(L"Enemy");
	AddChild(enemy);
	m_pEnemies.push_back(enemy);
	//
	enemy = new Enemy(DirectX::XMFLOAT3(661, 0, 1410), L"Resources/Animations/Ovm/Ninja.ovm", 8, 20, m_pCharacter);
	enemy->SetTag(L"Enemy");
	AddChild(enemy);
	m_pEnemies.push_back(enemy);
	//
	enemy = new Enemy(DirectX::XMFLOAT3(450, 0, 1210), L"Resources/Animations/Ovm/Ninja.ovm", 8, 20, m_pCharacter);
	enemy->SetTag(L"Enemy");
	AddChild(enemy);
	m_pEnemies.push_back(enemy);
	//
	enemy = new Enemy(DirectX::XMFLOAT3(-60, 0, 1420), L"Resources/Animations/Ovm/Ninja.ovm", 8, 20, m_pCharacter);
	enemy->SetTag(L"Enemy");
	AddChild(enemy);
	m_pEnemies.push_back(enemy);
	//
	enemy = new Enemy(DirectX::XMFLOAT3(-80, 0, 1120), L"Resources/Animations/Ovm/Ninja.ovm", 8, 20, m_pCharacter);
	enemy->SetTag(L"Enemy");
	AddChild(enemy);
	m_pEnemies.push_back(enemy);
	//
	enemy = new Enemy(DirectX::XMFLOAT3(30, 0, 650), L"Resources/Animations/Ovm/Ninja.ovm", 8, 20, m_pCharacter);
	enemy->SetTag(L"Enemy");
	AddChild(enemy);
	m_pEnemies.push_back(enemy);
	//
	enemy = new Enemy(DirectX::XMFLOAT3(-50, 0, 335), L"Resources/Animations/Ovm/Ninja.ovm", 8, 20, m_pCharacter);
	enemy->SetTag(L"Enemy");
	AddChild(enemy);
	m_pEnemies.push_back(enemy);
	//
	enemy = new Enemy(DirectX::XMFLOAT3(240, 0, 1050), L"Resources/Animations/Ovm/Ninja.ovm", 8, 20, m_pCharacter);
	enemy->SetTag(L"Enemy");
	AddChild(enemy);
	m_pEnemies.push_back(enemy);
}

void ExamGameScene::MaterialsInit()
{
	auto entity1Mat = new DiffuseMaterial_Shadow();
	entity1Mat->SetDiffuseTexture(L"Resources/Textures/Crate_Diffuse.png");
	m_GameContext.pMaterialManager->AddMaterial(entity1Mat, 5);

	auto pickUp1Mat = new DiffuseMaterial_Shadow();
	pickUp1Mat->SetDiffuseTexture(L"Resources/Textures/Medkit.jpg");
	m_GameContext.pMaterialManager->AddMaterial(pickUp1Mat, 16);

	auto pickUp2Mat = new DiffuseMaterial_Shadow();
	pickUp2Mat->SetDiffuseTexture(L"Resources/Textures/Coin.png");
	m_GameContext.pMaterialManager->AddMaterial(pickUp2Mat, 17);

	auto playerMat = new SkinnedDiffuseMaterial_Shadow();
	playerMat->SetDiffuseTexture(L"Resources/Textures/Ninja_Diffuse.png");
	m_GameContext.pMaterialManager->AddMaterial(playerMat, 7);

	auto enemyMat = new SkinnedDiffuseMaterial_Shadow();
	enemyMat->SetDiffuseTexture(L"Resources/Textures/Ninja_DiffuseEnemy.png");
	m_GameContext.pMaterialManager->AddMaterial(enemyMat, 8);

	auto groundMat = new DiffuseMaterial_Shadow();
	groundMat->SetDiffuseTexture(L"Resources/Textures/GroundDiffuse.jpg");
	m_GameContext.pMaterialManager->AddMaterial(groundMat, 99);

	auto treeMat = new DiffuseMaterial_Shadow();
	treeMat->SetDiffuseTexture(L"Resources/Textures/EnvironmentTexture.png");
	m_GameContext.pMaterialManager->AddMaterial(treeMat, 6);

	auto skyMat = new DiffuseMaterial();
	skyMat->SetDiffuseTexture(L"Resources/Textures/Sky.png");
	m_GameContext.pMaterialManager->AddMaterial(skyMat, 77);
}

void ExamGameScene::EntitiesAndPickUpsInit(bool isRestart)
{
	const int crateScale = 3;
	const int spawnPosYOffset = 2;
	Entity* entity;

	if (!isRestart)
	{
		//---------------------Environment
		entity = new Entity(L"Resources/Meshes/Cliffs.ovm", 6, false, false, DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(90, 90, 0), DirectX::XMFLOAT3(2, 2, 2), DirectX::XMFLOAT3(1, 1, 1));
		AddChild(entity);

		for (size_t i = 0; i < 20; i++)
		{
			for (size_t j = 0; j < 10; j++)
			{
				entity = new Entity(L"Resources/Meshes/Tree.ovm", 6, false, true, DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(90, 90, 0), DirectX::XMFLOAT3(1, 1, 1), DirectX::XMFLOAT3(2, 2, 7));
				float posX = j * (randF(0.1f, 0.3f) * 1000) - 300;
				float posZ = i * (randF(0.1f, 0.3f) * 1000) - 200 + 75;
				entity->GetComponent<TransformComponent>()->Translate(DirectX::XMFLOAT3(posX, 0, posZ));
				AddChild(entity);
			}
		}

		//--------------------Ground

		entity = new Entity(L"Resources/Meshes/Ground.ovm", 99, false, false, DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(90, 90, 0), DirectX::XMFLOAT3(2, 2, 2), DirectX::XMFLOAT3(1, 1, 1));
		AddChild(entity);
	}
	//------------------------------------------------------------------CRATES
	entity = new Entity(L"Resources/Meshes/Crate.ovm", 5, true, true, DirectX::XMFLOAT3(110, spawnPosYOffset, 90), DirectX::XMFLOAT3(90, 0, 0), DirectX::XMFLOAT3(crateScale, crateScale, crateScale), DirectX::XMFLOAT3(1, 1, 1));
	AddChild(entity);
	m_pEntities.push_back(entity);

	entity = new Entity(L"Resources/Meshes/Crate.ovm", 5, true, true, DirectX::XMFLOAT3(70, spawnPosYOffset, 200), DirectX::XMFLOAT3(90, 0, 0), DirectX::XMFLOAT3(crateScale, crateScale, crateScale), DirectX::XMFLOAT3(1, 1, 1));
	AddChild(entity);
	m_pEntities.push_back(entity);

	entity = new Entity(L"Resources/Meshes/Crate.ovm", 5, true, true, DirectX::XMFLOAT3(-70, spawnPosYOffset, 110), DirectX::XMFLOAT3(90, 0, 0), DirectX::XMFLOAT3(crateScale, crateScale, crateScale), DirectX::XMFLOAT3(1, 1, 1));
	AddChild(entity);
	m_pEntities.push_back(entity);

	entity = new Entity(L"Resources/Meshes/Crate.ovm", 5, true, true, DirectX::XMFLOAT3(145, spawnPosYOffset, 565), DirectX::XMFLOAT3(90, 0, 0), DirectX::XMFLOAT3(crateScale, crateScale, crateScale), DirectX::XMFLOAT3(1, 1, 1));
	AddChild(entity);
	m_pEntities.push_back(entity);

	entity = new Entity(L"Resources/Meshes/Crate.ovm", 5, true, true, DirectX::XMFLOAT3(0, spawnPosYOffset, 510), DirectX::XMFLOAT3(90, 0, 0), DirectX::XMFLOAT3(crateScale, crateScale, crateScale), DirectX::XMFLOAT3(1, 1, 1));
	AddChild(entity);
	m_pEntities.push_back(entity);

	entity = new Entity(L"Resources/Meshes/Crate.ovm", 5, true, true, DirectX::XMFLOAT3(-140, spawnPosYOffset, 1120), DirectX::XMFLOAT3(90, 0, 0), DirectX::XMFLOAT3(crateScale, crateScale, crateScale), DirectX::XMFLOAT3(1, 1, 1));
	AddChild(entity);
	m_pEntities.push_back(entity);

	entity = new Entity(L"Resources/Meshes/Crate.ovm", 5, true, true, DirectX::XMFLOAT3(-180, spawnPosYOffset, 1450), DirectX::XMFLOAT3(90, 0, 0), DirectX::XMFLOAT3(crateScale, crateScale, crateScale), DirectX::XMFLOAT3(1, 1, 1));
	AddChild(entity);
	m_pEntities.push_back(entity);

	entity = new Entity(L"Resources/Meshes/Crate.ovm", 5, true, true, DirectX::XMFLOAT3(230, spawnPosYOffset, 1600), DirectX::XMFLOAT3(90, 0, 0), DirectX::XMFLOAT3(crateScale, crateScale, crateScale), DirectX::XMFLOAT3(1, 1, 1));
	AddChild(entity);
	m_pEntities.push_back(entity);

	entity = new Entity(L"Resources/Meshes/Crate.ovm", 5, true, true, DirectX::XMFLOAT3(700, spawnPosYOffset, 1250), DirectX::XMFLOAT3(90, 0, 0), DirectX::XMFLOAT3(crateScale, crateScale, crateScale), DirectX::XMFLOAT3(1, 1, 1));
	AddChild(entity);
	m_pEntities.push_back(entity);

	entity = new Entity(L"Resources/Meshes/Crate.ovm", 5, true, true, DirectX::XMFLOAT3(1230, spawnPosYOffset, 1510), DirectX::XMFLOAT3(90, 0, 0), DirectX::XMFLOAT3(crateScale, crateScale, crateScale), DirectX::XMFLOAT3(1, 1, 1));
	AddChild(entity);
	m_pEntities.push_back(entity);

	entity = new Entity(L"Resources/Meshes/Crate.ovm", 5, true, true, DirectX::XMFLOAT3(1850, spawnPosYOffset, 1210), DirectX::XMFLOAT3(90, 0, 0), DirectX::XMFLOAT3(crateScale, crateScale, crateScale), DirectX::XMFLOAT3(1, 1, 1));
	AddChild(entity);
	m_pEntities.push_back(entity);

	entity = new Entity(L"Resources/Meshes/Crate.ovm", 5, true, true, DirectX::XMFLOAT3(2270, spawnPosYOffset, 1380), DirectX::XMFLOAT3(90, 0, 0), DirectX::XMFLOAT3(crateScale, crateScale, crateScale), DirectX::XMFLOAT3(1, 1, 1));
	AddChild(entity);
	m_pEntities.push_back(entity);

	entity = new Entity(L"Resources/Meshes/Crate.ovm", 5, true, true, DirectX::XMFLOAT3(2170, spawnPosYOffset, 1800), DirectX::XMFLOAT3(90, 0, 0), DirectX::XMFLOAT3(crateScale, crateScale, crateScale), DirectX::XMFLOAT3(1, 1, 1));
	AddChild(entity);
	m_pEntities.push_back(entity);
	//-----------------------------------------------------------------PICKUP----------------------------------------------------------
	Pickup* pickup = new Pickup(L"Resources/Meshes/Medkit.ovm", 16, DirectX::XMFLOAT3(2.5f, 7, 120), DirectX::XMFLOAT3(0, 0, 0), Pickup::PickupType::health);
	AddPickUp(pickup);

	pickup = new Pickup(L"Resources/Meshes/Coin.ovm", 17, DirectX::XMFLOAT3(6, 7, 220), DirectX::XMFLOAT3(0, 0, 0), Pickup::PickupType::score);
	AddPickUp(pickup);

	pickup = new Pickup(L"Resources/Meshes/Throwable.ovm", 6, DirectX::XMFLOAT3(10, 7, 556), DirectX::XMFLOAT3(0, 0, 0), Pickup::PickupType::throwable);
	AddPickUp(pickup);

	pickup = new Pickup(L"Resources/Meshes/Throwable.ovm", 6, DirectX::XMFLOAT3(30, 7, 556), DirectX::XMFLOAT3(0, 0, 0), Pickup::PickupType::throwable);
	AddPickUp(pickup);

	pickup = new Pickup(L"Resources/Meshes/Throwable.ovm", 6, DirectX::XMFLOAT3(50, 7, 556), DirectX::XMFLOAT3(0, 0, 0), Pickup::PickupType::throwable);
	AddPickUp(pickup);

	pickup = new Pickup(L"Resources/Meshes/Throwable.ovm", 6, DirectX::XMFLOAT3(70, 7, 556), DirectX::XMFLOAT3(0, 0, 0), Pickup::PickupType::throwable);
	AddPickUp(pickup);

	pickup = new Pickup(L"Resources/Meshes/Throwable.ovm", 6, DirectX::XMFLOAT3(110, 7, 556), DirectX::XMFLOAT3(0, 0, 0), Pickup::PickupType::throwable);
	AddPickUp(pickup);

	pickup = new Pickup(L"Resources/Meshes/Throwable.ovm", 6, DirectX::XMFLOAT3(90, 7, 556), DirectX::XMFLOAT3(0, 0, 0), Pickup::PickupType::throwable);
	AddPickUp(pickup);
}

void ExamGameScene::MakeLevelBoxCollider(int x, int y, int z, const DirectX::XMFLOAT3& pos)
{
	using namespace physx;
	std::shared_ptr<PxGeometry> boxGeom(new PxBoxGeometry(physx::PxReal(x), physx::PxReal(y), physx::PxReal(z)));
	PxPhysics* const pPhysics = PhysxManager::GetInstance()->GetPhysics();
	PxMaterial* const pPhysicsMaterial = pPhysics->createMaterial(0.f, 0.f, 1.f);
	//
	GameObject* levelColliderObject = new GameObject();
	levelColliderObject->SetTag(L"Environment");
	ColliderComponent* collision = new ColliderComponent(boxGeom, *pPhysicsMaterial);
	levelColliderObject->AddComponent(new RigidBodyComponent(true));
	levelColliderObject->AddComponent(collision);
	levelColliderObject->GetTransform()->Translate(pos);
	AddChild(levelColliderObject);
}

void ExamGameScene::PauseMenuInit()
{
	m_PauseMenuPng = new GameObject();
	m_PauseMenuPng->AddComponent(new SpriteComponent(L"./Resources/Textures/PauseMenu.jpg", DirectX::XMFLOAT2(0.f, 0.f), DirectX::XMFLOAT4(1, 1, 1, 1)));
	m_PauseMenuPng->SetIsVisible(false);
	AddChild(m_PauseMenuPng);
	//Adding text
	m_PauseMenuOptions.push_back(L"Resume");
	m_PauseMenuOptions.push_back(L"Restart");
	m_PauseMenuOptions.push_back(L"Exit");
	//Binding keys
	GetGameContext().pInput->AddInputAction(InputAction((int)MenuControls::Up, InputTriggerState::Pressed, VK_UP));
	GetGameContext().pInput->AddInputAction(InputAction((int)MenuControls::Down, InputTriggerState::Pressed, VK_DOWN));
	GetGameContext().pInput->AddInputAction(InputAction((int)MenuControls::Select, InputTriggerState::Pressed, VK_RETURN));
	GetGameContext().pInput->AddInputAction(InputAction((int)MenuControls::Paused, InputTriggerState::Pressed, 'P'));
}

void ExamGameScene::PauseMenuDraw()
{
	if (m_pFont->GetFontName() != L"")
	{
		for (size_t i = 0; i < m_PauseMenuOptions.size(); i++)
		{
			DirectX::XMFLOAT2 windowWH = TextRenderer::GetInstance()->GetWindowWidthHeight();
			if ((int)i == m_SelectedMenuOption)
			{
				TextRenderer::GetInstance()->DrawText(m_pFont, m_PauseMenuOptions[i], DirectX::XMFLOAT2(windowWH.x / 2 - 80, windowWH.y / 2 + (i * 60) - 50), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Black));
			}
			else
			{
				TextRenderer::GetInstance()->DrawText(m_pFont, m_PauseMenuOptions[i], DirectX::XMFLOAT2(windowWH.x / 2 - 80, windowWH.y / 2 + (i * 60) - 50), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White));
			}
		}
	}
}

void ExamGameScene::PauseMenuUpdate()
{
	if (GetGameContext().pInput->IsActionTriggered((int)MenuControls::Up))
	{
		if (m_SelectedMenuOption == 0)
		{
			m_SelectedMenuOption = m_PauseMenuOptions.size() - 1;
		}
		else
		{
			m_SelectedMenuOption--;
		}
	}
	else if (GetGameContext().pInput->IsActionTriggered((int)MenuControls::Down))
	{
		if (m_SelectedMenuOption == (int)m_PauseMenuOptions.size() - 1)
		{
			m_SelectedMenuOption = 0;
		}
		else
		{
			m_SelectedMenuOption++;
		}
	}
	else if (GetGameContext().pInput->IsActionTriggered((int)MenuControls::Select))
	{
		if (m_SelectedMenuOption == 0)
		{
			m_IsGamePaused = false;
			PauseOrUnPauseAllTheObjects(false);
			m_SelectedMenuOption = 0;
		}
		else if (m_SelectedMenuOption == 1)
		{
			RestartLevel();
			m_SelectedMenuOption = 0;
			//post proc remove
			if (m_pPostProcDeath)
			{
				SceneManager::GetInstance()->GetActiveScene()->RemovePostProcessingEffect(m_pPostProcDeath);
				m_pPostProcDeath = nullptr;
			}
		}
		else if (m_SelectedMenuOption == 2)
		{
			//EXIT GAME
			OverlordGame::EndGame();
		}
	}
}

void ExamGameScene::GameOverMenuInit()
{
	m_GameOverMenuPng = new GameObject();
	m_GameOverMenuPng->AddComponent(new SpriteComponent(L"./Resources/Textures/GameOverMenu.png", DirectX::XMFLOAT2(0.f, 0.f), DirectX::XMFLOAT4(1, 1, 1, 1)));
	m_GameOverMenuPng->SetIsVisible(false);
	AddChild(m_GameOverMenuPng);

	//Adding text
	m_GameOverMenuOptions.push_back(L"Restart");
	m_GameOverMenuOptions.push_back(L"MainMenu");
}

void ExamGameScene::GameOverMenuDraw()
{
	if (m_pFont->GetFontName() != L"")
	{
		for (size_t i = 0; i < m_GameOverMenuOptions.size(); i++)
		{
			DirectX::XMFLOAT2 windowWH = TextRenderer::GetInstance()->GetWindowWidthHeight();
			if ((int)i == m_SelectedMenuOption)
			{
				TextRenderer::GetInstance()->DrawText(m_pFont, m_GameOverMenuOptions[i], DirectX::XMFLOAT2(windowWH.x / 2 - 80, windowWH.y / 2 + (i * 60) - 50), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Black));
			}
			else
			{
				TextRenderer::GetInstance()->DrawText(m_pFont, m_GameOverMenuOptions[i], DirectX::XMFLOAT2(windowWH.x / 2 - 80, windowWH.y / 2 + (i * 60) - 50), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Black));
			}
		}
	}
}

void ExamGameScene::GameOverMenuUpdate()
{
	if (GetGameContext().pInput->IsActionTriggered((int)MenuControls::Up))
	{
		if (m_SelectedMenuOption == 0)
			m_SelectedMenuOption = m_GameOverMenuOptions.size() - 1;
		else m_SelectedMenuOption--;
	}
	else if (GetGameContext().pInput->IsActionTriggered((int)MenuControls::Down))
	{
		if (m_SelectedMenuOption == (int)m_GameOverMenuOptions.size() - 1)
			m_SelectedMenuOption = 0;
		else m_SelectedMenuOption++;
	}
	else if (GetGameContext().pInput->IsActionTriggered((int)MenuControls::Select))
	{
		if (m_SelectedMenuOption == 0)
		{
			RestartLevel();
			m_SelectedMenuOption = 0;
			//post proc remove
			if (m_pPostProcDeath)
			{
				SceneManager::GetInstance()->GetActiveScene()->RemovePostProcessingEffect(m_pPostProcDeath);
				m_pPostProcDeath = nullptr;
			}
		}
		else if (m_SelectedMenuOption == 1)
		{
			RestartLevel();
			SceneManager::GetInstance()->SetActiveGameScene(L"ExamGameMainMenuScene");
			m_SelectedMenuOption = 0;
			//post proc remove
			if (m_pPostProcDeath)
			{
				SceneManager::GetInstance()->GetActiveScene()->RemovePostProcessingEffect(m_pPostProcDeath);
				m_pPostProcDeath = nullptr;
			}
		}
	}
}

void ExamGameScene::WinMenuInit()
{
	m_WinMenuPng = new GameObject();
	m_WinMenuPng->AddComponent(new SpriteComponent(L"./Resources/Textures/WinMenu.png", DirectX::XMFLOAT2(0.f, 0.f), DirectX::XMFLOAT4(1, 1, 1, 1)));
	m_WinMenuPng->SetIsVisible(false);
	AddChild(m_WinMenuPng);

	//Adding text
	m_WinMenuOptions.push_back(L"Restart");
	m_WinMenuOptions.push_back(L"MainMenu");
}

void ExamGameScene::WinMenuDraw()
{
	if (m_pFont->GetFontName() != L"")
	{
		DirectX::XMFLOAT2 windowWH = TextRenderer::GetInstance()->GetWindowWidthHeight();
		for (size_t i = 0; i < m_WinMenuOptions.size(); i++)
		{
			if ((int)i == m_SelectedMenuOption)
			{
				TextRenderer::GetInstance()->DrawText(m_pFont, m_WinMenuOptions[i], DirectX::XMFLOAT2(windowWH.x / 2 - 80, windowWH.y / 2 + (i * 60) - 50), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Black));
			}
			else
			{
				TextRenderer::GetInstance()->DrawText(m_pFont, m_WinMenuOptions[i], DirectX::XMFLOAT2(windowWH.x / 2 - 80, windowWH.y / 2 + (i * 60) - 50), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Black));
			}
		}
		if (m_pHUDFont->GetFontName() != L"")
		{
			int healthHUD = m_pCharacter->GetComponent<HealthComponent>()->GetCurrHealth();
			TextRenderer::GetInstance()->DrawText(m_pHUDFont, L"Final Score: ", DirectX::XMFLOAT2(windowWH.x / 2 - 150, windowWH.y / 2 - 120), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White));
			TextRenderer::GetInstance()->DrawText(m_pHUDFont, std::to_wstring(m_pCharacter->GetScore()), DirectX::XMFLOAT2(windowWH.x / 2 + 180, windowWH.y / 2 - 120), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White));
		}
	}
}

void ExamGameScene::WinMenuUpdate()
{
	if (GetGameContext().pInput->IsActionTriggered((int)MenuControls::Up))
	{
		if (m_SelectedMenuOption == 0)
			m_SelectedMenuOption = m_WinMenuOptions.size() - 1;
		else m_SelectedMenuOption--;
	}
	else if (GetGameContext().pInput->IsActionTriggered((int)MenuControls::Down))
	{
		if (m_SelectedMenuOption == (int)m_WinMenuOptions.size() - 1)
			m_SelectedMenuOption = 0;
		else m_SelectedMenuOption++;
	}
	else if (GetGameContext().pInput->IsActionTriggered((int)MenuControls::Select))
	{
		if (m_SelectedMenuOption == 0)
		{
			RestartLevel();
			m_SelectedMenuOption = 0;
			//post proc remove
			if (m_pPostProcDeath)
			{
				SceneManager::GetInstance()->GetActiveScene()->RemovePostProcessingEffect(m_pPostProcDeath);
				m_pPostProcDeath = nullptr;
			}
		}
		else if (m_SelectedMenuOption == 1)
		{
			RestartLevel();
			SceneManager::GetInstance()->SetActiveGameScene(L"ExamGameMainMenuScene");
			m_SelectedMenuOption = 0;
			//post proc remove
			if (m_pPostProcDeath)
			{
				SceneManager::GetInstance()->GetActiveScene()->RemovePostProcessingEffect(m_pPostProcDeath);
				m_pPostProcDeath = nullptr;
			}
		}
	}
}

void ExamGameScene::AddPickUp(Pickup* pickup)
{
	m_pPickUps.push_back(pickup);
	AddChild(pickup);
}

void ExamGameScene::RestartLevel()
{
	//----------------Clearing, deleting gameobjects from prev level---------------
	for (auto& enemy : m_pEnemies)
	{
		enemy->SetDeleteLater(true);
	}
	for (auto& pickUp : m_pPickUps)
	{
		pickUp->SetDeleteLater(true);
	}
	for (auto& entity : m_pEntities)
	{
		entity->SetDeleteLater(true);
	}
	m_pEnemies.clear();
	m_pEntities.clear();
	m_pPickUps.clear();
	//-----------------------------------------------------------------PLAYER, INTERACTABLES AND ENEMIES-----------------------------------------------------
	m_pCharacter->Restart();
	//
	EnemiesInit();
	CallBackLambdaInit();
	EntitiesAndPickUpsInit(true);
	//Pause settings reset
	m_IsGamePaused = false;
	m_PauseMenuPng->SetIsVisible(false);
	m_GameOverMenuPng->SetIsVisible(false);
	m_WinMenuPng->SetIsVisible(false);
}

void ExamGameScene::PauseOrUnPauseAllTheObjects(bool isPause)
{
	if (isPause)
	{
		m_pCharacter->SetIsGamePaused(true);
		for (auto& enemy : m_pEnemies)
		{
			enemy->SetIsGamePaused(true);
		}
		for (auto& entity : m_pEntities)
		{
			entity->SetIsGamePaused(true);
		}
		for (auto& pickup : m_pPickUps)
		{
			pickup->SetIsGamePaused(true);
		}
	}
	else
	{
		m_pCharacter->SetIsGamePaused(false);
		for (auto& enemy : m_pEnemies)
		{
			enemy->SetIsGamePaused(false);
		}
		for (auto& entity : m_pEntities)
		{
			entity->SetIsGamePaused(false);
		}
		for (auto& pickup : m_pPickUps)
		{
			pickup->SetIsGamePaused(false);
		}
	}
}

void ExamGameScene::DrawHUD()
{
	if (m_pHUDFont->GetFontName() != L"")
	{
		int healthHUD = m_pCharacter->GetComponent<HealthComponent>()->GetCurrHealth();
		//
		TextRenderer::GetInstance()->DrawText(m_pHUDFont, L"Health: ", DirectX::XMFLOAT2(10, 10), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White));
		TextRenderer::GetInstance()->DrawText(m_pHUDFont, std::to_wstring(healthHUD), DirectX::XMFLOAT2(200, 10), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White));
		//
		TextRenderer::GetInstance()->DrawText(m_pHUDFont, L"Score: ", DirectX::XMFLOAT2(10, 40), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White));
		TextRenderer::GetInstance()->DrawText(m_pHUDFont, std::to_wstring(m_pCharacter->GetScore()), DirectX::XMFLOAT2(200, 40), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White));
		//
		TextRenderer::GetInstance()->DrawText(m_pHUDFont, L"Throwables: ", DirectX::XMFLOAT2(10, 70), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White));
		TextRenderer::GetInstance()->DrawText(m_pHUDFont, std::to_wstring(m_pCharacter->GetAmountOfThrowables()), DirectX::XMFLOAT2(200, 70), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White));
	}
}

void ExamGameScene::PostProcessingUpdate()
{
	//post proc
	if (m_pPostProcGreenPulse)
	{
		m_pPostProcGreenPulse->SetElapsedTime(GetGameContext().pGameTime->GetElapsed());
		m_GreenPulseCurrentTime += m_GameContext.pGameTime->GetElapsed();
		if (m_GreenPulseCurrentTime >= m_PostProcPulseMaxTime)
		{
			m_GreenPulseCurrentTime = 0.0f;
			SceneManager::GetInstance()->GetActiveScene()->RemovePostProcessingEffect(m_pPostProcGreenPulse);
			m_pPostProcGreenPulse = nullptr;
		}
	}
	//score
	if (m_pPostProcYellowPulse)
	{
		m_pPostProcYellowPulse->SetElapsedTime(GetGameContext().pGameTime->GetElapsed());
		m_YellowPulseCurrentTime += m_GameContext.pGameTime->GetElapsed();
		if (m_YellowPulseCurrentTime >= m_PostProcPulseMaxTime)
		{
			m_YellowPulseCurrentTime = 0.0f;
			SceneManager::GetInstance()->GetActiveScene()->RemovePostProcessingEffect(m_pPostProcYellowPulse);
			m_pPostProcYellowPulse = nullptr;
		}
	}
	//throwable
	if (m_pPostProcBluePulse)
	{
		m_pPostProcBluePulse->SetElapsedTime(GetGameContext().pGameTime->GetElapsed());
		m_BluePulseCurrentTime += m_GameContext.pGameTime->GetElapsed();
		if (m_BluePulseCurrentTime >= m_PostProcPulseMaxTime)
		{
			m_BluePulseCurrentTime = 0.0f;
			SceneManager::GetInstance()->GetActiveScene()->RemovePostProcessingEffect(m_pPostProcBluePulse);
			m_pPostProcBluePulse = nullptr;
		}
	}
	//death PostProc
	if (m_pPostProcDeath)
	{
		m_pPostProcDeath->SetElapsedTime(GetGameContext().pGameTime->GetElapsed());
	}
	if (m_pCharacter->GetIsDead() && !m_pPostProcDeath)
	{
		m_pPostProcDeath = new PostProcessingDeath();
		SceneManager::GetInstance()->GetActiveScene()->AddPostProcessingEffect(m_pPostProcDeath);
	}
}