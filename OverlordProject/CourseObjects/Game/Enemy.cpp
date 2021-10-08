#include "stdafx.h"
#include "Enemy.h"
#include "Components.h"
#include "Prefabs.h"
#include "HealthComponent.h"
#include "ModelAnimator.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "../OverlordProject/AudioManager.h"
#include <cmath>
# define M_PI           3.14159265358979323846  /* pi */

Enemy::Enemy(DirectX::XMFLOAT3 position, const std::wstring& modelPath, int textureId, int moveSpeed, Character* player)
	: m_pModel{ nullptr }
	, m_PatrolSemiAmplitude{ 10 }
	, m_PatrolSpeed{ 10 }
	, m_StartPos{ position }
	, m_ModelPath{ modelPath }
	, m_ModelMatId{ textureId }
	, m_TargetPos{}
	, m_PlayerPos{}
	, m_IsPlayerSpotted{ false }
	, m_MoveSpeed{ moveSpeed }
	, m_InitialMoveSpeed{ moveSpeed }
	, m_AttackPlayer{ false }
	, m_HitDamage{ 10 }
	, m_MaxTimeForAttack{ 2.0f }
	, m_CurrentAttackTime{ 0.0f }
	, m_IsCollidingPlayer{ false }
	, m_IsDead{ false }
	, m_TriggerDeath{ false }
	, m_IsAttackable{ false }
	, m_pPlayer{ player }
{
}

void Enemy::Initialize(const GameContext&)
{
	//controllercomponent
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	physx::PxMaterial* bouncyMaterial = physX->createMaterial(0, 0, 1);
	//rb
	RigidBodyComponent* rb = new RigidBodyComponent();
	rb->SetKinematic(true);
	AddComponent(rb);
	//
	std::shared_ptr<physx::PxGeometry> geom(new physx::PxBoxGeometry(3, 3, 3));
	AddComponent(new ColliderComponent(geom, *bouncyMaterial, physx::PxTransform(physx::PxQuat(0, physx::PxVec3(0, 0, 1)))));

	SetTag(L"Enemy");
	GetTransform()->Translate(m_StartPos);
	//

	m_pModel = new ModelComponent(m_ModelPath);
	m_pModel->SetMaterial(m_ModelMatId);
	auto obj = new GameObject();
	obj->AddComponent(m_pModel);
	AddChild(obj);

	obj->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	obj->GetTransform()->Translate(0, 0, 0);

	AddComponent(new HealthComponent(100));
}

void Enemy::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//animatiosn
	m_pModel->GetAnimator()->SetAnimation(0);
	m_pModel->GetAnimator()->Play();
}

void Enemy::Update(const GameContext& gameContext)
{
	if (m_IsDead)
	{
		if (m_TriggerDeath && !m_pModel->GetAnimator()->IsCurrentAnimationNumber(5))
		{
			SetDeleteLater(true);
		}
		//
		if (!m_TriggerDeath && !m_pModel->GetAnimator()->IsCurrentAnimationNumber(5))
		{
			m_pModel->GetAnimator()->SetAnimation(5);
			m_TriggerDeath = true;
		}
		return;
	}
	//
	m_CurrentAttackTime += gameContext.pGameTime->GetElapsed();
	m_IsPlayerSpotted = IsPlayerSpotted();
	if (!m_pModel->GetAnimator()->IsCurrentAnimationNumber(4)) //if not attacking
	{
		//movement
		if (!m_IsPlayerSpotted)
		{
			Patrol();
		}
		else
		{
			m_TargetPos = m_PlayerPos;
		}

		GoToTargetPos(gameContext.pGameTime->GetElapsed());

		//move animation
		if (!m_pModel->GetAnimator()->IsCurrentAnimationNumber(1))
		{
			m_pModel->GetAnimator()->SetAnimation(1);
		}
	}

	//rotation to target pos
	m_pModel->GetTransform()->Rotate(0, atan2(this->GetTransform()->GetPosition().z - m_TargetPos.z, this->GetTransform()->GetPosition().x - m_TargetPos.x) * (-180.f / (float)M_PI) - 270.0f, 0);

	//
	AttackBehaviour();
}

void Enemy::GoToTargetPos(float elapsedTime)
{
	using namespace DirectX;
	DirectX::XMFLOAT3 desiredDir;
	m_TargetPos.y = 0;
	DirectX::XMStoreFloat3(&desiredDir, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&m_TargetPos) - DirectX::XMLoadFloat3(&GetTransform()->GetPosition())) * elapsedTime * (float)m_MoveSpeed);
	DirectX::XMFLOAT3 movePosWithOffset;
	DirectX::XMStoreFloat3(&movePosWithOffset, DirectX::XMLoadFloat3(&GetTransform()->GetPosition()) + DirectX::XMLoadFloat3(&desiredDir));
	GetTransform()->Translate(movePosWithOffset);
}

void Enemy::Patrol()
{
	if (m_MovingRight)
	{
		if (GetTransform()->GetPosition().x > (m_StartPos.x - m_PatrolSemiAmplitude))
		{
			m_TargetPos = m_StartPos;
			m_TargetPos.x = m_StartPos.x - m_PatrolSemiAmplitude;
		}
		else m_MovingRight = false;
	}
	else
	{
		if (GetTransform()->GetPosition().x < (m_StartPos.x + m_PatrolSemiAmplitude))
		{
			m_TargetPos = m_StartPos;
			m_TargetPos.x = m_StartPos.x + m_PatrolSemiAmplitude;
		}
		else m_MovingRight = true;
	}
}

bool Enemy::IsPlayerSpotted()
{
	const int distance = 40;
	if (abs(m_PlayerPos.x - GetTransform()->GetPosition().x) <= distance && abs(m_PlayerPos.z - GetTransform()->GetPosition().z) <= distance)
	{
		return true;
	}
	return false;
}

void Enemy::AttackBehaviour()
{
	const int distance = 5;
	if (abs(m_PlayerPos.x - GetTransform()->GetPosition().x) <= distance && abs(m_PlayerPos.z - GetTransform()->GetPosition().z) <= distance)
	{
		m_MoveSpeed = 0;
		//attack animation
		if (m_CurrentAttackTime >= m_MaxTimeForAttack)
		{
			if (!m_pModel->GetAnimator()->IsCurrentAnimationNumber(4))
			{
				m_pModel->GetAnimator()->SetAnimation(4);
			}
			const float offsetTime = 0.7f;
			if (m_CurrentAttackTime >= m_MaxTimeForAttack + offsetTime)
			{
				m_pPlayer->GetDamage(m_HitDamage);
				m_CurrentAttackTime = 0;
			}
		}
	}
	else
	{
		m_MoveSpeed = m_InitialMoveSpeed;
	}
}

void Enemy::SetIsCollidingPlayer(bool isCollidingPlayer)
{
	m_IsCollidingPlayer = isCollidingPlayer;
}

bool Enemy::GetIsCollidingPlayer() const
{
	return m_IsCollidingPlayer;
}

bool Enemy::GetIsAttackable() const
{
	return m_IsAttackable;
}

void Enemy::SetIsAttackable(bool isAttackable)
{
	m_IsAttackable = isAttackable;
}

void Enemy::SetIsDead(bool isDead)
{
	m_IsDead = isDead;
	if (isDead)
	{
		AudioManager::GetInstance()->Play(AudioManager::SoundId::EnemyDeath);
		const int score = 100;
		m_pPlayer->AddScore(score);
	}
}

bool Enemy::GetIsDead() const
{
	return m_IsDead;
}