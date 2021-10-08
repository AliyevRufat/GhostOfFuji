#include "stdafx.h"
#include "Throwable.h"
#include "PhysxManager.h"
#include "RigidBodyComponent.h"
#include "CubePrefab.h"
#include "ColliderComponent.h"
#include "TransformComponent.h"
#include "ModelComponent.h"
#include "HealthComponent.h"
#include "../OverlordProject/CourseObjects/Game/Enemy.h"
#include "../OverlordProject/CourseObjects/Game/Entity.h"

Throwable::Throwable(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir)
	: m_Pos{ pos }
	, m_Direction{ dir }
{
}

void Throwable::Initialize(const GameContext&)
{
	CallBackLambdaInit();

	auto model = new ModelComponent(L"Resources/Meshes/Throwable.ovm");

	model->SetMaterial(6);

	AddComponent(model);

	AddComponent(new HealthComponent(1));

	GetTransform()->Translate(m_Pos);
}

void Throwable::PostInitialize(const GameContext&)
{
}

void Throwable::Update(const GameContext& gameContext)
{
	const int throwSpeed = 100;
	using namespace DirectX;
	m_Direction.y = 0;
	DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&m_Direction));
	DirectX::XMFLOAT3 movePosWithOffset;
	DirectX::XMStoreFloat3(&movePosWithOffset, DirectX::XMLoadFloat3(&GetTransform()->GetPosition()) + DirectX::XMLoadFloat3(&m_Direction) * -1 * gameContext.pGameTime->GetElapsed() * throwSpeed);
	movePosWithOffset.y = 5;
	GetTransform()->Translate(movePosWithOffset);
	//
	m_CallBackTriggerCollider->GetTransform()->Translate(movePosWithOffset);
}

void Throwable::CallBackLambdaInit()
{
	using namespace DirectX;
	using namespace physx;
	PxPhysics* const pxPhysX = PhysxManager::GetInstance()->GetPhysics();
	PxMaterial* const pPhysicsMaterial = pxPhysX->createMaterial(0.f, 0.f, 1.f);

	auto characterCallBacks = [this](GameObject* trigger, GameObject* other, GameObject::TriggerAction action)
	{
		if (trigger->GetTag() == L"Throwable")
		{
			if (other->GetTag() == L"Enemy")
			{
				if (action == GameObject::TriggerAction::ENTER)
				{
					Enemy* enemy = static_cast<Enemy*>(other);
					if (enemy)
					{
						enemy->GetComponent<HealthComponent>()->Damage(100);
					}
					SetDeleteLater(true);
				}
			}
			else if (other->GetTag() == L"Environment" || other->GetTag() == L"NonDestroyableEntity")
			{
				if (action == GameObject::TriggerAction::ENTER)
				{
					SetDeleteLater(true);
				}
			}
		}
	};

	//gameobject with rigidbody component and collision component that acts as a trigger
	using namespace physx;
	std::shared_ptr<PxGeometry> capsuleGeometry(new PxCapsuleGeometry(2, 2));
	RigidBodyComponent* pRb = new RigidBodyComponent(false);

	m_CallBackTriggerCollider = new CubePrefab(0, 0, 0);
	m_CallBackTriggerCollider->SetTag(L"Throwable");
	m_CallBackTriggerCollider->AddComponent(pRb);
	m_CallBackTriggerCollider->GetTransform()->Translate(0, 0, 50);
	auto colliderCompCapsule = new ColliderComponent(capsuleGeometry, *pPhysicsMaterial);
	colliderCompCapsule->EnableTrigger(true);
	m_CallBackTriggerCollider->SetOnTriggerCallBack(characterCallBacks);
	m_CallBackTriggerCollider->AddComponent(colliderCompCapsule);
	AddChild(m_CallBackTriggerCollider);
}