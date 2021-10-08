#include "stdafx.h"
#include "MeshFilterLoader.h"
#include "BinaryReader.h"
#include "EffectHelper.h"

//#define MULTI_TEXCOORD
#define OVM_vMAJOR 1
#ifdef MULTI_TEXCOORD
#define OVM_vMINOR 61
#else
#define OVM_vMINOR 1
#endif

MeshFilter* MeshFilterLoader::LoadContent(const std::wstring& assetFile)
{
	auto binReader = new BinaryReader();
	binReader->Open(assetFile);

	if (!binReader->Exists())
		return nullptr;

	//READ OVM FILE
	const int versionMajor = binReader->Read<char>();
	const int versionMinor = binReader->Read<char>();

	if (versionMajor != OVM_vMAJOR || versionMinor != OVM_vMINOR)
	{
		Logger::LogFormat(LogLevel::Warning, L"MeshDataLoader::Load() > Wrong OVM version\n\tFile: \"%s\" \n\tExpected version %i.%i, not %i.%i.", assetFile.c_str(), OVM_vMAJOR, OVM_vMINOR, versionMajor, versionMinor);
		delete binReader;
		return nullptr;
	}

	unsigned int vertexCount = 0;
	unsigned int indexCount = 0;

	auto pMesh = new MeshFilter();

	for (;;)
	{
		const auto meshDataType = static_cast<MeshDataType>(binReader->Read<char>());
		//if ((int) meshDataType < 1 || (int)meshDataType > 12)
		if (meshDataType == MeshDataType::END)
			break;

		const auto dataOffset = binReader->Read<unsigned int>();

		switch (meshDataType)
		{
		case MeshDataType::HEADER:
		{
			pMesh->m_MeshName = binReader->ReadString();
			vertexCount = binReader->Read<unsigned int>();
			indexCount = binReader->Read<unsigned int>();

			pMesh->m_VertexCount = vertexCount;
			pMesh->m_IndexCount = indexCount;
		}
		break;
		case MeshDataType::POSITIONS:
		{
			pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::POSITION);

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT3 pos;
				pos.x = binReader->Read<float>();
				pos.y = binReader->Read<float>();
				pos.z = binReader->Read<float>();
				pMesh->m_Positions.push_back(pos);
			}
		}
		break;
		case MeshDataType::INDICES:
		{
			for (unsigned int i = 0; i < indexCount; ++i)
			{
				pMesh->m_Indices.push_back(binReader->Read<DWORD>());
			}
		}
		break;
		case MeshDataType::NORMALS:
		{
			pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::NORMAL);

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT3 normal;
				normal.x = binReader->Read<float>();
				normal.y = binReader->Read<float>();
				normal.z = binReader->Read<float>();
				pMesh->m_Normals.push_back(normal);
			}
			const int dfg = 1;
		}
		break;
		case MeshDataType::TANGENTS:
		{
			pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::TANGENT);

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT3 tangent;
				tangent.x = binReader->Read<float>();
				tangent.y = binReader->Read<float>();
				tangent.z = binReader->Read<float>();
				pMesh->m_Tangents.push_back(tangent);
			}
		}
		break;
		case MeshDataType::BINORMALS:
		{
			pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::BINORMAL);

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT3 binormal;
				binormal.x = binReader->Read<float>();
				binormal.y = binReader->Read<float>();
				binormal.z = binReader->Read<float>();
				pMesh->m_Binormals.push_back(binormal);
			}
		}
		break;
		case MeshDataType::TEXCOORDS:
		{
			pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::TEXCOORD);

			auto amountTexCoords = 1;
#ifdef MULTI_TEXCOORD
			amountTexCoords = binReader->Read<USHORT>();
