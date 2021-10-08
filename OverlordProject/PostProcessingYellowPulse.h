#pragma once
#include "PostProcessingMaterial.h"

class ID3D11EffectShaderResourceVariable;

class PostProcessingYellowPulse : public PostProcessingMaterial
{
public:
	PostProcessingYellowPulse();
	PostProcessingYellowPulse(const PostProcessingYellowPulse& other) = delete;
	PostProcessingYellowPulse(PostProcessingYellowPulse&& other) noexcept = delete;
	PostProcessingYellowPulse& operator=(const PostProcessingYellowPulse& other) = delete;
	PostProcessingYellowPulse& operator=(PostProcessingYellowPulse&& other) noexcept = delete;
	virtual ~PostProcessingYellowPulse() = default;

	void SetElapsedTime(float elapsedTime) { m_ElapsedTime += elapsedTime; }

protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* pRendertarget) override;
private:
	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;
	static ID3DX11EffectScalarVariable* m_pElapsedTime;
	float m_ElapsedTime;
};