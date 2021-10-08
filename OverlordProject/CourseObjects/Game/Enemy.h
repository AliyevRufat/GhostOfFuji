#pragma once
#include "GameObject.h"
#include "Character.h"
#include "ModelComponent.h"

class HealthComponent;
class Character;

class Enemy : public GameObject
{
public:

	Enemy(DirectX::XMFLOAT3 position, const std::wstring& modelPath, int textureId, int moveSpeed, Character* player);
	virtual ~Enemy() = default;

	Enemy(const Enemy& other) = delete;
	Enemy(Enemy&& other) noexcept = delete;
	Enemy& operator=(const Enemy& other) = delete;
	Enemy& operator=(Enemy&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void SetPlayerPos(DirectX::XMFLOAT3 pos) { m_PlayerPos = pos; }
	//
	bool GetIsCollidingPlayer() const;
	void SetIsCollidingPlayer(bool isCollidingPlayer);
	bool GetIsAttackable() const;
	void SetIsAttackable(bool isAttackable);
	void SetIsDead(bool isDead);
	bool GetIsDead() const;

private:

	void AttackBehaviour();
	void GoToTargetPos(float elapsedTime);
	void Patrol();
	bool IsPlayerSpotted();

	Character* m_pPlayer;
	std::wstring m_ModelPath;
	int m_ModelMatId;
	ModelComponent* m_pModel;
	DirectX::XMFLOAT3 m_StartPos;

	float m_PatrolSemiAmplitude;
	bool m_MovingRight = true;
	float m_PatrolSpeed;
	DirectX::XMFLOAT3 m_TargetPos;
	bool m_IsPlayerSpotted;
	DirectX::XMFLOAT3 m_PlayerPos;
	//speed
	int m_MoveSpeed;
	const int m_InitialMoveSpeed;
	const int m_HitDamage;
	bool m_AttackPlayer;
	//attack time
	float m_CurrentAttackTime;
	const float m_MaxTimeForAttack;
	bool m_IsCollidingPlayer;
	bool m_IsAttackable;
	bool m_IsDead;
	bool m_TriggerDeath;
};
