//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Mirror;
	AddressV = Mirror;
};

Texture2D gTexture;
float gElapsedTime;

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
DepthStencilState EnableDepthWriting
{
	//Enable Depth Rendering
	DepthEnable = TRUE;
	//Disable Depth Writing
	DepthWriteMask = ALL;
};

/// Create Rasterizer State (Backface culling)
RasterizerState BackCulling
{
	CullMode = BACK;
};

//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
};

struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};

//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	// Set the Position
	output.Position = float4(input.Position, 1);
	// Set the TexCoord
	output.TexCoord = input.TexCoord;

	return output;
}

//PIXEL SHADER
//------------
float4 PS(PS_INPUT input) : SV_Target
{
	float4 textureSample = gTexture.Sample(samPoint, input.TexCoord);

	textureSample = float4(textureSample.x + gElapsedTime / 4.0f, textureSample.y + gElapsedTime / 4.0f,  textureSample.z, textureSample.w);

	return textureSample;
}

//TECHNIQUE
//---------
technique11 YellowPulse
{
	pass P0
	{
		// Set states...
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));

		SetRasterizerState(BackCulling);
		SetDepthStencilState(EnableDepthWriting, 0);
	}
}