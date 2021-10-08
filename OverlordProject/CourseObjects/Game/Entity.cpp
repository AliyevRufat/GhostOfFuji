#include "stdafx.h"
#include "Entity.h"
#include "Components.h"
#include "Prefabs.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "HealthComponent.h"
#include "PickUp.h"
#include "SceneManager.h"
#include "ExamGameScene.h"
#include "../OverlordProject/Materials/DiffuseMaterial.h"

Entity::Entity(const std::wstring& modelPath, const int& matIndex, bool isDestroyable, bool hasCollider, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& geomScale)
	: m_ModelPath{ modelPath }
	, m_IsDestroyable{ isDestroyable }
	, m_IsCollidingPlayer{ false }
	, m_IsAttackable{ false }
	, m_Health{ 100.0f }
	, m_MatIndex{ matIndex }
	, m_GeomScale{ DirectX::XMFLOAT3(geomScale.x * scale.x,geomScale.y * scale.y,geomScale.z * scale.z) }
	, m_Rotation{ rot }
	, m_IsDestroyed{ false }
	, m_HasCollider{ hasCollider }
{
	TransformComponent* transform = GetComponent<TransformComponent>();
	transform->Scale(scale);
	GetTransform()->Translate(pos);
}

void Entity::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	if (m_IsDestroyable) //TODO : which one should it be, don't do both
	{
		SetTag(L"DestroyableEntity");
	}
	else
	{
		SetTag(L"NonDestroyableEntity");
	}

	auto model = new ModelComponent(m_ModelPath);

	model->SetMaterial(m_MatIndex);

	AddComponent(model);
	auto bouncyMaterial = PhysxManager::GetInstance()->GetPhysics()->createMaterial(0, 0, 1);
	if (m_HasCollider)
	{
		auto rigidBody = new RigidBodyComponent(false);
		rigidBody->SetKinematic(true);
		AddComponent(rigidBody);

		std::shared_ptr<physx::PxGeometry> geom(new physx::PxBoxGeometry(m_GeomScale.x, m_GeomScale.y, m_GeomScale.z));
		AddComponent(new ColliderComponent(geom, *bouncyMaterial, physx::PxTransform(physx::PxQuat(0, physx::PxVec3(0, 0, 0)))));
	}

	AddComponent(new HealthComponent((int)m_Health));
}

void Entity::PostInitialize(const GameContext&)
{
	GetComponent<TransformComponent>()->Rotate(m_Rotation, true);
}

void Entity::Update(const GameContext&)
{
	if (m_IsDestroyed)
	{
		OnDestroy();
		m_IsDestroyed = false;
	}
}

bool Entity::GetIsCollidingPlayer()const
{
	return m_IsCollidingPlayer;
}

void Entity::SetIsCollidingPlayer(bool isCollidingPlayer)
{
	m_IsCollidingPlayer = isCollidingPlayer;
}

bool Entity::GetIsAttackable() const
{
	return m_IsAttackable;
}

void Entity::SetIsAttackable(bool isAttackable)
{
	m_IsAttackable = isAttackable;
}

void Entity::SetIsDestroyed(bool isDestroyed)
{
	m_IsDestroyed = isDestroyed;
}

void Entity::OnDestroy()
{
	int randNr = (rand() % 3) + 1;
	Pickup::PickupType pickupType;
	Pickup* pickUp;
	if (randNr == 0)
	{
		pickupType = Pickup::PickupType::health;
		pickUp = new Pickup(L"Resources/Meshes/Medkit.ovm", 16, DirectX::XMFLOAT3(this->GetTransform()->GetPosition().x, this->GetTransform()->GetPosition().y + 3, this->GetTransform()->GetPosition().z), DirectX::XMFLOAT3(90, 0, 0), pickupType);
	}
	else if (randNr == 1)
	{
		pickupType = Pickup::PickupType::score;
		pickUp = new Pickup(L"Resources/Meshes/Coin.ovm", 17, DirectX::XMFLOAT3(this->GetTransform()->GetPosition().x, this->GetTransform()->GetPosition().y + 3, this->GetTransform()->GetPosition().z), DirectX::XMFLOAT3(90, 0, 0), pickupType);
	}
	else
	{
		pickupType = Pickup::PickupType::throwable;
		pickUp = new Pickup(L"Resources/Meshes/Throwable.ovm", 6, DirectX::XMFLOAT3(this->GetTransform()->GetPosition().x, this->GetTransform()->GetPosition().y + 3, this->GetTransform()->GetPosition().z), DirectX::XMFLOAT3(90, 0, 0), pickupType);
	}

	ExamGameScene* gameScene = static_cast<ExamGameScene*>(SceneManager::GetInstance()->GetActiveScene());
	pickUp->SetIsSpawned(true);
	gameScene->AddPickUp(pickUp);
}