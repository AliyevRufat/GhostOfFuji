#include "stdafx.h"
#include "HealthComponent.h"
#include "GameObject.h"
#include "Character.h"
#include "TransformComponent.h"
#include "../OverlordProject/CourseObjects/Game/Entity.h"
#include "../OverlordProject/CourseObjects/Game/Enemy.h"
#include "Logger.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "../OverlordProject/ParticleEffectManager.h"
#include "../OverlordProject/AudioManager.h"

HealthComponent::HealthComponent(const int maxHealth)
	:m_MaxHealth{ maxHealth }
{
}

int HealthComponent::GetCurrHealth() const
{
	return m_CurrentHealth;
}

int HealthComponent::GetMaxHealth() const
{
	return m_MaxHealth;
}

void HealthComponent::AddHealth(int amountOfHealth)
{
	m_CurrentHealth += amountOfHealth;

	if (m_CurrentHealth >= m_MaxHealth)
	{
		m_CurrentHealth = m_MaxHealth;
	}
}

void HealthComponent::Damage(int amount)
{
	if (m_pGameObject->GetTag() == L"Enemy")
	{
		ParticleEffectManager::GetInstance()->AddParticles(ParticleEffectManager::ParticlesID::BLOOD, m_pGameObject->GetTransform()->GetPosition(), 1.f);
	}
	if (m_pGameObject->GetTag() == L"Player")
	{
		AudioManager::GetInstance()->Play(AudioManager::SoundId::PlayerDamage);
		ParticleEffectManager::GetInstance()->AddParticles(ParticleEffectManager::ParticlesID::BLOOD, m_pGameObject->GetTransform()->GetPosition(), 1.f);
	}
	else
	{
		AudioManager::GetInstance()->Play(AudioManager::SoundId::CrateDamage);
		ParticleEffectManager::GetInstance()->AddParticles(ParticleEffectManager::ParticlesID::SMOKE, m_pGameObject->GetTransform()->GetPosition(), 1.f);
		ParticleEffectManager::GetInstance()->AddParticles(ParticleEffectManager::ParticlesID::DEBRIS, m_pGameObject->GetTransform()->GetPosition(), 1.f);
	}

	m_CurrentHealth -= amount;

	if (m_CurrentHealth <= 0)
	{
		Die();
	}
}

void HealthComponent::Update(const GameContext& context) { UNREFERENCED_PARAMETER(context); }

void HealthComponent::Draw(const GameContext& context) { UNREFERENCED_PARAMETER(context); }

void HealthComponent::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	m_CurrentHealth = m_MaxHealth;
}

void HealthComponent::Die()
{
	if (m_pGameObject->GetTag() == L"Player")
	{
		Character* character = dynamic_cast<Character*>(m_pGameObject);

		if (character)
		{
			AudioManager::GetInstance()->Play(AudioManager::SoundId::PlayerDeath);
			character->SetIsDead(true);
		}
	}
	else if (m_pGameObject->GetTag() == L"Enemy")
	{
		Enemy* enemy = dynamic_cast<Enemy*>(m_pGameObject);

		if (enemy)
		{
			enemy->SetIsDead(true);
		}
	}
	else
	{
		Entity* entity = dynamic_cast<Entity*>(m_pGameObject);

		if (entity)
		{
			entity->SetDeleteLater(true);
			entity->SetIsDestroyed(true);
		}
	}
}