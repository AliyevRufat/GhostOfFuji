#pragma once
#include "PostProcessingMaterial.h"

class ID3D11EffectShaderResourceVariable;

class PostProcessingBluePulse : public PostProcessingMaterial
{
public:
	PostProcessingBluePulse();
	PostProcessingBluePulse(const PostProcessingBluePulse& other) = delete;
	PostProcessingBluePulse(PostProcessingBluePulse&& other) noexcept = delete;
	PostProcessingBluePulse& operator=(const PostProcessingBluePulse& other) = delete;
	PostProcessingBluePulse& operator=(PostProcessingBluePulse&& other) noexcept = delete;
	virtual ~PostProcessingBluePulse() = default;

	void SetElapsedTime(float elapsedTime) { m_ElapsedTime += elapsedTime; }

protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* pRendertarget) override;
private:
	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;
	static ID3DX11EffectScalarVariable* m_pElapsedTime;
	float m_ElapsedTime;
};