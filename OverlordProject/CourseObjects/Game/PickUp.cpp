#include "stdafx.h"
#include "Pickup.h"
#include "TransformComponent.h"
#include "ModelComponent.h"
#include "ColliderComponent.h"
#include "RigidBodyComponent.h"
#include "PhysxManager.h"
#include "HealthComponent.h"
#include "GameScene.h"
#include "PhysxProxy.h"
#include "Character.h"
#include "../OverlordProject/CourseObjects/Game/PickUp.h"
#include "../OverlordProject/ParticleEffectManager.h"

Pickup::Pickup(const std::wstring& modelPath, int diffuseMatId, const DirectX::XMFLOAT3& startLocation, const DirectX::XMFLOAT3& startRotation, PickupType pickupType)
	: m_RotationSpeed{ 60 }
	, m_CurrentRotation{ startRotation }
	, m_HealAmount{ 40 }
	, m_IsSpawned{ false }
	, m_CurrentMoveTime{ 0.0f }
	, m_MaxTimeForMove{ 2.0f }
	, m_ScoreAmount{ 50 }
	, m_PickupType{ pickupType }
{
	auto model = new ModelComponent(modelPath);
	model->SetMaterial(diffuseMatId);
	AddComponent(model);

	auto bouncyMaterial = PhysxManager::GetInstance()->GetPhysics()->createMaterial(0, 0, 1);
	AddComponent(new RigidBodyComponent(false));
	GetComponent<RigidBodyComponent>()->SetKinematic(true);
	SetTag(L"Pickup");

	std::shared_ptr<physx::PxGeometry> geom(new physx::PxBoxGeometry(0.5f, 0.5f, 0.5f));
	AddComponent(new ColliderComponent(geom, *bouncyMaterial, physx::PxTransform(physx::PxQuat(0, physx::PxVec3(0, 0, 1)))));

	GetTransform()->Translate(startLocation);
}

void Pickup::Initialize(const GameContext&)
{
};
void Pickup::PostInitialize(const GameContext&)
{
};

void Pickup::Update(const GameContext& gameContext)
{
	float deltaT = gameContext.pGameTime->GetElapsed();
	this;
	m_CurrentRotation.y += deltaT * m_RotationSpeed;

	GetTransform()->Rotate(m_CurrentRotation);
	//gravity
	const int groundOffset = 2;
	if (GetTransform()->GetPosition().y >= groundOffset)
	{
		const float gravitySpeed = 8.0f;
		GetTransform()->Translate(GetTransform()->GetPosition().x, GetTransform()->GetPosition().y - gravitySpeed * gameContext.pGameTime->GetElapsed(), GetTransform()->GetPosition().z);
	}
	if (m_IsSpawned)
	{
		OnSpawn(gameContext.pGameTime->GetElapsed());
		m_CurrentMoveTime += gameContext.pGameTime->GetElapsed();
		if (m_CurrentMoveTime >= m_MaxTimeForMove)
		{
			m_IsSpawned = false;
		}
	}
};

void Pickup::OnPickUp(Character* receiver)
{
	switch (m_PickupType)
	{
	case Pickup::PickupType::health:
		receiver->AddHealth((int)m_HealAmount);
		break;
	case Pickup::PickupType::score:
		receiver->AddScore(m_ScoreAmount);
		break;
	case Pickup::PickupType::throwable:
		receiver->AddThrowable(1);
		break;
	}
	SetDeleteLater(true);
}

void Pickup::OnSpawn(float)
{
	const int moveSpeed = 10;
	//go to rand direction
	using namespace DirectX;
	DirectX::XMFLOAT3 desiredDir = DirectX::XMFLOAT3(GetTransform()->GetPosition().x, GetTransform()->GetPosition().y, GetTransform()->GetPosition().z);
	DirectX::XMFLOAT3 movePosWithOffset;
	DirectX::XMStoreFloat3(&movePosWithOffset, DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&GetTransform()->GetPosition()), DirectX::XMLoadFloat3(&desiredDir), 0.05f));
	GetTransform()->Translate(movePosWithOffset);
}

void Pickup::SetIsSpawned(bool isSpawned)
{
	m_IsSpawned = isSpawned;
	m_CurrentMoveTime = 0.0f;
}

Pickup::PickupType Pickup::GetPickupType()
{
	return m_PickupType;
}