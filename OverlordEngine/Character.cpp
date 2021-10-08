#include "stdafx.h"
#include "Character.h"
#include "Components.h"
#include "Prefabs.h"
#include "GameScene.h"
#include "PhysxManager.h"
#include "SceneManager.h"
#include "PhysxProxy.h"
#include "Logger.h"
#include "HealthComponent.h"
#include "../OverlordProject/AudioManager.h"
#include <cmath>
# define M_PI           3.14159265358979323846  /* pi */

Character::Character(float radius, float height, float moveSpeed) :
	m_Radius(radius),
	m_Height(height),
	m_MoveSpeed(moveSpeed),
	m_pController(nullptr),
	m_TotalPitch(0),
	m_TotalYaw(0),
	m_RotationSpeed(90.f),
	//Running
	m_MaxRunVelocity(50.0f),
	m_TerminalVelocity(20),
	m_Gravity(9.81f),
	m_RunAccelerationTime(0.3f),
	m_JumpAccelerationTime(0.5f),
	m_RunAcceleration(m_MaxRunVelocity / m_RunAccelerationTime),
	m_JumpAcceleration(m_Gravity / m_JumpAccelerationTime),
	m_RunVelocity(0),
	m_JumpVelocity(0),
	m_Velocity(0, 0, 0),
	m_IsDead{ false },
	m_IsJumpTriggered{ false },
	m_pModel{ nullptr },
	m_Forward{},
	m_Right{},
	m_AttackRunVelocity{ 3 },
	m_AmountOfThrowables{ 3 },
	m_CurrentScore{ 0 }
{}

void Character::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//Animations (model)

	m_pModel = new ModelComponent(L"Resources/Animations/Ovm/Ninja.ovm");
	m_pModel->SetMaterial(7);
	auto obj = new GameObject();
	obj->AddComponent(m_pModel);
	AddChild(obj);

	obj->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	obj->GetTransform()->Translate(0, -5, 0);
	//-----------------
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	physx::PxMaterial* bouncyMaterial = physX->createMaterial(0, 0, 1);

	//TODO: Create controller
	m_pController = new ControllerComponent(bouncyMaterial);
	AddComponent(m_pController);

	//TODO: Register all Input Actions
	gameContext.pInput->AddInputAction(InputAction((int)CharacterBehavior::ATTACK, InputTriggerState::Pressed, 'Q'));
	gameContext.pInput->AddInputAction(InputAction((int)CharacterBehavior::THROW, InputTriggerState::Pressed, 'E'));
	gameContext.pInput->AddInputAction(InputAction((int)CharacterMovement::JUMP, InputTriggerState::Down, VK_SPACE));
}

void Character::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//animatiosn
	m_pModel->GetAnimator()->SetAnimation(0);
	m_pModel->GetAnimator()->Play();
	//camera
}

void Character::SetForwardAndRightDirections(const DirectX::XMFLOAT3& forward, const DirectX::XMFLOAT3& right)
{
	m_Forward = forward;
	m_Right = right;
}

