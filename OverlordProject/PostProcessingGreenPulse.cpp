#include "stdafx.h"
#include "PostProcessingGreenPulse.h"
#include "RenderTarget.h"

ID3DX11EffectScalarVariable* PostProcessingGreenPulse::m_pElapsedTime = nullptr;

PostProcessingGreenPulse::PostProcessingGreenPulse()
	: PostProcessingMaterial(L"Resources/Effects/Post/GreenPulse.fx", 1),
	m_pTextureMapVariabele(nullptr),
	m_ElapsedTime{ 0 }
{
}

void PostProcessingGreenPulse::LoadEffectVariables()
{
	//TODO: Bind the 'gTexture' variable with 'm_pTextureMapVariable'
	//Check if valid!
	if (!m_pTextureMapVariabele)
	{
		m_pTextureMapVariabele = GetEffect()->GetVariableByName("gTexture")->AsShaderResource();
		if (!m_pTextureMapVariabele->IsValid())
		{
			Logger::LogWarning(L"PostInvert::LoadEffectVariables() > \'m_pTextureMapVariabele\' variable not found!");
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

void PostProcessingGreenPulse::UpdateEffectVariables(RenderTarget* pRendertarget)
{
	UNREFERENCED_PARAMETER(pRendertarget);
	//TODO: Update the TextureMapVariable with the Color ShaderResourceView of the given RenderTarget
	if (pRendertarget && m_pTextureMapVariabele)
	{
		m_pTextureMapVariabele->SetResource(pRendertarget->GetShaderResourceView());
	}
	m_pElapsedTime->SetFloat(m_ElapsedTime);
}