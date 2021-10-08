#pragma once
#include "GameObject.h"
#include "ModelComponent.h"
#include "GameScene.h"

class Entity final : public GameObject
{
public:
	Entity(const std::wstring& modelPath, const int& matIndex, bool isDestroyable, bool hasCollider, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& geomScale);
	virtual ~Entity() = default;

	Entity(const Entity& other) = delete;
	Entity(Entity&& other) noexcept = delete;
	Entity& operator=(const Entity& other) = delete;
	Entity& operator=(Entity&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext&) override;
	void Update(const GameContext& gameContext) override;
	bool GetIsCollidingPlayer() const;
	void SetIsCollidingPlayer(bool isCollidingPlayer);
	bool GetIsAttackable() const;
	void SetIsAttackable(bool isAttackable);
	void SetIsDestroyed(bool isDestroyed);
private:
	void OnDestroy();

	std::wstring m_ModelPath;
	const int m_MatIndex;
	DirectX::XMFLOAT3 m_GeomScale;
	bool m_IsDestroyable;
	bool m_IsCollidingPlayer;
	bool m_IsAttackable;
	float m_Health;
	DirectX::XMFLOAT3 m_Rotation;
	bool m_IsDestroyed;
	bool m_HasCollider;
};