#pragma once
#include "GameObject.h"

#include "ModelComponent.h"
#include "ModelAnimator.h"
#include "../OverlordProject/Materials/SkinnedDiffuseMaterial.h"
#include "../OverlordProject/CourseObjects/Game/Throwable.h"

class ControllerComponent;
class CameraComponent;

class Character : public GameObject
{
public:
	enum CharacterMovement : UINT
	{
		LEFT = 0,
		RIGHT,
		FORWARD,
		BACKWARD,
		JUMP
	};

	enum CharacterBehavior : UINT
	{
		ATTACK = 99,
		THROW = 98
	};

	Character(float radius = 2, float height = 5, float moveSpeed = 100);
	virtual ~Character() = default;

	Character(const Character& other) = delete;
	Character(Character&& other) noexcept = delete;
	Character& operator=(const Character& other) = delete;
	Character& operator=(Character&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;

	void SetIsDead(bool isDead) { m_IsDead = isDead; }
	bool GetIsDead() const { return m_IsDead; }

	void SetForwardAndRightDirections(const DirectX::XMFLOAT3& forward, const DirectX::XMFLOAT3& right);
	bool IsAttacking() const;
	void GetDamage(int amountOfDamage);
	void Restart();

	void AddHealth(int amountOfHealth);
	void AddScore(int amountOfScore);
	void AddThrowable(int amountOfThrowables);
	int GetScore() const;
	int GetAmountOfThrowables() const;
protected:
	void Attack(const GameContext& gameContext);
	void Throw(const GameContext& gameContext);

	ControllerComponent* m_pController;

	bool m_IsJumpTriggered = false;
	float m_TotalPitch, m_TotalYaw;
	float m_MoveSpeed, m_RotationSpeed;
	float m_Radius, m_Height;

	//Running
	float m_MaxRunVelocity,
		m_TerminalVelocity,
		m_Gravity,
		m_RunAccelerationTime,
		m_JumpAccelerationTime,
		m_RunAcceleration,
		m_JumpAcceleration,
		m_RunVelocity,
		m_JumpVelocity;

	bool m_IsDead;
	int m_AttackRunVelocity;

	DirectX::XMFLOAT3 m_Velocity;
	//animations (model)
	ModelComponent* m_pModel;
	int m_AnimationIndex = 0;
	DirectX::XMFLOAT3 m_Forward{};
	DirectX::XMFLOAT3 m_Right{};
	//
	int m_AmountOfThrowables;
	int m_CurrentScore;
};
