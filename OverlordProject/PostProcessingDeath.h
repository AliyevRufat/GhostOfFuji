#pragma once
#include "PostProcessingMaterial.h"

class ID3D11EffectShaderResourceVariable;

class PostProcessingDeath final : public PostProcessingMaterial
{
public:
	PostProcessingDeath();
	PostProcessingDeath(const PostProcessingDeath& other) = delete;
	PostProcessingDeath(PostProcessingDeath&& other) noexcept = delete;
	PostProcessingDeath& operator=(const PostProcessingDeath& other) = delete;
	PostProcessingDeath& operator=(PostProcessingDeath&& other) noexcept = delete;
	virtual ~PostProcessingDeath() = default;

	void SetElapsedTime(float elapsedTime) { m_ElapsedTime += elapsedTime; }

protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* pRendertarget) override;
private:
	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;
	static ID3DX11EffectScalarVariable* m_pElapsedTime;
	float m_ElapsedTime;
};