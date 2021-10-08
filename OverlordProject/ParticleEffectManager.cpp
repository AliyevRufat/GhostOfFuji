#include "stdafx.h"
#include "ParticleEffectManager.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "ParticleEmitterComponent.h"
#include "SceneManager.h"
#include "GameScene.h"

ParticleEffectManager::ParticleEffectManager()
{
}

ParticleEffectManager::~ParticleEffectManager()
{
	m_ParticleEmitterObjects.clear();
}

void ParticleEffectManager::Update(float elapsedTime)
{
	for (size_t i = 0; i < m_ParticleEmitterObjects.size(); i++)
	{
		if (m_ParticleEmitterObjects[i].second <= 0.f)
		{
			//RemoveChild(m_ParticleEmitterObjects[i].first);
			m_ParticleEmitterObjects[i].first->SetDeleteLater(true);
			//RemoveChild(m_ParticleEmitterObjects[i].first);
			//SafeDelete(m_ParticleEmitterObjects[i].first);
			m_ParticleEmitterObjects.erase(m_ParticleEmitterObjects.begin() + i);
		}
		else
		{
			m_ParticleEmitterObjects[i].second -= elapsedTime;
			m_ParticleEmitterObjects[i].first->GetComponent<ParticleEmitterComponent>()->SetTransparency(m_ParticleEmitterObjects[i].second);
		}
	}
}

GameObject* ParticleEffectManager::AddParticles(ParticlesID particleID, const DirectX::XMFLOAT3& pos, float lifeTime)
{
	GameObject* particlesObj = new GameObject();
	particlesObj->GetTransform()->Translate(pos);
	ParticleEmitterComponent* newPEC;

	switch (particleID)
	{
	case ParticlesID::SMOKE:
		newPEC = new ParticleEmitterComponent(L"Resources/Textures/Smoke.png", 50);
		newPEC->SetVelocity(DirectX::XMFLOAT3(0, 3.0f, 0));
		newPEC->SetMinSize(2.f);
		newPEC->SetMaxSize(3.f);
		newPEC->SetMinEnergy(1.0f);
		newPEC->SetMaxEnergy(1.2f);
		newPEC->SetMinSizeGrow(1.5f);
		newPEC->SetMaxSizeGrow(3.5f);
		newPEC->SetMinEmitterRange(0.2f);
		newPEC->SetMaxEmitterRange(0.5f);
		newPEC->SetColor(DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.2f));

		particlesObj->AddComponent(newPEC);
		m_ParticleEmitterObjects.push_back(std::make_pair(particlesObj, lifeTime));

		break;
	case ParticlesID::BLOOD:
		newPEC = new ParticleEmitterComponent(L"Resources/Textures/Blood.png", 50);
		newPEC->SetVelocity(DirectX::XMFLOAT3(0.f, 5.0f, 0.f));
		newPEC->SetMinSize(2.5f);
		newPEC->SetMaxSize(4.5f);
		newPEC->SetMinEnergy(1.0f);
		newPEC->SetMaxEnergy(1.0f);
		newPEC->SetMinSizeGrow(1.0f);
		newPEC->SetMaxSizeGrow(2.0f);
		newPEC->SetMinEmitterRange(0.5f);
		newPEC->SetMaxEmitterRange(2.5f);
		newPEC->SetColor(DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.0f));

		particlesObj->AddComponent(newPEC);
		m_ParticleEmitterObjects.push_back(std::make_pair(particlesObj, lifeTime));
		break;
	case ParticlesID::DEBRIS:
		newPEC = new ParticleEmitterComponent(L"Resources/Textures/Debris.png", 20);
		newPEC->SetVelocity(DirectX::XMFLOAT3(0, -5.0f, 0));
		newPEC->SetMinSize(4.5f);
		newPEC->SetMaxSize(5.0f);
		newPEC->SetMinEnergy(3.0f);
		newPEC->SetMaxEnergy(4.5f);
		newPEC->SetMinSizeGrow(3.0f);
		newPEC->SetMaxSizeGrow(4.5f);
		newPEC->SetMinEmitterRange(0.5f);
		newPEC->SetMaxEmitterRange(0.7f);
		newPEC->SetColor(DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.6f));

		particlesObj->AddComponent(newPEC);
		m_ParticleEmitterObjects.push_back(std::make_pair(particlesObj, lifeTime));
		break;
	case ParticlesID::PICKUP:
		newPEC = new ParticleEmitterComponent(L"Resources/Textures/PickUpThrowable.png", 20);
		newPEC->SetVelocity(DirectX::XMFLOAT3(0, 5.0f, 0));
		newPEC->SetMinSize(4.5f);
		newPEC->SetMaxSize(5.0f);
		newPEC->SetMinEnergy(3.0f);
		newPEC->SetMaxEnergy(4.5f);
		newPEC->SetMinSizeGrow(3.0f);
		newPEC->SetMaxSizeGrow(4.5f);
		newPEC->SetMinEmitterRange(0.5f);
		newPEC->SetMaxEmitterRange(0.7f);
		newPEC->SetColor(DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.6f));

		particlesObj->AddComponent(newPEC);
		m_ParticleEmitterObjects.push_back(std::make_pair(particlesObj, lifeTime));
		break;
	}
	SceneManager::GetInstance()->GetActiveScene()->AddChild(particlesObj);
	//AddChild(particlesObj);

	return particlesObj;
}