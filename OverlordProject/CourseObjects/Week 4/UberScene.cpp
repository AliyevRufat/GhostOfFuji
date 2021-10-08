#include "stdafx.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "GameObject.h"
#include "ContentManager.h"
#include "Components.h"
#include "UberScene.h"
#include "../../Materials/UberMaterial.h"

UberScene::UberScene() : GameScene(L"UberScene")
{
}

void UberScene::Initialize()
{
	const auto gameContext = GetGameContext();

	GetPhysxProxy()->EnablePhysxDebugRendering(true);
	//GROUND PLANE
	//************
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto pBouncyMaterial = physX->createMaterial(0, 0, 1);
	auto pGround = new GameObject();
	pGround->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<physx::PxGeometry> geom(new physx::PxPlaneGeometry());
	pGround->AddComponent(new ColliderComponent(geom, *pBouncyMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1)))));
	AddChild(pGround);

	//TEAPOT using the Uber Shader
	m_pTeapot = new GameObject();

	auto pModel = new ModelComponent(L"./Resources/Meshes/Teapot.ovm");

	UberMaterial* pMat = new UberMaterial();

	pMat->SetDiffuseTexture(L"./Resources/Textures/Wall_DiffuseMap.jpg");
	pMat->SetSpecularLevelTexture(L"./Resources/Textures/Specular_Level.jpg");
	pMat->SetNormalMapTexture(L"./Resources/Textures/Wall_NormalMap.jpg");
	pMat->SetEnvironmentCube(L"./Resources/Textures/Sunol_Cubemap.dds");
	pMat->SetOpacityTexture(L"./Resources/Textures/Ice_Opacity.jpg");
	//
	pMat->SetLightDirection(DirectX::XMFLOAT3(-0.577f, -0.577f, 0.577f));
	//
	pMat->EnableDiffuseTexture(false);
	pMat->SetDiffuseColor(DirectX::XMFLOAT4(0, 0, 0, 1));
	//
	pMat->SetSpecularColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	pMat->EnableSpecularLevelTexture(true);
	pMat->SetShininess(50);
	//
	pMat->SetAmbientColor(DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f));
	pMat->SetAmbientIntensity(0.0f);
	//
	pMat->FlipNormalGreenCHannel(false);
	pMat->EnableNormalMapping(false);
	//
	pMat->EnableEnvironmentMapping(true);
	pMat->SetReflectionStrength(1);
	pMat->SetRefractionStrength(0.1f);
	pMat->SetRefractionIndex(0.8f);
	//
	pMat->SetOpacity(1.0f);
	pMat->EnableOpacityMap(false);
	//
	pMat->EnableSpecularBlinn(false);
	pMat->EnableSpecularPhong(false);
	//
	pMat->EnableFresnelFaloff(false);
	pMat->SetFresnelColor(DirectX::XMFLOAT4(0.5f, 0.3f, 0.9f, 1));
	pMat->SetFresnelPower(20);
	pMat->SetFresnelMultiplier(20);
	pMat->SetFresnelHardness(10);

	//
	gameContext.pMaterialManager->AddMaterial(pMat, 62);
	pModel->SetMaterial(62);

	// Build and Run
	m_pTeapot->GetTransform()->Translate(0, 10, 0);
	//m_pTeapot->AddComponent(pRb);
	//m_pTeapot->AddComponent(pCollider);
	m_pTeapot->AddComponent(pModel);
	AddChild(m_pTeapot);
}

void UberScene::Update()
{
}

void UberScene::Draw()
{
}