#pragma once
#include "GameObject.h"

class Throwable final : public GameObject
{
public:

	Throwable(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir);
	virtual ~Throwable() = default;

	Throwable(const Throwable& other) = delete;
	Throwable(Throwable&& other) noexcept = delete;
	Throwable& operator=(const Throwable& other) = delete;
	Throwable& operator=(Throwable&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext&) override;
	void Update(const GameContext& gameContext) override;
	void CallBackLambdaInit();

private:
	DirectX::XMFLOAT3 m_Pos;
	DirectX::XMFLOAT3 m_Direction;
	GameObject* m_CallBackTriggerCollider = nullptr;
};
