#include "..\public\Model.h"
#include "GameInstance.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Bone.h"
#include "Animation.h"
#include "MathUtils.h"
#include "Channel.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_iNumBones(rhs.m_iNumBones)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_strAnimationName(rhs.m_strAnimationName)
	, m_PivotMatrix(rhs.m_PivotMatrix)
{
	for (auto& pMaterial : m_Materials)
	{
		for (int i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(pMaterial.pTexture[i]);
	}

	for (auto& pMesh : rhs.m_Meshes)
		m_Meshes.push_back(static_cast<CMesh*>(pMesh->Clone()));

	wcscpy_s(m_FilePath, rhs.m_FilePath);
}

CBone * CModel::Get_BonePtr(const char * pBoneName)
{
	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool {
		return !strcmp(pBoneName, pBone->Get_Name());
	});

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;
}

HRESULT CModel::Init_Prototype(TYPE eType, const _tchar * pModelFilePath, _fmatrix PivotMatrix)
{
	_uint			iFlag = 0;

	m_eType = eType;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	wcscpy_s(m_FilePath, pModelFilePath);

	HANDLE      hFile = CreateFile(pModelFilePath,
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	if (FAILED(Load_MesheContainers(hFile, eType, PivotMatrix)))
		return E_FAIL;

	if (FAILED(Load_Materials(hFile)))
		return E_FAIL;

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CModel::Init(void * pArg)
{
	_tchar szBone[MAX_PATH] = TEXT("BONE");
	wcscat_s(m_FilePath, szBone);

	HANDLE      hFile = CreateFile(m_FilePath,
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	if (FAILED(Load_Bones(hFile, nullptr)))
		return E_FAIL;

	for (auto& pMesh : m_Meshes)
		pMesh->Load_MeshBones(this, hFile);

	if (FAILED(Load_Animations(hFile)))
		return E_FAIL;

	CloseHandle(hFile);

	return S_OK;
}

void CModel::Play_Animation(_double TimeDelta)
{
	if (TYPE_NONANIM == m_eType)
		return;

		if (m_fBlendCurTime < m_fBlendDuration)
		{
			_float fBlendRatio = m_fBlendCurTime / m_fBlendDuration;
			m_Animations[m_iPreAnimIndex]->Update_Bones(TimeDelta);
			m_Animations[m_iCurrentAnimIndex]->Update_Bones_Blend(TimeDelta, fBlendRatio);

			m_fBlendCurTime += (float)TimeDelta;
		}
		else
		{
			m_Animations[m_iCurrentAnimIndex]->Update_Bones(TimeDelta);
		}

		for (auto& pBone : m_Bones)
		{
			if (nullptr != pBone)
				pBone->Compute_CombindTransformationMatrix();
		}

		//m_vMovePos = _float4(0.f, 0.f, 0.f, 1.f);
		//m_vMovePos = m_Animations[m_iCurrentAnimIndex]->Get_MovePos();
}

void CModel::Play_AddtivieAnim(_double TimeDelta, _float fRatio)
{
	if (TYPE_NONANIM == m_eType)
		return;

	m_Animations[m_iAdditiveAnimIndex]->Update_Bones_Add(TimeDelta, fRatio);

	for (auto& pBone : m_Bones)
	{
		if (nullptr != pBone)
			pBone->Compute_CombindTransformationMatrix();
	}

	//m_vMovePos = _float4(0.f, 0.f, 0.f, 1.f);
	//m_vMovePos = m_Animations[m_iCurrentAnimIndex]->Get_MovePos();
}

void CModel::Set_AnimationIndex(_uint iIndex , _double time)
{
	if (m_iCurrentAnimIndex != iIndex)
	{
		m_iPreAnimIndex = m_iCurrentAnimIndex;
		m_fBlendCurTime = 0.f;
	}

	m_iCurrentAnimIndex = iIndex;
}

HRESULT CModel::Bind_Material(CShader * pShader, _uint iMeshIndex, TextureType eType, const char * pConstantName)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	if (nullptr != m_Materials[iMaterialIndex].pTexture[eType])
		m_Materials[iMaterialIndex].pTexture[eType]->Bind_ShaderResource(pShader, pConstantName);
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex, const char* pBoneConstantName)
{
	if (m_Meshes[iMeshIndex] != nullptr)
	{
		if (pBoneConstantName != nullptr)
		{
			_float4x4		BoneMatrices[256];

			m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, m_PivotMatrix);
		
			pShader->Set_MatrixArray(pBoneConstantName, BoneMatrices, 256);
		}
		pShader->Begin(iPassIndex);

		m_Meshes[iMeshIndex]->Render();
	}

	return S_OK;
}

void CModel::Imgui_RenderProperty()
{
	ImGui::Begin("Model");

	if (ImGui::BeginTabBar("Manager_Tab"))
	{
#pragma region 뼈 imgui
		if (ImGui::BeginTabItem("Bone"))
		{
			if (ImGui::BeginListBox("##"))
			{
				for (auto& pBone : m_Bones)
				{
					const bool bSelected = m_pSelectedBone == pBone;

					if (bSelected)
						ImGui::SetItemDefaultFocus();

					if (ImGui::Selectable(pBone->Get_Name(), bSelected))
						m_pSelectedBone = pBone;
				}
				ImGui::EndListBox();
			}
			ImGui::EndTabItem();
		}
#pragma endregion
		if (ImGui::BeginTabItem("Material"))
		{
			for (auto& pMaterial : m_Materials)
			{
				ImGui::Text(pMaterial.MaterialName);
				for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
				{
					if (pMaterial.pTexture[i] != nullptr)
					{
						ImGui::Image(pMaterial.pTexture[i]->Get_Texture(0), ImVec2(60.f, 60.f));
						ImGui::SameLine();
					}
				}
				ImGui::NewLine();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	if (m_Animations.size() > 0 && m_eType == CModel::TYPE_ANIM)
	{
		if (ImGui::CollapsingHeader("For.Animation"))
		{
			const char* combo_preview_value = Get_CurAnim()->Get_Name();

			if (ImGui::BeginCombo("ANIM", combo_preview_value))
			{
				_uint iAnimSize = static_cast<_uint>(m_Animations.size());
				for (_uint i = 0; i < iAnimSize; i++)
				{
					const bool is_selected = (m_iCurrentAnimIndex == i);
					if (ImGui::Selectable(m_Animations[i]->Get_Name(), is_selected))
						m_iCurrentAnimIndex = i;

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
		ImGui::Text("Current Anim Index"); ImGui::SameLine();
		ImGui::Text(to_string(m_iCurrentAnimIndex).c_str());
	}

	ImGui::End();
}

HRESULT CModel::Load_MesheContainers(HANDLE hFile, TYPE eType, _fmatrix PivotMatrix)
{
	DWORD	dwByte = 0;

	if (eType == TYPE_ANIM)
	{
		// m_iNumMeshes
		ReadFile(hFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < m_iNumMeshes; ++i)
		{
			CMesh::MeshLoadTag meshLoad;
			ZeroMemory(&meshLoad, sizeof(CMesh::MeshLoadTag));

			//MaterialIndex
			ReadFile(hFile, &meshLoad.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

			//NumVertices
			ReadFile(hFile, &meshLoad.iNumVertices, sizeof(_uint), &dwByte, nullptr);

			//NumPrimitive
			ReadFile(hFile, &meshLoad.iNumPrimitive, sizeof(_uint), &dwByte, nullptr);

			//NumBones
			ReadFile(hFile, &meshLoad.iNumBones, sizeof(_uint), &dwByte, nullptr);

			//Load Vertices
			meshLoad.pVertices = new VTXANIMMODEL[meshLoad.iNumVertices];
			ZeroMemory(meshLoad.pVertices, sizeof(VTXANIMMODEL) * meshLoad.iNumVertices);

			for (_uint i = 0; i < meshLoad.iNumVertices; ++i)
			{
				ReadFile(hFile, &meshLoad.pVertices[i], sizeof(VTXANIMMODEL), &dwByte, nullptr);
				//XMStoreFloat3(&meshLoad.pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&meshLoad.pVertices[i].vPosition), PivotMatrix));
				//XMStoreFloat3(&meshLoad.pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&meshLoad.pVertices[i].vNormal), PivotMatrix));
				//XMStoreFloat3(&meshLoad.pVertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&meshLoad.pVertices[i].vTangent), PivotMatrix));
			}

			//Load Indices
			meshLoad.pIndices = new FACEINDICES32[meshLoad.iNumPrimitive];
			ZeroMemory(meshLoad.pIndices, sizeof(FACEINDICES32) * meshLoad.iNumPrimitive);

			for (_uint i = 0; i < meshLoad.iNumPrimitive; ++i)
				ReadFile(hFile, &meshLoad.pIndices[i], sizeof(FACEINDICES32), &dwByte, nullptr);

			CMesh*		pMesh = CMesh::Create(m_pDevice, m_pContext, eType, meshLoad, this);
			if (nullptr == pMesh)
				return E_FAIL;

			Safe_Delete_Array(meshLoad.pVertices);
			Safe_Delete_Array(meshLoad.pIndices);

			m_Meshes.push_back(pMesh);
		}
	}
	else if (eType == TYPE_NONANIM)
	{
		// m_iNumMeshes
		ReadFile(hFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < m_iNumMeshes; ++i)
		{
			CMesh::MeshLoadTag meshLoad;
			ZeroMemory(&meshLoad, sizeof(CMesh::MeshLoadTag));

			//MaterialIndex
			ReadFile(hFile, &meshLoad.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

			//NumVertices
			ReadFile(hFile, &meshLoad.iNumVertices, sizeof(_uint), &dwByte, nullptr);

			//NumPrimitive
			ReadFile(hFile, &meshLoad.iNumPrimitive, sizeof(_uint), &dwByte, nullptr);

			//NumBones
			ReadFile(hFile, &meshLoad.iNumBones, sizeof(_uint), &dwByte, nullptr);

			//Load Vertices
			meshLoad.pModelVertices = new VTXMODEL[meshLoad.iNumVertices];
			ZeroMemory(meshLoad.pModelVertices, sizeof(VTXMODEL) * meshLoad.iNumVertices);

			for (_uint i = 0; i < meshLoad.iNumVertices; ++i)
			{
				ReadFile(hFile, &meshLoad.pModelVertices[i], sizeof(VTXMODEL), &dwByte, nullptr);
				XMStoreFloat3(&meshLoad.pModelVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&meshLoad.pModelVertices[i].vPosition), PivotMatrix));
				XMStoreFloat3(&meshLoad.pModelVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&meshLoad.pModelVertices[i].vNormal), PivotMatrix));
				XMStoreFloat3(&meshLoad.pModelVertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&meshLoad.pModelVertices[i].vTangent), PivotMatrix));
			}

			//Load Indices
			meshLoad.pIndices = new FACEINDICES32[meshLoad.iNumPrimitive];
			ZeroMemory(meshLoad.pIndices, sizeof(FACEINDICES32) * meshLoad.iNumPrimitive);

			for (_uint i = 0; i < meshLoad.iNumPrimitive; ++i)
				ReadFile(hFile, &meshLoad.pIndices[i], sizeof(FACEINDICES32), &dwByte, nullptr);

			CMesh*		pMesh = CMesh::Create(m_pDevice, m_pContext, eType, meshLoad, this);
			if (nullptr == pMesh)
				return E_FAIL;

			Safe_Delete_Array(meshLoad.pModelVertices);
			Safe_Delete_Array(meshLoad.pIndices);

			m_Meshes.push_back(pMesh);
		}
	}


	return S_OK;
}

HRESULT CModel::Load_Materials(HANDLE hFile)
{
	DWORD	dwByte = 0;

	//m_iNumMaterials
	ReadFile(hFile, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MODELMATERIAL			ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof(ModelMaterial));

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			_uint texNum = 0;
			ReadFile(hFile, &texNum, sizeof(_uint), &dwByte, nullptr);

			if (texNum == AI_TEXTURE_TYPE_MAX)
				continue;

			//문자열 길이와 문자열 저장
			_uint TextureFilePathLen = 0;
			ReadFile(hFile, &TextureFilePathLen, sizeof(_uint), &dwByte, nullptr);

			_tchar* TextureFilePath = new _tchar[TextureFilePathLen];
			ReadFile(hFile, TextureFilePath, sizeof(_tchar) * TextureFilePathLen, &dwByte, nullptr);

			ModelMaterial.pTexture[texNum] = CTexture::Create(m_pDevice, m_pContext, TextureFilePath);

			if (nullptr == ModelMaterial.pTexture[texNum])
			{
				Safe_Delete_Array(TextureFilePath);
				return E_FAIL;
			}
			Safe_Delete_Array(TextureFilePath);
		}
		m_Materials.push_back(ModelMaterial);
	}

	return S_OK;
}

HRESULT CModel::Load_Bones(HANDLE hFile, CBone* pParent)
{
	DWORD	dwByte = 0;

	//BoneName
	char szName[MAX_PATH];
	ReadFile(hFile, szName, sizeof(char) * MAX_PATH, &dwByte, nullptr);

	//TransformMatrix
	_float4x4 BoneTransformMatrix;
	ReadFile(hFile, &BoneTransformMatrix, sizeof(_float4x4), &dwByte, nullptr);

	CBone::BONELOAD tBoneLoad;
	ZeroMemory(&tBoneLoad, sizeof(CBone::BONELOAD));

	strcpy_s(tBoneLoad.mName, szName);
	tBoneLoad.mTransformation = BoneTransformMatrix;

	CBone* pBone = CBone::Create(tBoneLoad, pParent);

	if (pBone == nullptr)
		return E_FAIL;

	m_iNumBones++;
	m_Bones.push_back(pBone);


	//NumChildren
	_uint NumChildren;
	ReadFile(hFile, &NumChildren, sizeof(_uint), &dwByte, nullptr);

	/* 재귀 함수를 돌면서 모든 뼈를 준비해준다.*/
	for (_uint i = 0; i < NumChildren; ++i)
		Load_Bones(hFile, pBone);

	return S_OK;
}

HRESULT CModel::Load_Animations(HANDLE hFile)
{
	DWORD	dwByte = 0;

	//m_iNumAnimations
	ReadFile(hFile, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation::ANIMATIONLOAD tAnimationLoad;

		ReadFile(hFile, tAnimationLoad.mName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &tAnimationLoad.mDuration, sizeof(_double), &dwByte, nullptr);
		ReadFile(hFile, &tAnimationLoad.mTicksPerSecond, sizeof(_double), &dwByte, nullptr);
		ReadFile(hFile, &tAnimationLoad.mIsLooping, sizeof(_bool), &dwByte, nullptr);
		ReadFile(hFile, &tAnimationLoad.mNumChannels, sizeof(_uint), &dwByte, nullptr);

		for (_uint j = 0; j < tAnimationLoad.mNumChannels; ++j)
		{
			CChannel::CHANNELLOAD tChannelLoad;
			ReadFile(hFile, tChannelLoad.mNodeName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, &tChannelLoad.mNumKeyFrames, sizeof(_uint), &dwByte, nullptr);

			for (_uint k = 0; k < tChannelLoad.mNumKeyFrames; ++k)
			{
				KEYFRAME KeyFrame;
				ZeroMemory(&KeyFrame, sizeof(KEYFRAME));
				ReadFile(hFile, &KeyFrame, sizeof(KEYFRAME), &dwByte, nullptr);
				tChannelLoad.mKeyFrames.push_back(KeyFrame);
			}
			tAnimationLoad.pvecChannels.push_back(tChannelLoad);
		}
		CAnimation*		pAnim = CAnimation::Create(tAnimationLoad, this);
		if (nullptr == pAnim)
			return E_FAIL;

		m_Animations.push_back(pAnim);
		m_strAnimationName.push_back(pAnim->Get_AnimationName());
	}
	return S_OK;
}

CAnimation * CModel::FindAnim(const string & strAnim)
{
	auto itr = find_if(m_Animations.begin(), m_Animations.end(),
		[strAnim](CAnimation* pAnim) {
		return  0 == strcmp(strAnim.c_str(), pAnim->Get_Name());
	});

	return *itr;
}

CAnimation * CModel::Get_IndexAnim(_uint iIndex)
{
	return m_Animations[iIndex];
}

CAnimation * CModel::Get_CurAnim()
{
	if (m_eType == CModel::TYPE_NONANIM)
		return nullptr;

	return m_Animations[m_iCurrentAnimIndex];
}

void CModel::Set_AnimPlaySpeed(_double fPlaySpeed)
{
	for (auto& pAnimation : m_Animations)
		pAnimation->Set_MulSecond(fPlaySpeed);
}

void CModel::Set_PlayRate(_double playRate)
{
	m_Animations[m_iCurrentAnimIndex]->Set_PlayRate(playRate);
}

void CModel::Last_AnimLoop(_uint iIndex)
{
	m_Animations[iIndex]->FinishAnimLoop();
}

void CModel::Reset_AnimPlayTime(_uint iIndex)
{
	m_Animations[iIndex]->Reset();
}

_bool CModel::Check_AnimationSet(const _float & fTime)
{
	return m_Animations[m_iCurrentAnimIndex]->Check_AnimationSet(fTime);
}

void CModel::Model_Change(_uint iIndex, double playTime)
{
	m_Animations[iIndex]->Set_PlayRate(playTime);
}

void CModel::Model_IndexChange(_uint iCurrent, _uint iPre, _uint iAdditive)
{
	m_iCurrentAnimIndex = iCurrent;
	m_iPreAnimIndex = iPre;
	m_iAdditiveAnimIndex = iAdditive;
}

_double CModel::Get_AnimPlayTime(_uint iIndex)
{
	return m_Animations[iIndex]->Get_PlayTime();
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const _tchar* pModelFilePath, _fmatrix PivotMatrix)
{
	CModel*		pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(eType, pModelFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*		pInstance = new CModel(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();

	for (auto& pMaterial : m_Materials)
	{
		for (int i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(pMaterial.pTexture[i]);
	}
	m_Materials.clear();

	for (auto& pBones : m_Bones)
		Safe_Release(pBones);
	m_Bones.clear();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();
}