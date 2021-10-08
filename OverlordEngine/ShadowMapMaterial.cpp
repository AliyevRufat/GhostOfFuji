//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ShadowMapMaterial.h"
#include "ContentManager.h"

ShadowMapMaterial::~ShadowMapMaterial()
{
	SafeRelease(m_pInputLayouts[1]);
	SafeRelease(m_pInputLayouts[0]);
}

void ShadowMapMaterial::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	if (!m_IsInitialized)
	{
		//TODO: initialize the effect, techniques, shader variables, input layouts (hint use EffectHelper::BuildInputLayout), etc.
		m_pShadowEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/ShadowMapGenerator.fx");

		m_pShadowTechs[0] = m_pShadowEffect->GetTechniqueByIndex(0);
		m_pShadowTechs[1] = m_pShadowEffect->GetTechniqueByIndex(1);

		//UINT m_InputLayoutIds[NUM_TYPES] = { 0,0 };
		//std::vector<ILDescription> m_InputLayoutDescriptions[NUM_TYPES];

		EffectHelper::BuildInputLayout(gameContext.pDevice, m_pShadowTechs[0], &m_pInputLayouts[0], m_InputLayoutDescriptions[0], m_InputLayoutSizes[0], m_InputLayoutIds[0]);
		EffectHelper::BuildInputLayout(gameContext.pDevice, m_pShadowTechs[1], &m_pInputLayouts[1], m_InputLayoutDescriptions[1], m_InputLayoutSizes[1], m_InputLayoutIds[1]);

		m_pWorldMatrixVariable = m_pShadowEffect->GetVariableByName("gWorld")->AsMatrix();
		m_pBoneTransforms = m_pShadowEffect->GetVariableByName("gBones")->AsMatrix();
		m_pLightVPMatrixVariable = m_pShadowEffect->GetVariableByName("gLightViewProj")->AsMatrix();

		m_IsInitialized = true;
	}
}

void ShadowMapMaterial::SetLightVP(DirectX::XMFLOAT4X4 lightVP) const
{
	UNREFERENCED_PARAMETER(lightVP);
	//TODO: set the correct shader variable
	m_pLightVPMatrixVariable->SetMatrix(&lightVP._11);
}

void ShadowMapMaterial::SetWorld(const DirectX::XMFLOAT4X4& world) const
{
	UNREFERENCED_PARAMETER(world);
	//TODO: set the correct shader variable
	m_pWorldMatrixVariable->SetMatrix(&world._11);
}

void ShadowMapMaterial::SetBones(const float* pData, int count) const
{
	UNREFERENCED_PARAMETER(pData);
	UNREFERENCED_PARAMETER(count);
	//TODO: set the correct shader variable
	m_pBoneTransforms->SetMatrixArray(pData, 0, count);
}