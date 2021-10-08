#include "stdafx.h"
#include "AudioManager.h"
#include "Logger.h"
#include "ContentManager.h"
#include "SoundManager.h"
#include "TransformComponent.h"

using namespace FMOD;

AudioManager::AudioManager()
	: m_Volume(1.f)
	, m_LastCamPos(FMOD_VECTOR())
	, m_MaxSoundChannels{ 20 }
	, m_DeltaT{ 0.0f }
{
	Initialize();
}

inline FMOD_VECTOR ToFmodVector(DirectX::XMFLOAT3 vec)
{
	auto fVec = FMOD_VECTOR();
	fVec.x = vec.x;
	fVec.y = vec.y;
	fVec.z = vec.z;

	return fVec;
}

void AudioManager::Initialize()
{
	// sound source made visible
	const float radius{ 1.0f };
	const int slices{ 10 }, stacks{ 10 };

	//2D SOUND
	FMOD_RESULT fmodResult;
	auto pFmodSystem = SoundManager::GetInstance()->GetSystem();

	{
		FMOD::Sound* pSound;
		fmodResult = pFmodSystem->createStream("Resources/Sounds/PlayerAttack.wav", FMOD_3D | FMOD_3D_LINEARROLLOFF, 0, &pSound);
		SoundManager::GetInstance()->ErrorCheck(fmodResult);

		SoundData data;
		data.sound = pSound;

		data.loop = false;
		data.volume = 0.1f;

		std::pair< SoundId, SoundData> newSound(SoundId::PlayerAttack, data);
		m_SoundLibrary.insert(newSound);
	}
	{
		FMOD::Sound* pSound;
		fmodResult = pFmodSystem->createStream("Resources/Sounds/PlayerThrow.wav", FMOD_3D | FMOD_3D_LINEARROLLOFF, 0, &pSound);
		SoundManager::GetInstance()->ErrorCheck(fmodResult);

		SoundData data;
		data.sound = pSound;

		data.loop = false;
		data.volume = 0.6f;

		std::pair< SoundId, SoundData> newSound(SoundId::PlayerThrow, data);
		m_SoundLibrary.insert(newSound);
	}
	{
		FMOD::Sound* pSound;
		fmodResult = pFmodSystem->createStream("Resources/Sounds/PlayerJump.wav", FMOD_3D | FMOD_3D_LINEARROLLOFF, 0, &pSound);
		SoundManager::GetInstance()->ErrorCheck(fmodResult);

		SoundData data;
		data.sound = pSound;

		data.loop = false;
		data.volume = 0.4f;

		std::pair< SoundId, SoundData> newSound(SoundId::PlayerJump, data);
		m_SoundLibrary.insert(newSound);
	}
	{
		FMOD::Sound* pSound;
		fmodResult = pFmodSystem->createStream("Resources/Sounds/PlayerDamage.wav", FMOD_3D | FMOD_3D_LINEARROLLOFF, 0, &pSound);
		SoundManager::GetInstance()->ErrorCheck(fmodResult);

		SoundData data;
		data.sound = pSound;

		data.loop = false;
		data.volume = 0.6f;

		std::pair< SoundId, SoundData> newSound(SoundId::PlayerDamage, data);
		m_SoundLibrary.insert(newSound);
	}
	{
		FMOD::Sound* pSound;
		fmodResult = pFmodSystem->createStream("Resources/Sounds/ScorePickup.wav", FMOD_3D | FMOD_3D_LINEARROLLOFF, 0, &pSound);
		SoundManager::GetInstance()->ErrorCheck(fmodResult);

		SoundData data;
		data.sound = pSound;

		data.loop = false;
		data.volume = 0.6f;

		std::pair< SoundId, SoundData> newSound(SoundId::PickupScore, data);
		m_SoundLibrary.insert(newSound);
	}
	{
		FMOD::Sound* pSound;
		fmodResult = pFmodSystem->createStream("Resources/Sounds/ThrowablePickup.wav", FMOD_3D | FMOD_3D_LINEARROLLOFF, 0, &pSound);
		SoundManager::GetInstance()->ErrorCheck(fmodResult);

		SoundData data;
		data.sound = pSound;

		data.loop = false;
		data.volume = 0.6f;

		std::pair< SoundId, SoundData> newSound(SoundId::PickupThrowable, data);
		m_SoundLibrary.insert(newSound);
	}
	{
		FMOD::Sound* pSound;
		fmodResult = pFmodSystem->createStream("Resources/Sounds/HealthPickup.wav", FMOD_3D | FMOD_3D_LINEARROLLOFF, 0, &pSound);
		SoundManager::GetInstance()->ErrorCheck(fmodResult);

		SoundData data;
		data.sound = pSound;

		data.loop = false;
		data.volume = 0.6f;

		std::pair< SoundId, SoundData> newSound(SoundId::PickupHealth, data);
		m_SoundLibrary.insert(newSound);
	}
	{
		FMOD::Sound* pSound;
		fmodResult = pFmodSystem->createStream("Resources/Sounds/PlayerDeath.wav", FMOD_3D | FMOD_3D_LINEARROLLOFF, 0, &pSound);
		SoundManager::GetInstance()->ErrorCheck(fmodResult);

		SoundData data;
		data.sound = pSound;

		data.loop = false;
		data.volume = 0.6f;

		std::pair< SoundId, SoundData> newSound(SoundId::PlayerDeath, data);
		m_SoundLibrary.insert(newSound);
	}
	{
		FMOD::Sound* pSound;
		fmodResult = pFmodSystem->createStream("Resources/Sounds/Destroyable.wav", FMOD_3D | FMOD_3D_LINEARROLLOFF, 0, &pSound);
		SoundManager::GetInstance()->ErrorCheck(fmodResult);

		SoundData data;
		data.sound = pSound;

		data.loop = false;
		data.volume = 0.6f;

		std::pair< SoundId, SoundData> newSound(SoundId::CrateDamage, data);
		m_SoundLibrary.insert(newSound);
	}
	{
		FMOD::Sound* pSound;
		fmodResult = pFmodSystem->createStream("Resources/Sounds/Pause.wav", FMOD_3D | FMOD_3D_LINEARROLLOFF, 0, &pSound);
		SoundManager::GetInstance()->ErrorCheck(fmodResult);

		SoundData data;
		data.sound = pSound;

		data.loop = false;
		data.volume = 0.6f;

		std::pair< SoundId, SoundData> newSound(SoundId::Pause, data);
		m_SoundLibrary.insert(newSound);
	}
	{
		FMOD::Sound* pSound;
		fmodResult = pFmodSystem->createStream("Resources/Sounds/EnemyDeath.wav", FMOD_DEFAULT, 0, &pSound);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D.
		SoundManager::GetInstance()->ErrorCheck(fmodResult);

		SoundData data;
		data.sound = pSound;
		data.loop = false;
		data.volume = 0.6f;

		std::pair< SoundId, SoundData> newSound(SoundId::EnemyDeath, data);
		m_SoundLibrary.insert(newSound);
	}
	{
		FMOD::Sound* pSound;
		fmodResult = pFmodSystem->createStream("Resources/Sounds/BackgroundMusic.mp3", FMOD_DEFAULT, 0, &pSound);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D.
		SoundManager::GetInstance()->ErrorCheck(fmodResult);

		SoundData data;
		data.sound = pSound;
		data.loop = true;
		data.volume = 0.2f;

		std::pair< SoundId, SoundData> newSound(SoundId::Ambient, data);
		m_SoundLibrary.insert(newSound);
	}
}