#endif
			pMesh->m_TexCoordCount = amountTexCoords;

			for (unsigned int i = 0; i < vertexCount * amountTexCoords; ++i)
			{
				DirectX::XMFLOAT2 tc;
				tc.x = binReader->Read<float>();
				tc.y = binReader->Read<float>();
				pMesh->m_TexCoords.push_back(tc);
			}
		}
		break;
		case MeshDataType::COLORS:
		{
			pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::COLOR);

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT4 color;
				color.x = binReader->Read<float>();
				color.y = binReader->Read<float>();
				color.z = binReader->Read<float>();
				color.w = binReader->Read<float>();
				pMesh->m_Colors.push_back(color);
			}
		}
		break;
		case MeshDataType::BLENDINDICES:
		{
			pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::BLENDINDICES);

			//TODO: Start parsing the BlendIndices for every vertex
			//and add them to the corresponding vector
			//pMesh->m_BlendIndices
			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT4 blendIndex;
				blendIndex.x = binReader->Read<float>();
				blendIndex.y = binReader->Read<float>();
				blendIndex.z = binReader->Read<float>();
				blendIndex.w = binReader->Read<float>();
				pMesh->m_BlendIndices.push_back(blendIndex);
			}
		}
		break;
		case MeshDataType::BLENDWEIGHTS:
		{
			pMesh->m_HasElement |= static_cast<UINT>(ILSemantic::BLENDWEIGHTS);

			//TODO: Start parsing the BlendWeights for every vertex
			//and add them to the corresponding vector
			//pMesh->m_BlendWeights
			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT4 blendWeight;
				blendWeight.x = binReader->Read<float>();
				blendWeight.y = binReader->Read<float>();
				blendWeight.z = binReader->Read<float>();
				blendWeight.w = binReader->Read<float>();
				pMesh->m_BlendWeights.push_back(blendWeight);
			}
		}
		break;
		case MeshDataType::ANIMATIONCLIPS:
		{
			pMesh->m_HasAnimations = true;

			//TODO: Start parsing the AnimationClips
			//1. Read the clipCount
			const USHORT clipCount = binReader->Read<USHORT>();
			//2. For every clip
			for (int i = 0; i < clipCount; i++)
			{
				//3. Create a AnimationClip object (clip)
				AnimationClip animationClip{};
				//4. Read/Assign the ClipName
				animationClip.Name = binReader->ReadString();
				//5. Read/Assign the ClipDuration
				animationClip.Duration = binReader->Read<float>();
				//6. Read/Assign the TicksPerSecond
				animationClip.TicksPerSecond = binReader->Read<float>();
				//7. Read the KeyCount for this clip
				const USHORT keyCount = binReader->Read<USHORT>();
				//8. For every key
				for (int j = 0; j < keyCount; j++)
				{
					//9. Create a AnimationKey object (key)
					AnimationKey animationKey;
					//10. Read/Assign the Tick
					animationKey.Tick = binReader->Read<float>();
					//11. Read the TransformCount
					const USHORT transformCount = binReader->Read<USHORT>();
					//12. For every transform
					for (int k = 0; k < transformCount; k++)
					{
						//13. Create a XMFLOAT4X4
						DirectX::XMFLOAT4X4 boneTransform{};
						//14. The following 16 floats are the matrix values, they are stored by row

						boneTransform._11 = binReader->Read<float>();
						boneTransform._12 = binReader->Read<float>();
						boneTransform._13 = binReader->Read<float>();
						boneTransform._14 = binReader->Read<float>();
						boneTransform._21 = binReader->Read<float>();
						boneTransform._22 = binReader->Read<float>();
						boneTransform._23 = binReader->Read<float>();
						boneTransform._24 = binReader->Read<float>();
						boneTransform._31 = binReader->Read<float>();
						boneTransform._32 = binReader->Read<float>();
						boneTransform._33 = binReader->Read<float>();
						boneTransform._34 = binReader->Read<float>();
						boneTransform._41 = binReader->Read<float>();
						boneTransform._42 = binReader->Read<float>();
						boneTransform._43 = binReader->Read<float>();
						boneTransform._44 = binReader->Read<float>();
						//15. Add The matrix to the BoneTransform vector of the key
						animationKey.BoneTransforms.push_back(boneTransform);
					}
					//16. Add the key to the key vector of the clip
					animationClip.Keys.push_back(animationKey);
				}
				//17. Add the clip to the AnimationClip vector of the MeshFilter (pMesh->m_AnimationClips)
				pMesh->m_AnimationClips.push_back(animationClip);
			}
		}
		break;
		case MeshDataType::SKELETON:
		{
			//auto temp = binReader->ReadString();
			//TODO: Complete
			//1. Read/Assign the boneCount (pMesh->m_BoneCount)
			pMesh->m_BoneCount = binReader->Read<USHORT>();

			binReader->MoveBufferPosition(dataOffset - sizeof(USHORT));
		}
		break;
		default:
			binReader->MoveBufferPosition(dataOffset);
			break;
		}
	}

	delete binReader;

	return pMesh;
}

void MeshFilterLoader::Destroy(MeshFilter* objToDestroy)
{
	SafeDelete(objToDestroy);
}