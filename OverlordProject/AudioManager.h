#pragma once
#include "GameObject.h"
#include "Singleton.h"

class AudioManager final : public GameObject, public Singleton<AudioManager>
{
public:

	struct SoundData
	{
		FMOD::Sound* sound;
		bool loop;
		float volume;
	};

	enum class SoundId
	{
		Ambient,
		Pause,
		PickupHealth,
		PickupThrowable,
		PickupScore,
		PlayerJump,
		PlayerThrow,
		PlayerAttack,
		PlayerDamage,
		Pickup,
		PlayerDeath,
		CrateDamage,
		EnemyDeath
	};

	AudioManager(const AudioManager& other) = delete;
	AudioManager(AudioManager&& other) noexcept = delete;
	AudioManager& operator=(const AudioManager& other) = delete;
	AudioManager& operator=(AudioManager&& other) noexcept = delete;

	void Initialize(const GameContext&) override {};
	void Initialize();
	void PostInitialize(const GameContext&) override;
	void Update(const GameContext& gameContext) override;

	void Play(SoundId sound);
	void IncreaseVolume();
	void DecreaseVolume();

	void SetInitialized(bool value) { m_Initialized = value; }
	bool GetInitialized() const { return m_Initialized; };

	virtual ~AudioManager()
	{
		std::map<SoundId, SoundData>::iterator it;

		for (it = m_SoundLibrary.begin(); it != m_SoundLibrary.end(); it++)
		{
			it->second.sound->release();
		}

		for (int i = 0; i < m_MaxSoundChannels; i++)
		{
			m_pChannel[i]->stop();
			m_pChannel[i] = nullptr;
		}

		SetInstanceNull();
	}

private:
	friend class Singleton<AudioManager>;

	AudioManager();

	std::map<SoundId, SoundData> m_SoundLibrary;

	const int m_MaxSoundChannels;
	FMOD::Channel* m_pChannel[20];
	FMOD_VECTOR m_LastCamPos;
	float m_Volume;
	float m_DeltaT;
	bool m_Initialized; // Is in scene
};
