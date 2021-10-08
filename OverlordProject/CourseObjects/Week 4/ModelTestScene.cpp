#include "stdafx.h"

#include "ModelTestScene.h"
#include "GameObject.h"
#include "Components.h"
#include "PhysxProxy.h"
#include "ContentManager.h"
#include "PhysxManager.h"
#include "../../Materials/DiffuseMaterial.h"

ModelTestScene::ModelTestScene() : GameScene(L"ModelTestScene"),
m_pChair(nullptr)
{}

void ModelTestScene::Initialize()
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

	//CHAIR OBJECT
	//************
	m_pChair = new GameObject();

	//1. Attach a modelcomponent (chair.ovm)
	auto pModel = new ModelComponent(L"./Resources/Meshes/Chair.ovm");

	//2. Create a ColorMaterial and add it to the material manager
	DiffuseMaterial* pMat = new DiffuseMaterial();
	//gameContext.pMaterialManager->AddMaterial(pMat, 61);

	//3. Assign the material to the previous created modelcomponent
	//pModel->SetMaterial(61);
	// Build and Run

	//4. Create a DiffuseMaterial (using PosNormTex3D.fx)
	//		Make sure you are able to set a texture (DiffuseMaterial::SetDiffuseTexture(const wstring& assetFile))
	//		Load the correct shadervariable and set it during the material variable update
	pMat->SetDiffuseTexture(L"./Resources/Textures/Chair_Dark.dds");

	//5. Assign the material to the modelcomponent
	gameContext.pMaterialManager->AddMaterial(pMat, 61);
	pModel->SetMaterial(61);
	// Build and Run

	physx::PxPhysics* const pxPhysX = PhysxManager::GetInstance()->GetPhysics(); //create physX

	//6. Attach a rigidbody component (pure-dynamic)

	//pxPhysX->createRigidDynamic(physx::PxTransform::createIdentity());

	auto mat = pxPhysX->createMaterial(0.5f, 0.5f, 0.5f);
	RigidBodyComponent* pRb = new RigidBodyComponent();
	//pRb->SetDensity(5);
	physx::PxConvexMesh* pConvexMesh = ContentManager::Load<physx::PxConvexMesh>(L"./Resources/Meshes/Chair.ovpc");
	std::shared_ptr<physx::PxGeometry> geomMesh{ new physx::PxConvexMeshGeometry(pConvexMesh) };
	auto pCollider = new ColliderComponent(geomMesh, *mat);
	//7. Attach a collider component (Use a PxConvexMeshGeometry [chair.ovpc])

	////convex chair
	//PxRigidDynamic* const pConvexActor = pxPhysX->createRigidDynamic(PxTransform::createIdentity());
	//pConvexActor->createShape(PxConvexMeshGeometry(pConvexMesh), *pDefaultMaterial);
	//PxRigidBodyExt::updateMassAndInertia(*pConvexActor, 10.f);

	// Build and Run
	m_pChair->GetTransform()->Translate(0, 10, 0);
	m_pChair->AddComponent(pRb);
	m_pChair->AddComponent(pCollider);
	m_pChair->AddComponent(pModel);
	AddChild(m_pChair);
}

void ModelTestScene::Update()
{}

void ModelTestScene::Draw()
{}