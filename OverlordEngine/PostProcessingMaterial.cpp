#include "stdafx.h"
#include "PostProcessingMaterial.h"
#include "RenderTarget.h"
#include "OverlordGame.h"
#include "ContentManager.h"

PostProcessingMaterial::PostProcessingMaterial(std::wstring effectFile, unsigned int renderIndex,
	std::wstring technique)
	: m_IsInitialized(false),
	m_pInputLayout(nullptr),
	m_pInputLayoutSize(0),
	m_effectFile(std::move(effectFile)),
	m_InputLayoutID(0),
	m_RenderIndex(renderIndex),
	m_pRenderTarget(nullptr),
	m_pVertexBuffer(nullptr),
	m_pIndexBuffer(nullptr),
	m_NumVertices(0),
	m_NumIndices(0),
	m_VertexBufferStride(0),
	m_pEffect(nullptr),
	m_pTechnique(nullptr),
	m_TechniqueName(std::move(technique))
{
}

PostProcessingMaterial::~PostProcessingMaterial()
{
	//TODO: delete and/or release necessary objects and/or resources
	SafeRelease(m_pInputLayout);
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pIndexBuffer);
	SafeDelete(m_pRenderTarget);
}

void PostProcessingMaterial::Initialize(const GameContext& gameContext)
{
	if (!m_IsInitialized)
	{
		//TODO: complete
		//1. LoadEffect (LoadEffect(...))
		LoadEffect(gameContext, m_effectFile);
		//2. CreateInputLaytout (CreateInputLayout(...))
		EffectHelper::BuildInputLayout(gameContext.pDevice, m_pTechnique, &m_pInputLayout, m_pInputLayoutDescriptions, m_pInputLayoutSize, m_InputLayoutID);
		//CreateIndexBuffer(gameContext);
		//   CreateVertexBuffer (CreateVertexBuffer(...)) > As a TriangleStrip (FullScreen Quad)
		CreateVertexBuffer(gameContext);
		CreateIndexBuffer(gameContext);
		//3. Create RenderTarget (m_pRenderTarget)
		m_pRenderTarget = new RenderTarget(gameContext.pDevice);
		RENDERTARGET_DESC renderTargetDesc{};
		renderTargetDesc.Width = OverlordGame::GetGameSettings().Window.Width;
		renderTargetDesc.Height = OverlordGame::GetGameSettings().Window.Height;
		renderTargetDesc.EnableColorBuffer = true;
		renderTargetDesc.EnableColorSRV = true;
		renderTargetDesc.GenerateMipMaps_Color = true;
		renderTargetDesc.EnableDepthBuffer = true;
		renderTargetDesc.EnableDepthSRV = false;

		m_pRenderTarget->Create(renderTargetDesc);
		//		Take a look at the class, figure out how to initialize/create a RenderTarget Object
		//		GameSettings > OverlordGame::GetGameSettings()
		m_IsInitialized = true;
	}
}

bool PostProcessingMaterial::LoadEffect(const GameContext& gameContext, const std::wstring& effectFile)
{
	//TODO: complete
	UNREFERENCED_PARAMETER(gameContext);
	//Load Effect through ContentManager
	m_pEffect = ContentManager::Load<ID3DX11Effect>(effectFile);
	//Check if m_TechniqueName (default constructor parameter) is set
	if (!m_TechniqueName.empty())
	{
		// If SET > Use this Technique (+ check if valid)
		using convert_type = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_type, wchar_t> converter;

		LPCSTR name = converter.to_bytes(m_TechniqueName).c_str();

		//std::string strName = std::string(m_TechniqueName.begin(), m_TechniqueName.end());
		//LPCSTR name = strName.c_str();
		m_pTechnique = m_pEffect->GetTechniqueByName(name);

		//	const std::string t{ std::string(m_TechniqueName.begin(), m_TechniqueName.end()) };
		//
		//	m_pTechnique = m_pEffect->GetTechniqueByName(t.c_str());
	}
	else
	{
		// If !SET > Use Technique with index 0
		m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
	}

	//Call LoadEffectVariables
	LoadEffectVariables();
	return true;
}

void PostProcessingMaterial::Draw(const GameContext& gameContext, RenderTarget* previousRendertarget)
{
	//TODO: complete
	//1. Clear the object's RenderTarget (m_pRenderTarget) [Check RenderTarget Class]
	FLOAT rgba[4]{ 0,0,0,0 };
	m_pRenderTarget->Clear(gameContext, rgba);
	//2. Call UpdateEffectVariables(...)
	UpdateEffectVariables(previousRendertarget);
	//
	UINT offset = 0;
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc(&techDesc);
	//3. Set InputLayout
	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);
	//4. Set VertexBuffer
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertexBufferStride, &offset);
	//4.a Set IndexBuffer
	gameContext.pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//5. Set PrimitiveTopology (TRIANGLELIST)
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//6. Draw
	for (size_t i = 0; i < techDesc.Passes; i++)
	{
		m_pTechnique->GetPassByIndex(i)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
	// Generate Mips
	gameContext.pDeviceContext->GenerateMips(m_pRenderTarget->GetShaderResourceView());
}

void PostProcessingMaterial::CreateVertexBuffer(const GameContext& gameContext)
{
	std::vector<VertexPosTex> v
	{
		{ VertexPosTex(DirectX::XMFLOAT3(-1.f, 1.f, 0.f), DirectX::XMFLOAT2(0.f, 0.f)) },
		{ VertexPosTex(DirectX::XMFLOAT3(1.f, 1.f, 0.f), DirectX::XMFLOAT2(1.f, 0.f)) },
		{ VertexPosTex(DirectX::XMFLOAT3(1.f, -1.f, 0.f), DirectX::XMFLOAT2(1.f, 1.f)) },
		{ VertexPosTex(DirectX::XMFLOAT3(-1.f, -1.f, 0.f), DirectX::XMFLOAT2(0.f, 1.f)) }
	};
	m_VertexBufferStride = sizeof(VertexPosTex);

	m_NumVertices = 4;

	//TODO: complete
	//Create vertex array containing three elements in system memory

	//fill a buffer description to copy the vertexdata into graphics memory
	//create a ID3D10Buffer in graphics memory containing the vertex info

	SafeRelease(m_pVertexBuffer);

	//Vertexbuffer
	D3D11_BUFFER_DESC buffDesc;
	buffDesc.Usage = D3D11_USAGE_DEFAULT;
	buffDesc.ByteWidth = sizeof(VertexPosTex) * m_NumVertices;
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.CPUAccessFlags = 0;
	buffDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = v.data();

	gameContext.pDevice->CreateBuffer(&buffDesc, &initData, &m_pVertexBuffer);
}

void PostProcessingMaterial::CreateIndexBuffer(const GameContext& gameContext)
{
	m_NumIndices = 6;
	//TODO: complete
	// Create index buffer
	int indices[]{ 0,1,2,3,0,2 };

	D3D11_BUFFER_DESC indexBuffDesc;
	indexBuffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	indexBuffDesc.ByteWidth = sizeof(DWORD) * m_NumIndices;
	indexBuffDesc.CPUAccessFlags = 0;
	indexBuffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	indexBuffDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = &indices;

	auto hr = gameContext.pDevice->CreateBuffer(&indexBuffDesc, &initData, &m_pIndexBuffer);
	Logger::LogHResult(hr, L"PostProcessingMaterial::CreateIndexBuffer()");
}