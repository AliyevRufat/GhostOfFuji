#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter) :
	m_pMeshFilter(pMeshFilter),
	m_Transforms(std::vector<DirectX::XMFLOAT4X4>()),
	m_IsPlaying(false),
	m_Reversed(false),
	m_ClipSet(false),
	m_TickCount(0),
	m_AnimationSpeed(1.0f)
{
	SetAnimation(0);
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	UNREFERENCED_PARAMETER(clipNumber);
	//TODO: complete
	//Set m_ClipSet to false
	m_ClipSet = false;
	m_CurrentAnimationNumber = clipNumber;
	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	if (!(clipNumber < m_pMeshFilter->m_AnimationClips.size()))
	{
		Reset();
		Logger::LogWarning(L"Warning: clipNumber is bigger than the actual m_AnimationClips vector size");
		return;
	}
	else
	{
		AnimationClip newAnimClip = m_pMeshFilter->m_AnimationClips[clipNumber];
		SetAnimation(newAnimClip);
	}

	//If not,
		//	Call Reset
		//	Log a warning with an appropriate message
		//	return
	//else
		//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
		//	Call SetAnimation(AnimationClip clip)
}

void ModelAnimator::SetAnimation(std::wstring clipName)
{
	UNREFERENCED_PARAMETER(clipName);
	//TODO: complete

	//Set m_ClipSet to false
	m_ClipSet = false;

	auto it = std::find_if(m_pMeshFilter->m_AnimationClips.begin(), m_pMeshFilter->m_AnimationClips.end(),
		[clipName](AnimationClip clip)
	{
		return clip.Name == clipName;
	});
	if (it != m_pMeshFilter->m_AnimationClips.end())
		SetAnimation(*it);
	else
	{
		Reset();
		Logger::LogWarning(L"Warning: The m_AnimationClips vector doesn't have a clip with the given name (clipName)!");
	}

	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	//If found,
	//	Call SetAnimation(Animation Clip) with the found clip
	//Else
	//	Call Reset
	//	Log a warning with an appropriate message
}

void ModelAnimator::SetAnimation(AnimationClip clip)
{
	UNREFERENCED_PARAMETER(clip);
	//TODO: complete
	//Set m_ClipSet to true
	m_ClipSet = true;
	//Set m_CurrentClip
	m_CurrentClip = clip;
	//Call Reset(false)
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	UNREFERENCED_PARAMETER(pause);
	//TODO: complete
	//If pause is true, set m_IsPlaying to false
	if (pause)
	{
		m_IsPlaying = false;
	}

	m_TickCount = 0;
	m_AnimationSpeed = 1.0f;

	if (m_ClipSet)
	{
		std::vector<DirectX::XMFLOAT4X4> boneTransforms = m_CurrentClip.Keys[0].BoneTransforms;
		m_Transforms.assign(boneTransforms.begin(), boneTransforms.end());
	}
	else
	{
		for (size_t i = 0; i < m_pMeshFilter->m_BoneCount; i++)
		{
			m_Transforms[i] = DirectX::XMFLOAT4X4{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		}
	}
}

void ModelAnimator::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: complete
	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1.
		//Calculate the passedTicks (see the lab document)
		auto passedTicks = gameContext.pGameTime->GetElapsed() * m_AnimationSpeed * m_CurrentClip.TicksPerSecond;
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		//Clamp(passedTicks, m_CurrentClip.Duration);
		passedTicks = fmod(passedTicks, m_CurrentClip.Duration);

		//2.
		//IF m_Reversed is true
		if (m_Reversed)
		{
			//	Subtract passedTicks from m_TickCount
			passedTicks -= m_TickCount;
		}
		//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
		if (m_TickCount < 0)
		{
			m_TickCount += m_CurrentClip.Duration;
		}
		else
		{
			//ELSE
			//	Add passedTicks to m_TickCount
			m_TickCount += passedTicks;
		}

		//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
		if (m_TickCount > m_CurrentClip.Duration)
		{
			m_TickCount -= m_CurrentClip.Duration;
		}

		//3.
		//Find the enclosing keys
		AnimationKey keyA, keyB;
		//Iterate all the keys of the clip and find the following keys:
		for (size_t i = 0; i < m_CurrentClip.Keys.size(); i++)
		{
			if (m_CurrentClip.Keys[i].Tick < m_TickCount)
			{
				keyA = m_CurrentClip.Keys[i];
			}
		}
		for (size_t i = 0; i < m_CurrentClip.Keys.size(); i++) //maybe make one for loop if you are sure it goes from left to right
		{
			if (m_CurrentClip.Keys[i].Tick > m_TickCount)
			{
				keyB = m_CurrentClip.Keys[i];
				break;
			}
		}

		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		//float blendFactorA = keyB.Tick - m_TickCount;
		//float blendFactorB = m_TickCount - keyA.Tick;
		float blendFactorA = keyB.Tick - m_TickCount;
		float offset = keyB.Tick - keyA.Tick;

		blendFactorA /= offset;

		//Clear the m_Transforms vector
		m_Transforms.clear();
		//FOR every boneTransform in a key (So for every bone)
		//for (size_t i = 0; i < m_CurrentClip.Keys.size(); i++)
		{
			for (size_t j = 0; j < m_pMeshFilter->m_BoneCount; j++)
			{
				if (keyB.BoneTransforms.size() > 0 && keyA.BoneTransforms.size() > 0)
				{
					const auto transformA = keyA.BoneTransforms[j];
					const auto transformB = keyB.BoneTransforms[j];

					auto transformAMatrix = DirectX::XMLoadFloat4x4(&transformA);
					auto transformBMatrix = DirectX::XMLoadFloat4x4(&transformB);

					DirectX::XMVECTOR transA{}, rotA{}, scaleA{}, transB{}, rotB{}, scaleB{};

					DirectX::XMMatrixDecompose(&scaleA, &rotA, &transA, transformAMatrix);
					DirectX::XMMatrixDecompose(&scaleB, &rotB, &transB, transformBMatrix);

					auto translation = DirectX::XMVectorLerp(transB, transA, blendFactorA);
					auto scale = DirectX::XMVectorLerp(scaleB, scaleA, blendFactorA);
					auto rot = DirectX::XMQuaternionSlerp(rotB, rotA, blendFactorA);

					auto newTranformMatrix = DirectX::XMMatrixAffineTransformation(scale, DirectX::g_XMZero, rot, translation);
					DirectX::XMFLOAT4X4 newTransformFloat4x4 = {};
					DirectX::XMStoreFloat4x4(&newTransformFloat4x4, newTranformMatrix);

					m_Transforms.push_back(newTransformFloat4x4);
				}
			}
		}

		//	Retrieve the transform from keyA (transformA)
		//	auto transformA = ...
		// 	Retrieve the transform from keyB (transformB)
		//	auto transformB = ...
		//	Decompose both transforms
		//	Lerp between all the transformations (Position, Scale, Rotation)
		//	Compose a transformation matrix with the lerp-results
		//	Add the resulting matrix to the m_Transforms vector
		if (m_TickCount >= m_CurrentClip.Duration - 0.5f)
		{
			SetAnimation(0);
			m_TickCount = 0;
		}
	}
}