#include "..\public\Model.h"
#include "GameInstance.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Bone.h"
#include "Animation.h"
#include "MathUtils.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pAIScene(rhs.m_pAIScene)
	, m_eType(rhs.m_eType)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	/*, m_Meshes(rhs.m_Meshes)*/
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_iNumBones(rhs.m_iNumBones)
	/*, m_Bones(rhs.m_Bones)*/
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	/*, m_Animations(rhs.m_Animations)*/
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
}

CBone * CModel::Get_BonePtr(const char * pBoneName)
{
	auto iter = find_if(m_Bones.begin(), m_Bones.end(),[&](CBone* pBone)->_bool {
		return !strcmp(pBoneName, pBone->Get_Name());
	});

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;
}

HRESULT CModel::Init_Prototype(TYPE eType, const char * pModelFilePath, _fmatrix PivotMatrix)
{
	_uint			iFlag = 0;

	m_eType = eType;

	if (TYPE_NONANIM == eType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (nullptr == m_pAIScene)
		return E_FAIL;

	if (FAILED(Ready_MeshContainers(eType)))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Init(void * pArg)
{
	if (FAILED(Ready_Bones(m_pAIScene->mRootNode)))
		return E_FAIL;

	for (auto& pMesh : m_Meshes)
		pMesh->SetUp_MeshBones(this);

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	return S_OK;
}

void CModel::Play_Animation(_double TimeDelta)
{
	m_Animations[m_iCurrentAnimIndex]->Update_Bones(TimeDelta);
	
	for (auto& pBone : m_Bones)
	{
		if (pBone != nullptr)
			pBone->Compute_CombindTransformationMatrix();
	}
}

HRESULT CModel::Bind_Material(CShader * pShader, _uint iMeshIndex, aiTextureType eType, const char * pConstantName)
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
		//if (ImGui::BeginTabItem("Bone"))
		//{
		//	if (ImGui::BeginListBox("##"))
		//	{
		//		for (auto& pBone : m_Bones)
		//		{
		//			const bool bSelected = m_pSelectedBone == pBone;

		//			if (bSelected)
		//				ImGui::SetItemDefaultFocus();

		//			if (ImGui::Selectable(pBone->Get_Name(), bSelected))
		//				m_pSelectedBone = pBone;
		//		}
		//		ImGui::EndListBox();
		//	}

		//	if (m_pSelectedBone != nullptr)
		//	{
		//		static float snap[3] = { 1.f, 1.f, 1.f };
		//		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
		//		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
		//		if (ImGui::IsKeyPressed(90))
		//			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		//		if (ImGui::IsKeyPressed(69))
		//			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		//		if (ImGui::IsKeyPressed(82)) // r Key
		//			mCurrentGizmoOperation = ImGuizmo::SCALE;
		//		if (ImGui::RadioButton("Translate_BONE", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		//			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		//		ImGui::SameLine();
		//		if (ImGui::RadioButton("Rotate_BONE", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		//			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		//		ImGui::SameLine();
		//		if (ImGui::RadioButton("Scale_BONE", mCurrentGizmoOperation == ImGuizmo::SCALE))
		//			mCurrentGizmoOperation = ImGuizmo::SCALE;

		//		float matrixTranslation[3], matrixRotation[3], matrixScale[3];

		//		ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float*>(&CMathUtils::Mul_Matrix(CMathUtils::Mul_Matrix(m_pSelectedBone->Get_OffsetMatrix(), m_pSelectedBone->Get_CombindMatrix()), m_PivotMatrix)), matrixTranslation, matrixRotation, matrixScale);
		//		ImGui::InputFloat3("Translate_BONE", matrixTranslation);
		//		ImGui::InputFloat3("Rotate_BONE", matrixRotation);
		//		ImGui::InputFloat3("Scale_BONE", matrixScale);
		//		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, reinterpret_cast<float*>(&CMathUtils::Mul_Matrix(CMathUtils::Mul_Matrix(m_pSelectedBone->Get_OffsetMatrix(), m_pSelectedBone->Get_CombindMatrix()), m_PivotMatrix)));

		//		if (mCurrentGizmoOperation != ImGuizmo::SCALE)
		//		{
		//			if (ImGui::RadioButton("Local_BONE", mCurrentGizmoMode == ImGuizmo::LOCAL))
		//				mCurrentGizmoMode = ImGuizmo::LOCAL;
		//			ImGui::SameLine();
		//			if (ImGui::RadioButton("World_BONE", mCurrentGizmoMode == ImGuizmo::WORLD))
		//				mCurrentGizmoMode = ImGuizmo::WORLD;
		//		}

		//		static bool useSnap(false);
		//		if (ImGui::IsKeyPressed(83))
		//			useSnap = !useSnap;
		//		ImGui::Checkbox("##something_BONE", &useSnap);
		//		ImGui::SameLine();
		//		switch (mCurrentGizmoOperation)
		//		{
		//		case ImGuizmo::TRANSLATE:
		//			ImGui::InputFloat3("Snap_BONE", &snap[0]);
		//			break;
		//		case ImGuizmo::ROTATE:
		//			ImGui::InputFloat("Angle Snap_BONE", &snap[0]);
		//			break;
		//		case ImGuizmo::SCALE:
		//			ImGui::InputFloat("Scale Snap_BONE", &snap[0]);
		//			break;
		//		}

		//		ImGuiIO& io = ImGui::GetIO();
		//		RECT rt;
		//		GetClientRect(CGameInstance::GetInstance()->GetHWND(), &rt);
		//		POINT lt{ rt.left, rt.top };
		//		ClientToScreen(CGameInstance::GetInstance()->GetHWND(), &lt);
		//		ImGuizmo::SetRect((_float)lt.x, (_float)lt.y, io.DisplaySize.x, io.DisplaySize.y);

		//		_float4x4 matView, matProj;
		//		XMStoreFloat4x4(&matView, CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		//		XMStoreFloat4x4(&matProj, CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

		//		ImGuizmo::Manipulate(
		//			reinterpret_cast<float*>(&matView),
		//			reinterpret_cast<float*>(&matProj),
		//			mCurrentGizmoOperation,
		//			mCurrentGizmoMode,
		//			reinterpret_cast<float*>(&CMathUtils::Mul_Matrix(CMathUtils::Mul_Matrix(m_pSelectedBone->Get_OffsetMatrix(), m_pSelectedBone->Get_CombindMatrix()), m_PivotMatrix)),
		//			nullptr, useSnap ? &snap[0] : nullptr);
		//	}
		//	ImGui::EndTabItem();
		//}
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
	ImGui::End();
}

HRESULT CModel::Ready_Bones(aiNode * pNode, CBone* pParent)
{
	CBone* pBone = CBone::Create(pNode, pParent);

	if (pBone == nullptr)
		return E_FAIL;

	m_Bones.push_back(pBone);

	/* 재귀 함수를 돌면서 모든 뼈를 준비해준다.*/
	for (_uint i = 0; i < pNode->mNumChildren; ++i)
		Ready_Bones(pNode->mChildren[i], pBone);

	return S_OK;
}

HRESULT CModel::Ready_MeshContainers(TYPE eType)
{
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_iNumMeshes = m_pAIScene->mNumMeshes;

	m_Meshes.reserve(m_iNumMeshes);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		aiMesh*		pAIMesh = m_pAIScene->mMeshes[i];

		CMesh*		pMesh = CMesh::Create(m_pDevice, m_pContext, eType, pAIMesh, this);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const char* pModelFilePath)
{
	char		szDirectory[MAX_PATH] = "";

	_splitpath_s(pModelFilePath, nullptr, 0, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

	/* 텍스쳐에 기록되어있는 픽셀단위 재질 정보를 로드한다. */
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		char		szTextureFileName[MAX_PATH] = "";
		char		szExt[MAX_PATH] = "";
		char		szTexturePath[MAX_PATH] = "";

		MODELMATERIAL			ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof(ModelMaterial));

		aiMaterial*		pAIMaterial = m_pAIScene->mMaterials[i];
		
		strcpy_s(ModelMaterial.MaterialName, pAIMaterial->GetName().C_Str());

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString			strTexturePath;

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath)))
				continue;

			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szTextureFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szTexturePath, szDirectory);
			strcat_s(szTexturePath, szTextureFileName);
			strcat_s(szTexturePath, szExt);

			_tchar			szFullPath[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, szTexturePath, static_cast<int>(strlen(szTexturePath)), szFullPath, MAX_PATH);
			ModelMaterial.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath);

			if (nullptr == ModelMaterial.pTexture[j])
				return E_FAIL;
		}
		m_Materials.push_back(ModelMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animation()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		aiAnimation*		pAIAnimation = m_pAIScene->mAnimations[i];

		CAnimation*			pAnim = CAnimation::Create(pAIAnimation, this);
		if (nullptr == pAnim)
			return E_FAIL;

		m_Animations.push_back(pAnim);
		m_strAnimationName.push_back(pAnim->Get_AnimationName());
	}

	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const char * pModelFilePath, _fmatrix PivotMatrix)
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
		{
			Safe_Release(pMaterial.pTexture[i]);
		}
	}
	m_Materials.clear();

	for (auto& pBones : m_Bones)
		Safe_Release(pBones);
	m_Bones.clear();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	m_Importer.FreeScene();
}
