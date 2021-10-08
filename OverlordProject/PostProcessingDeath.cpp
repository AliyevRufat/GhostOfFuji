#include "stdafx.h"
#include "PostProcessingDeath.h"
#include "RenderTarget.h"

ID3DX11EffectScalarVariable* PostProcessingDeath::m_pElapsedTime = nullptr;

PostProcessingDeath::PostProcessingDeath()
	: PostProcessingMaterial(L"Resources/Effects/Post/RedDeath.fx", 1),
	m_pTextureMapVariabele(nullptr),
	m_ElapsedTime{ 0 }
{
}

void PostProcessingDeath::LoadEffectVariables()
{
	if (!m_pTextureMapVariabele)
	{
		m_pTextureMapVariabele = GetEffect()->GetVariableByName("gTexture")->AsShaderResource();
		if (!m_pTextureMapVariabele->IsValid())
		{
			Logger::LogWarning(L"PostGrayscale::LoadEffectVariables() > \'m_pTextureMapVariabele\' variable not found!");
			m_pTextureMapVariabele = nullptr;
		}
	}

	if (!m_pElapsedTime)
	{
		m_pElapsedTime = GetEffect()->GetVariableByName("gElapsedTime")->AsScalar();
		if (!m_pElapsedTime->IsValid())
		{
			Logger::LogWarning(L"UberMaterial::LoadEffectVariables() > \'gElapsedTime\' variable not found!");
			m_pElapsedTime = nullptr;
		}
	}
}

void PostProcessingDeath::UpdateEffectVariables(RenderTarget* pRendertarget)
{
	UNREFERENCED_PARAMETER(pRendertarget);
	//TODO: Update the TextureMapVariable with the Color ShaderResourceView of the given RenderTarget
	if (pRendertarget && m_pTextureMapVariabele)
	{
		m_pTextureMapVariabele->SetResource(pRendertarget->GetShaderResourceView());
	}

	m_pElapsedTime->SetFloat(m_ElapsedTime);
}