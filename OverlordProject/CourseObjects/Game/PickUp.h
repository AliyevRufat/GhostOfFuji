#pragma once
#include "GameObject.h"

class Character;

class Pickup final : public GameObject
{
public:

	enum class PickupType
	{
		health,
		score,
		throwable
	};

	Pickup(const std::wstring& modelPath, int diffuseMatId, const DirectX::XMFLOAT3& startLocation, const DirectX::XMFLOAT3& startRotation = { 90, 0, 0 }, PickupType pickupType = PickupType::score);
	virtual ~Pickup() = default;

	Pickup(const Pickup& other) = delete;
	Pickup(Pickup&& other) noexcept = delete;
	Pickup& operator=(const Pickup& other) = delete;
	Pickup& operator=(Pickup&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;

	void OnPickUp(Character* receiver);
	void OnSpawn(float elapsedSec);
	void SetIsSpawned(bool isSpawned);
	PickupType GetPickupType();
private:
	DirectX::XMFLOAT3 m_CurrentRotation;
	float m_RotationSpeed;
	const float m_HealAmount;
	const int m_ScoreAmount;
	bool m_IsSpawned;
	float m_CurrentMoveTime;
	const float m_MaxTimeForMove;
	PickupType m_PickupType;
};
