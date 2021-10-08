/*
******************
* DAE SpikeyShader *
******************

FirstName: Rufat
LastName: Aliyev
Class: 2DAE_GD01
*/

//************
// VARIABLES *
//************
cbuffer cbPerObject
{
	float4x4 m_MatrixWorldViewProj : WORLDVIEWPROJECTION;
	float4x4 m_MatrixWorld : WORLD;
	float3 m_LightDir = { 0.2f, -1.0f, 0.2f };
}

RasterizerState FrontCulling
{
	CullMode = NONE;
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap; // of Mirror of Clamp of Border
	AddressV = Wrap; // of Mirror of Clamp of Border
};

Texture2D m_TextureDiffuse;

//**********
// STRUCTS *
//**********
struct VS_DATA
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};

struct GS_DATA
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD0;
};

//****************
// VERTEX SHADER *
//****************
VS_DATA MainVS(VS_DATA vsData)
{
	return vsData;
}

//******************
// GEOMETRY SHADER *
//******************
void CreateVertex(VS_DATA vsData, inout TriangleStream<GS_DATA> triStream)
{
	//Step 1. Create a GS_DATA object
	GS_DATA gsData = (GS_DATA)0;
	//Step 2. Transform the position using the WVP Matrix and assign it to (GS_DATA object).Position (Keep in mind: float3 -> float4)
	gsData.Position = mul((float4(vsData.Position, 1.0f)), m_MatrixWorldViewProj);
	//Step 3. Transform the normal using the World Matrix and assign it to (GS_DATA object).Normal (Only Rotation, No translation!)
	gsData.Normal = normalize(mul(vsData.Normal, (float3x3)m_MatrixWorld));
	//Step 4. Assign texCoord to (GS_DATA object).TexCoord
	gsData.TexCoord = vsData.TexCoord;
	//Step 5. Append (GS_DATA object) to the TriangleStream parameter (TriangleStream::Append(...))
	triStream.Append(gsData);
}

[maxvertexcount(6)]
void SpikeGenerator(inout TriangleStream<GS_DATA> triStream, triangle VS_DATA vertices[3])
{
	//Use these variable names
	float3 basePoint, baseNormal, top, left, right, spikeNormal;

	//Step 1. Calculate The basePoint
	basePoint = (vertices[0].Position + vertices[1].Position + vertices[2].Position) / 3.f;
	//Step 2. Calculate The normal of the basePoint
	baseNormal = (vertices[0].Normal + vertices[1].Normal + vertices[2].Normal) / 3.f;
	//Step 3. Calculate The Spike's Top Position
	top = basePoint + (8 * baseNormal);
	//Step 4. Calculate The Left And Right Positions
	float3 spikeDirection = (vertices[2].Position - vertices[0].Position) * 0.1f;
	left = basePoint - spikeDirection;
	right = basePoint + spikeDirection;
	//Step 5. Calculate The Normal of the spike
	spikeNormal = normalize(cross((top - left), (top - right)));
	//Step 6. Create The Vertices [Complete code in CreateVertex(...)]
	VS_DATA vsData = (VS_DATA)0;
	//
	vsData.Position = vertices[0].Position;
	vsData.Normal = vertices[0].Normal;
	vsData.TexCoord = vertices[0].TexCoord;
	CreateVertex(vsData, triStream);

	vsData.Position = vertices[1].Position;
	vsData.Normal = vertices[1].Normal;
	vsData.TexCoord = vertices[1].TexCoord;
	CreateVertex(vsData, triStream);

	vsData.Position = vertices[2].Position;
	vsData.Normal = vertices[2].Normal;
	vsData.TexCoord = vertices[2].TexCoord;
	CreateVertex(vsData, triStream);

	//Restart the strip so we can add another (independent) triangle!
	triStream.RestartStrip();

	//Create Spike Geometry
	vsData = (VS_DATA)0;
	vsData.Position = top;
	vsData.Normal = spikeNormal;
	vsData.TexCoord = float2(0, 0);
	//
	CreateVertex(vsData, triStream);
	//
	vsData.Position = left;
	CreateVertex(vsData, triStream);
	//
	vsData.Position = right;
	CreateVertex(vsData, triStream);
}

//***************
// PIXEL SHADER *
//***************
float4 MainPS(GS_DATA input) : SV_TARGET
{
	input.Normal = -normalize(input.Normal);
	float alpha = m_TextureDiffuse.Sample(samLinear, input.TexCoord).a;
	float3 color = m_TextureDiffuse.Sample(samLinear, input.TexCoord).rgb;
	float s = max(dot(m_LightDir, input.Normal), 0.4f);

	return float4(color * s, alpha);
}

//*************
// TECHNIQUES *
//*************
technique10 DefaultTechnique
{
	pass p0
	{
		SetRasterizerState(FrontCulling);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(CompileShader(gs_4_0, SpikeGenerator()));
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}