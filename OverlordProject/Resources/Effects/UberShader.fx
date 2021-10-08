/*
******************
* DAE Ubershader *
******************

FirstName: Rufat
LastName: Aliyev
Class: 2DAE_GD01

**This Shader Contains:

- Diffuse (Texture & Color)
	- Regular Diffuse
- Specular
	- Specular Level (Texture & Value)
	- Shininess (Value)
	- Models
		- Blinn
		- Phong
- Ambient (Color)
- EnvironmentMapping (CubeMap)
	- Reflection + Fresnel Falloff
	- Refraction
- Normal (Texture)
- Opacity (Texture & Value)

-Techniques
	- WithAlphaBlending
	- WithoutAlphaBlending
*/

//GLOBAL MATRICES
//***************
// The World View Projection Matrix
float4x4 gMatrixWVP : WORLDVIEWPROJECTION;
// The ViewInverse Matrix - the third row contains the camera position!
float4x4 gMatrixViewInverse : VIEWINVERSE;
// The World Matrix
float4x4 gMatrixWorld : WORLD;

//STATES
//******
RasterizerState gRS_FrontCulling
{
	CullMode = FRONT;
};

BlendState gBS_EnableBlending
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};

//SAMPLER STATES
//**************
SamplerState gTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;
};

//LIGHT
//*****
float3 gLightDirection :DIRECTION;

//DIFFUSE
//*******
bool gUseTextureDiffuse;

float4 gColorDiffuse;

Texture2D gTextureDiffuse;

//SPECULAR
//********
float4 gColorSpecular;

Texture2D gTextureSpecularIntensity;

bool gUseTextureSpecularIntensity;

int gShininess;

//AMBIENT
//*******
float4 gColorAmbient;

float gAmbientIntensity;

//NORMAL MAPPING
//**************
bool gFlipGreenChannel;

bool gUseTextureNormal;

Texture2D gTextureNormal;

//ENVIRONMENT MAPPING
//*******************
TextureCube gCubeEnvironment;

bool gUseEnvironmentMapping;

float gReflectionStrength;

float gRefractionStrength;

float gRefractionIndex;

//OPACITY
//***************
float gOpacityIntensity;

bool gUseTextureOpacity;

Texture2D gTextureOpacity;

//SPECULAR MODELS
//***************
bool gUseSpecularBlinn;

bool gUseSpecularPhong;

//FRESNEL FALLOFF
//***************
bool gUseFresnelFalloff;

float4 gColorFresnel;

float gFresnelPower;

float gFresnelMultiplier;

float gFresnelHardness;

//VS IN & OUT
//***********
struct VS_Input
{
	float3 Position: POSITION;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
	float2 TexCoord: TEXCOORD0;
};

struct VS_Output
{
	float4 Position: SV_POSITION;
	float4 WorldPosition: COLOR0;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
	float2 TexCoord: TEXCOORD0;
};

float3 CalculateSpecularBlinn(float3 viewDirection, float3 normal, float2 texCoord)
{
	float3 halfVector = -normalize(viewDirection + gLightDirection);

	float specularStrength = pow(saturate(dot(halfVector, normal)), gShininess);

	float3 specColor = gColorSpecular.rgb * specularStrength;

	if (gUseTextureSpecularIntensity)
	{
		specColor = specColor * gTextureSpecularIntensity.Sample(gTextureSampler, texCoord).r;
	}

	return specColor;
}

float3 CalculateSpecularPhong(float3 viewDirection, float3 normal, float2 texCoord)
{
	float3 reflectDir = reflect(-gLightDirection, normal);

	float specularStrength = pow(saturate(dot(reflectDir, viewDirection)), gShininess);

	float3 specColor = gColorSpecular.rgb * specularStrength;

	if (gUseTextureSpecularIntensity)
	{
		specColor = specColor * gTextureSpecularIntensity.Sample(gTextureSampler, texCoord).r;
	}

	return specColor;
}

float3 CalculateSpecular(float3 viewDirection, float3 normal, float2 texCoord)
{
	float3 specColor = float3(0, 0, 0);

	if (gUseSpecularBlinn)
	{
		specColor = CalculateSpecularBlinn(viewDirection, normal, texCoord);
	}
	else if (gUseSpecularPhong)
	{
		specColor = CalculateSpecularPhong(viewDirection, normal, texCoord);
	}

	return specColor;
}

