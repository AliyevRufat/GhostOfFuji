#pragma once
#include <vector>
#include "GameObject.h"
#include "Singleton.h"

class ParticleEffectManager final : public Singleton<ParticleEffectManager>
{
public:
	enum class ParticlesID
	{
		SMOKE,
		BLOOD,
		DEBRIS,
		PICKUP
	};

	ParticleEffectManager();
	~ParticleEffectManager();
	void Update(float elapsedTime);
	GameObject* AddParticles(ParticlesID particleID, const DirectX::XMFLOAT3& pos, float lifeTime);

private:
	std::vector<std::pair<GameObject*, float>> m_ParticleEmitterObjects;
};