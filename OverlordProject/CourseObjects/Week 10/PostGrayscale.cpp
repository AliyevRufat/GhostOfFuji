#include "stdafx.h"
#include "PostGrayscale.h"
#include "RenderTarget.h"

PostGrayscale::PostGrayscale()
	: PostProcessingMaterial(L"./Resources/Effects/Post/Grayscale.fx", 1),
	m_pTextureMapVariabele(nullptr)
{
}

void PostGrayscale::LoadEffectVariables()
{
	//TODO: Bind the 'gTexture' variable with 'm_pTextureMapVariable'
	//Check if valid!
	if (!m_pTextureMapVariabele)
	{
		m_pTextureMapVariabele = GetEffect()->GetVariableByName("gTexture")->AsShaderResource();
		if (!m_pTextureMapVariabele->IsValid())
		{
			Logger::LogWarning(L"PostGrayscale::LoadEffectVariables() > \'m_pTextureMapVariabele\' variable not found!");
			m_pTextureMapVariabele = nullptr;
		}
	}
}

void PostGrayscale::UpdateEffectVariables(RenderTarget* pRendertarget)
{
	//TODO: Update the TextureMapVariable with the Color ShaderResourceView of the given RenderTarget
	if (pRendertarget && m_pTextureMapVariabele)
	{
		m_pTextureMapVariabele->SetResource(pRendertarget->GetShaderResourceView());
	}
}