float3 CalculateNormal(float3 tangent, float3 normal, float2 texCoord)
{
	if (!gUseTextureNormal)
	{
		return normal;
	}

	float3 binormal = normalize(cross(normal, tangent));

	if (gFlipGreenChannel)
	{
		binormal = -binormal;
	}

	float3x3 localAxis = float3x3(tangent, binormal, normal);

	float3 normalSample = gTextureNormal.Sample(gTextureSampler, texCoord);

	float3 sampledNormal = 2.f * normalSample - 1.f;

	float3 newNormal = mul(sampledNormal, localAxis);

	return newNormal;
}

float3 CalculateDiffuse(float3 normal, float2 texCoord)
{
	float diffuseStrength = saturate(sqrt(dot(normal, -gLightDirection) * .5f + .5f));
	float3 diffColor = gColorDiffuse * diffuseStrength;
	if (gUseTextureDiffuse)
	{
		diffColor = diffColor * gTextureDiffuse.Sample(gTextureSampler, texCoord);
	}

	return diffColor;
}

float3 CalculateFresnelFalloff(float3 normal, float3 viewDirection, float3 environmentColor)
{
	if (!gUseFresnelFalloff)
	{
		return environmentColor;
	}
	float fresnel = mul(saturate(pow(1 - saturate(abs(dot(normal, viewDirection))), gFresnelPower)), gFresnelMultiplier);
	//
	float3 YUp = (0, -1, 0);
	//
	float fresnelMask = pow((1 - saturate(dot(YUp, normal))), gFresnelHardness);

	if (gUseEnvironmentMapping)
	{
		return fresnel * fresnelMask * environmentColor;
	}

	return fresnel * fresnelMask * gColorFresnel;
}

float3 CalculateEnvironment(float3 viewDirection, float3 normal)
{
	if (!gUseEnvironmentMapping)
	{
		return float3(0, 0, 0);
	}
	//
	float3 reflectedVector = reflect(viewDirection, normal);
	float3 refractedVector = refract(viewDirection, normal, gRefractionIndex);
//
	float3 environmentColor = gCubeEnvironment.Sample(gTextureSampler, reflectedVector) * gReflectionStrength + gCubeEnvironment.Sample(gTextureSampler, refractedVector).rgb * gRefractionStrength;
	return environmentColor;
}

float CalculateOpacity(float2 texCoord)
{
	float opacity = gOpacityIntensity;
	//
	if (gUseTextureOpacity)
	{
		opacity = gTextureOpacity.Sample(gTextureSampler, texCoord).r;
	}
	//
	return opacity;
}

// The main vertex shader
VS_Output MainVS(VS_Input input) 
{
	VS_Output output = (VS_Output)0;
	//
	output.Position = mul(float4(input.Position, 1.0), gMatrixWVP);
	output.WorldPosition = mul(float4(input.Position, 1.0), gMatrixWorld);
	output.Normal = mul(input.Normal, (float3x3)gMatrixWorld);
	output.Tangent = mul(input.Tangent, (float3x3)gMatrixWorld);
	output.TexCoord = input.TexCoord;
	//
	return output;
}

// The main pixel shader
float4 MainPS(VS_Output input) : SV_TARGET
{
	// NORMALIZE
	input.Normal = normalize(input.Normal);
	input.Tangent = normalize(input.Tangent);

	float3 viewDirection = normalize(input.WorldPosition.xyz - gMatrixViewInverse[3].xyz);

	//NORMAL
	float3 newNormal = CalculateNormal(input.Tangent, input.Normal, input.TexCoord);

	//SPECULAR
	float3 specColor = CalculateSpecular(viewDirection, input.Normal, input.TexCoord);

	//DIFFUSE
	float3 diffColor = CalculateDiffuse(newNormal, input.TexCoord);

	//AMBIENT
	float3 ambientColor = gColorAmbient * gAmbientIntensity;

	//ENVIRONMENT MAPPING
	float3 environmentColor = CalculateEnvironment(viewDirection, newNormal);

	//FRESNEL FALLOFF
	environmentColor = CalculateFresnelFalloff(newNormal, viewDirection, environmentColor);

	//FINAL COLOR CALCULATION
	float3 finalColor = diffColor + specColor + environmentColor + ambientColor;

	//OPACITY
	float opacity = CalculateOpacity(input.TexCoord);
	//
	return float4(finalColor,opacity);
}

// Default Technique
technique10 WithAlphaBlending 
{
	pass p0 {
		SetRasterizerState(gRS_FrontCulling);
		SetBlendState(gBS_EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}

// Default Technique
technique10 WithoutAlphaBlending 
{
	pass p0 {
		SetRasterizerState(gRS_FrontCulling);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}