#include "stdafx.h"
#include "DiffuseMaterial.h"
#include "ContentManager.h"
#include "TextureData.h"

ID3DX11EffectShaderResourceVariable* DiffuseMaterial::m_pDiffuseSRVvariable = nullptr;

DiffuseMaterial::DiffuseMaterial(bool enableTransparency) : Material(L"./Resources/Effects/PosNormTex3d.fx", enableTransparency ? L"TransparencyTech" : L"")
, m_pDiffuseTexture{}
{
}

DiffuseMaterial::~DiffuseMaterial()
{
}

void DiffuseMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void DiffuseMaterial::LoadEffectVariables()
{
	if (!m_pDiffuseSRVvariable)
	{
		m_pDiffuseSRVvariable = GetEffect()->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseSRVvariable->IsValid())
		{
			Logger::LogWarning(L"DiffuseMaterial::LoadEffectVariables() > \'m_TextureDiffuse\' variable not found!");
			m_pDiffuseSRVvariable = nullptr;
		}
	}
}

void DiffuseMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);

	if (m_pDiffuseTexture && m_pDiffuseSRVvariable)
	{
		m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
	}
}