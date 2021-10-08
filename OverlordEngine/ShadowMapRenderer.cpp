#include "stdafx.h"
#include "ShadowMapRenderer.h"
#include "ContentManager.h"
#include "ShadowMapMaterial.h"
#include "RenderTarget.h"
#include "MeshFilter.h"
#include "SceneManager.h"
#include "OverlordGame.h"

ShadowMapRenderer::~ShadowMapRenderer()
{
	//TODO: make sure you don't have memory leaks and/or resource leaks :) -> Figure out if you need to do something here
	if (m_pShadowMat)
	{
		delete m_pShadowMat;
		m_pShadowMat = nullptr;
	}

	if (m_pShadowRT)
	{
		delete m_pShadowRT;
		m_pShadowRT = nullptr;
	}
}

void ShadowMapRenderer::Initialize(const GameContext& gameContext)
{
	if (m_IsInitialized)
		return;

	UNREFERENCED_PARAMETER(gameContext);

	//TODO: create shadow generator material + initialize it
	m_pShadowMat = new ShadowMapMaterial();
	m_pShadowMat->Initialize(gameContext);
	//TODO: create a rendertarget with the correct settings (hint: depth only) for the shadow generator using a RENDERTARGET_DESC
	m_pShadowRT = new RenderTarget(gameContext.pDevice);
	RENDERTARGET_DESC renderTargetDesc{};
	renderTargetDesc.EnableColorBuffer = false;
	renderTargetDesc.EnableColorSRV = false;
	renderTargetDesc.ColorBufferSupplied = false;
	renderTargetDesc.EnableDepthBuffer = true;
	renderTargetDesc.EnableDepthSRV = true;
	renderTargetDesc.Width = 1280;
	renderTargetDesc.Height = 720;
	renderTargetDesc.DepthFormat = DXGI_FORMAT_D32_FLOAT;

	m_pShadowRT->Create(renderTargetDesc);

	m_IsInitialized = true;
}

void ShadowMapRenderer::SetLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction)
{
	UNREFERENCED_PARAMETER(position);
	UNREFERENCED_PARAMETER(direction);
	//TODO: store the input parameters in the appropriate datamembers
	m_LightDirection = direction;
	m_LightPosition = position;
	//TODO: calculate the Light VP matrix (Directional Light only ;)) and store it in the appropriate datamember

	using namespace DirectX;

	const auto windowSettings = OverlordGame::GetGameSettings().Window;

	const float viewWidth = (m_Size > 0) ? m_Size * windowSettings.AspectRatio : windowSettings.Width;
	const float viewHeight = (m_Size > 0) ? m_Size : windowSettings.Height;

	XMMATRIX proj = XMMatrixOrthographicLH(viewWidth, viewHeight, 1, 1000);

	XMVECTOR worldPos = XMLoadFloat3(&m_LightPosition);
	XMVECTOR lookAt = XMLoadFloat3(&m_LightDirection);
	XMFLOAT3 upVector = XMFLOAT3{ 0.f,1.f,0.f };

	const XMVECTOR upVec = XMLoadFloat3(&upVector);
	const XMMATRIX view = XMMatrixLookAtLH(worldPos, worldPos + lookAt, upVec);
	XMStoreFloat4x4(&m_LightVP, view * proj);
}

void ShadowMapRenderer::Begin(const GameContext& gameContext) const
{
	//Reset Texture Register 5 (Unbind)
	ID3D11ShaderResourceView* const pSRV[] = { nullptr };
	gameContext.pDeviceContext->PSSetShaderResources(1, 1, pSRV);

	UNREFERENCED_PARAMETER(gameContext);
	//TODO: set the appropriate render target that our shadow generator will write to (hint: use the OverlordGame::SetRenderTarget function through SceneManager)
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(m_pShadowRT);
	//TODO: clear this render target
	m_pShadowRT->Clear(gameContext, 0);
	//TODO: set the shader variables of this shadow generator material
	// 0 it
	m_pShadowMat->SetLightVP(m_LightVP);
	//m_pShadowMat->SetWorld(DirectX::XMFLOAT4X4{});
	//m_pShadowMat->SetBones(nullptr, 0);
}

void ShadowMapRenderer::End(const GameContext& gameContext) const
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: restore default render target (hint: passing nullptr to OverlordGame::SetRenderTarget will do the trick)
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(nullptr);//mb OMSetRenderTargets
}

void ShadowMapRenderer::Draw(const GameContext& gameContext, MeshFilter* pMeshFilter, DirectX::XMFLOAT4X4 world, const std::vector<DirectX::XMFLOAT4X4>& bones) const
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pMeshFilter);
	UNREFERENCED_PARAMETER(world);
	UNREFERENCED_PARAMETER(bones);

	//TODO: update shader variables in material
	m_pShadowMat->SetLightVP(m_LightVP);
	m_pShadowMat->SetWorld(world);
	if (!bones.empty())
		m_pShadowMat->SetBones(&bones.begin()->_11, bones.size());

	//TODO: set the correct inputlayout, buffers, topology (some variables are set based on the generation type Skinned or Static)

	UINT offset = 0;
	if (bones.empty())
	{ // Static
		VertexBufferData bufferData = pMeshFilter->GetVertexBufferData(gameContext, m_pShadowMat->m_InputLayoutIds[0]);//!!!

		gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &bufferData.pVertexBuffer, &bufferData.VertexStride, &offset);

		gameContext.pDeviceContext->IASetIndexBuffer(pMeshFilter->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		gameContext.pDeviceContext->IASetInputLayout(m_pShadowMat->m_pInputLayouts[0]);

		gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_pShadowMat->m_pShadowTechs[0]->GetPassByIndex(0)->Apply(0, gameContext.pDeviceContext);
	}
	else // 1 = Knight
	{ // Skinned
		VertexBufferData bufferData = pMeshFilter->GetVertexBufferData(gameContext, m_pShadowMat->m_InputLayoutIds[1]);//!!!!!!

		gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &bufferData.pVertexBuffer, &bufferData.VertexStride, &offset);

		gameContext.pDeviceContext->IASetIndexBuffer(pMeshFilter->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		gameContext.pDeviceContext->IASetInputLayout(m_pShadowMat->m_pInputLayouts[1]);

		gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_pShadowMat->m_pShadowTechs[1]->GetPassByIndex(0)->Apply(0, gameContext.pDeviceContext);
	}

	gameContext.pDeviceContext->DrawIndexed(pMeshFilter->m_IndexCount, 0, 0);
}

void ShadowMapRenderer::UpdateMeshFilter(const GameContext& gameContext, MeshFilter* pMeshFilter)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pMeshFilter);

	//TODO: based on the type (Skinned or Static) build the correct vertex buffers for the MeshFilter (Hint use MeshFilter::BuildVertexBuffer)

	if (pMeshFilter->m_HasAnimations)
		pMeshFilter->BuildVertexBuffer(gameContext, m_pShadowMat->m_InputLayoutIds[1], m_pShadowMat->m_InputLayoutSizes[1], m_pShadowMat->m_InputLayoutDescriptions[1]);
	else
		pMeshFilter->BuildVertexBuffer(gameContext, m_pShadowMat->m_InputLayoutIds[0], m_pShadowMat->m_InputLayoutSizes[0], m_pShadowMat->m_InputLayoutDescriptions[0]);
}

ID3D11ShaderResourceView* ShadowMapRenderer::GetShadowMap() const
{
	//TODO: return the depth shader resource view of the shadow generator render target
	return m_pShadowRT->GetDepthShaderResourceView();
}