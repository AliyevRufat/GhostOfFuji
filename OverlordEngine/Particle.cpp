#include "stdafx.h"
#include "Particle.h"

// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
using namespace DirectX;

Particle::Particle(const ParticleEmitterSettings& emitterSettings) :
	m_VertexInfo(ParticleVertex()),
	m_EmitterSettings(emitterSettings),
	m_IsActive(false),
	m_TotalEnergy(0),
	m_CurrentEnergy(0),
	m_SizeGrow(0),
	m_InitSize(0)
{}

void Particle::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//////1
	if (!m_IsActive)
	{
		return;
	}
	//////2
	m_CurrentEnergy -= gameContext.pGameTime->GetElapsed();
	//
	if (m_CurrentEnergy < 0)
	{
		m_IsActive = false;
		return;
	}
	//////3
	////A
	m_VertexInfo.Position.x += m_EmitterSettings.Velocity.x * gameContext.pGameTime->GetElapsed();
	m_VertexInfo.Position.y += m_EmitterSettings.Velocity.y * gameContext.pGameTime->GetElapsed();
	m_VertexInfo.Position.z += m_EmitterSettings.Velocity.z * gameContext.pGameTime->GetElapsed();
	////B
	//1.
	m_VertexInfo.Color = m_EmitterSettings.Color;
	//2.
	float particleLifePercent = m_CurrentEnergy / m_TotalEnergy; //starts from 1 ,ends at 0
	//3.
	m_VertexInfo.Color.w = particleLifePercent * 2;
	////C
	//1.
	m_VertexInfo.Size = m_SizeGrow;
	//2.
	//if (m_SizeGrow < 1)
	{
		m_VertexInfo.Size = m_InitSize * (m_SizeGrow * particleLifePercent); // IDK?
	}
	//else if (m_SizeGrow > 1)
	{
		//m_VertexInfo.Size =
	}
}

void Particle::Init(XMFLOAT3 initPosition)
{
	UNREFERENCED_PARAMETER(initPosition);
	//////1
	m_IsActive = true;
	//////2
	m_TotalEnergy = randF(m_EmitterSettings.MinEnergy, m_EmitterSettings.MaxEnergy);
	m_CurrentEnergy = randF(m_EmitterSettings.MinEnergy, m_EmitterSettings.MaxEnergy);
	//////3
	////B
	//1.
	DirectX::XMVECTOR randomDirection{ 1, 0, 0 };
	//2.
	auto randomRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(randF(-DirectX::XM_PI, DirectX::XM_PI), randF(-DirectX::XM_PI, DirectX::XM_PI), randF(-DirectX::XM_PI, DirectX::XM_PI));
	//3.
	auto randomNormalizedVector = DirectX::XMVector3TransformNormal(randomDirection, randomRotationMatrix);
	////C
	float distance = randF(m_EmitterSettings.MinEmitterRange, m_EmitterSettings.MaxEmitterRange);
	////D
	DirectX::XMStoreFloat3(&m_VertexInfo.Position, randomNormalizedVector * distance);
	//
	m_VertexInfo.Position.x += initPosition.x;
	m_VertexInfo.Position.y += initPosition.y;
	m_VertexInfo.Position.z += initPosition.z;
	//////4
	////A
	m_VertexInfo.Size = randF(m_EmitterSettings.MinSize, m_EmitterSettings.MaxSize);
	m_InitSize = randF(m_EmitterSettings.MinSize, m_EmitterSettings.MaxSize);
	////B
	m_SizeGrow = randF(m_EmitterSettings.MinSizeGrow, m_EmitterSettings.MaxSizeGrow);
	//////5
	m_VertexInfo.Rotation = randF(-DirectX::XM_PI, DirectX::XM_PI);
}