void AudioManager::PostInitialize(const GameContext&)
{
}

void AudioManager::Play(SoundId id)
{
	auto pFmodSystem = SoundManager::GetInstance()->GetSystem();

	const SoundData& data = m_SoundLibrary.at(id);

	if (data.loop)
		data.sound->setMode(FMOD_LOOP_NORMAL);
	else
		data.sound->setMode(FMOD_LOOP_OFF);

	bool isPlaying = true;

	for (int i = 0; i < m_MaxSoundChannels; i++)
	{
		if (m_pChannel[i])
			m_pChannel[i]->isPlaying(&isPlaying);

		if (!isPlaying)
		{
			FMOD_RESULT fmodResult = pFmodSystem->playSound(data.sound, 0, false, &m_pChannel[i]);
			m_pChannel[i]->setVolume(data.volume);
			SoundManager::GetInstance()->ErrorCheck(fmodResult);
			break;
		}
	}
}

void AudioManager::IncreaseVolume()
{
	std::map<SoundId, SoundData>::iterator it;

	for (it = m_SoundLibrary.begin(); it != m_SoundLibrary.end(); it++)
	{
		it->second.volume += m_DeltaT;
	}
}

void AudioManager::DecreaseVolume()
{
	std::map<SoundId, SoundData>::iterator it;

	for (it = m_SoundLibrary.begin(); it != m_SoundLibrary.end(); it++)
	{
		it->second.volume -= m_DeltaT;
	}
}

void AudioManager::Update(const GameContext& gameContext)
{
	//GLOBAL LISTENER SETTINGS
	auto cam = gameContext.pCamera->GetTransform();
	auto forward = ToFmodVector(cam->GetForward());
	auto up = ToFmodVector(cam->GetUp());
	auto pos = ToFmodVector(cam->GetPosition());

	auto vel = FMOD_VECTOR();

	m_DeltaT = gameContext.pGameTime->GetElapsed();
	vel.x = (pos.x - m_LastCamPos.x) / m_DeltaT;
	vel.y = (pos.y - m_LastCamPos.y) / m_DeltaT;
	vel.z = (pos.z - m_LastCamPos.z) / m_DeltaT;
	m_LastCamPos = pos;

	SoundManager::GetInstance()->GetSystem()->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
}