void Character::Update(const GameContext& gameContext)
{
	if (m_IsDead)
	{
		return;
	}
	using namespace DirectX;
	//attack
	Attack(gameContext);
	Throw(gameContext);

	//HANDLE INPUT
	auto move = XMFLOAT2(0, 0);
	move.y = InputManager::IsKeyboardKeyDown('W') ? 1.0f : 0.0f;
	if (move.y == 0) move.y = -(InputManager::IsKeyboardKeyDown('S') ? 1.0f : 0.0f);
	if (move.y == 0) move.y = InputManager::GetThumbstickPosition().y;

	move.x = InputManager::IsKeyboardKeyDown('D') ? 1.0f : 0.0f;
	if (move.x == 0) move.x = -(InputManager::IsKeyboardKeyDown('A') ? 1.0f : 0.0f);
	if (move.x == 0) move.x = InputManager::GetThumbstickPosition().x;
	//set the correct animation based on movement
	if (move.y != 0 || move.x != 0)
	{
		m_pModel->GetTransform()->Rotate(0, atan2(m_Forward.z, m_Forward.x) * (-180.f / (float)M_PI) - 90.0f + (atan2(move.y, -move.x) * (180.f / (float)M_PI) - 90.0f), 0);
		//m_pModel->GetGameObject()->GetTransform()->Rotate()
		if (!m_pModel->GetAnimator()->IsCurrentAnimationNumber(3) && !m_pModel->GetAnimator()->IsCurrentAnimationNumber(2) && !m_pModel->GetAnimator()->IsCurrentAnimationNumber(4) && !m_pModel->GetAnimator()->IsCurrentAnimationNumber(1))
		{
			m_pModel->GetAnimator()->SetAnimation(1);
		}
	}
	else
	{
		if (!m_pModel->GetAnimator()->IsCurrentAnimationNumber(3) && !m_pModel->GetAnimator()->IsCurrentAnimationNumber(2) && !m_pModel->GetAnimator()->IsCurrentAnimationNumber(4) && !m_pModel->GetAnimator()->IsCurrentAnimationNumber(0))
		{
			m_pModel->GetAnimator()->SetAnimation(0);
		}
	}

	//change model rotation based on the movement

	auto currSpeed = m_MoveSpeed;
	if (InputManager::IsKeyboardKeyDown(VK_LSHIFT))
		currSpeed *= 2;

	auto look = XMFLOAT2(0, 0);

	//CALCULATE TRANSFORMS
	if (move.x != 0 || move.y != 0)
	{
		XMVECTOR forwardMovement = DirectX::XMLoadFloat3(&m_Forward) * move.y;
		XMVECTOR rightMovement = DirectX::XMLoadFloat3(&m_Right) * move.x;

		auto movementDirectionVector = forwardMovement + rightMovement;
		movementDirectionVector = XMVector3Normalize(movementDirectionVector);

		m_RunVelocity += m_RunAcceleration * gameContext.pGameTime->GetElapsed();

		if (m_RunVelocity > m_MaxRunVelocity)
		{
			if (!m_pModel->GetAnimator()->IsCurrentAnimationNumber(3) && m_pModel->GetAnimator()->IsCurrentAnimationNumber(4))
			{
				m_RunVelocity = (float)m_AttackRunVelocity;
			}
			else
			{
				m_RunVelocity = m_MaxRunVelocity;
			}
		}

		XMVECTOR velocityVector = movementDirectionVector * m_RunVelocity;

		float velocityY = m_Velocity.y;
		DirectX::XMStoreFloat3(&m_Velocity, velocityVector);
		m_Velocity.y = velocityY;
	}
	else
	{
		m_Velocity.x = 0;
		m_Velocity.z = 0;
	}

	if (m_pController->GetCollisionFlags() != physx::PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		m_JumpVelocity -= m_JumpAcceleration * gameContext.pGameTime->GetElapsed();

		Clamp(m_Velocity.y, m_TerminalVelocity, 0.f);
	}
	else if (!m_IsJumpTriggered && gameContext.pInput->IsActionTriggered((int)CharacterMovement::JUMP))
	{
		m_IsJumpTriggered = true;
		m_JumpVelocity = 0;
		if (!m_pModel->GetAnimator()->IsCurrentAnimationNumber(2))
		{
			AudioManager::GetInstance()->Play(AudioManager::SoundId::PlayerJump);
			m_pModel->GetAnimator()->SetAnimation(2);
		}
	}
	else
	{
		m_Velocity.y = 0;
		m_IsJumpTriggered = false;
	}

	if (m_IsJumpTriggered)
	{
		const int height = 100;
		m_Velocity.y += gameContext.pGameTime->GetElapsed() * height;
	}

	m_Velocity.y += m_JumpVelocity;

	//
	XMFLOAT3 currPos = XMFLOAT3(GetTransform()->GetPosition().x, GetTransform()->GetPosition().y, GetTransform()->GetPosition().z);

	currPos.x = m_Velocity.x * gameContext.pGameTime->GetElapsed();
	currPos.y = m_Velocity.y * gameContext.pGameTime->GetElapsed();
	currPos.z = m_Velocity.z * gameContext.pGameTime->GetElapsed();

	m_TotalYaw += look.x * m_RotationSpeed * gameContext.pGameTime->GetElapsed();
	m_TotalPitch += look.y * m_RotationSpeed * gameContext.pGameTime->GetElapsed();

	GetTransform()->Rotate(m_TotalPitch, m_TotalYaw, 0);
	m_pController->Move(currPos);
}

void Character::Attack(const GameContext& gameContext)
{
	if (gameContext.pInput->IsActionTriggered((int)CharacterBehavior::ATTACK))
	{
		//Attack
		if (!m_pModel->GetAnimator()->IsCurrentAnimationNumber(4))
		{
			AudioManager::GetInstance()->Play(AudioManager::SoundId::PlayerAttack);
			m_pModel->GetAnimator()->SetAnimation(4);
		}
	}
}

void Character::Throw(const GameContext& gameContext)
{
	if (m_AmountOfThrowables <= 0)
	{
		return;
	}
	if (gameContext.pInput->IsActionTriggered((int)CharacterBehavior::THROW))
	{
		//Throw
		if (!m_pModel->GetAnimator()->IsCurrentAnimationNumber(3))
		{
			AudioManager::GetInstance()->Play(AudioManager::SoundId::PlayerThrow);
			m_pModel->GetAnimator()->SetAnimation(3);
			DirectX::XMFLOAT3 pos = GetTransform()->GetPosition();
			Throwable* throwable = new Throwable(pos, m_pModel->GetTransform()->GetForward());
			SceneManager::GetInstance()->GetActiveScene()->AddChild(throwable);
			--m_AmountOfThrowables;
		}
	}
}

bool Character::IsAttacking() const
{
	if (m_pModel->GetAnimator()->IsCurrentAnimationNumber(4))
	{
		return true;
	}
	return false;
}

void Character::GetDamage(int amountOfDamage)
{
	GetComponent<HealthComponent>()->Damage(amountOfDamage);

	if (m_IsDead)
	{
		m_pModel->GetAnimator()->SetAnimation(5);
	}
}

void Character::Restart()
{
	m_pModel->GetAnimator()->SetAnimation(0);
	GetComponent<HealthComponent>()->AddHealth(100);
	DirectX::XMFLOAT3 startPos = DirectX::XMFLOAT3(0, 5, 0);
	m_pController->Move(DirectX::XMFLOAT3(startPos.x - GetTransform()->GetPosition().x, startPos.y - GetTransform()->GetPosition().y, startPos.z - GetTransform()->GetPosition().z));
	m_IsDead = false;
	m_CurrentScore = 0;
	m_AmountOfThrowables = 3;
	SetIsGamePaused(false);
}

void Character::AddThrowable(int amountOfThrowables)
{
	m_AmountOfThrowables += amountOfThrowables;
}

void Character::AddHealth(int amountOfHealth)
{
	GetComponent<HealthComponent>()->AddHealth(amountOfHealth);
}

void Character::AddScore(int amountOfScore)
{
	m_CurrentScore += amountOfScore;
}

int Character::GetScore() const
{
	return m_CurrentScore;
}

int Character::GetAmountOfThrowables() const
{
	return m_AmountOfThrowables;
}