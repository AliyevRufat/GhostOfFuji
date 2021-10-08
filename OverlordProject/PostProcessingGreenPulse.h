#pragma once
#include "PostProcessingMaterial.h"

class ID3D11EffectShaderResourceVariable;

class PostProcessingGreenPulse : public PostProcessingMaterial
{
public:
	PostProcessingGreenPulse();
	PostProcessingGreenPulse(const PostProcessingGreenPulse& other) = delete;
	PostProcessingGreenPulse(PostProcessingGreenPulse&& other) noexcept = delete;
	PostProcessingGreenPulse& operator=(const PostProcessingGreenPulse& other) = delete;
	PostProcessingGreenPulse& operator=(PostProcessingGreenPulse&& other) noexcept = delete;
	virtual ~PostProcessingGreenPulse() = default;

	void SetElapsedTime(float elapsedTime) { m_ElapsedTime += elapsedTime; }

protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* pRendertarget) override;
private:
	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;
	static ID3DX11EffectScalarVariable* m_pElapsedTime;
	float m_ElapsedTime;
};