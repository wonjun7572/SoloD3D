#pragma once

#include "VIBuffer.h"
#include "Model.h"
/* 모델의 교체가능한 한 파츠. */
/* 이 메시를 그려내기위한 정점, 인덱스 버퍼를 보유한다. */
BEGIN(Engine)

class CMesh final : public CVIBuffer
{
public:
	typedef struct MeshLoadTag
	{
		_uint iMaterialIndex;
		_uint iNumVertices;
		_uint iNumPrimitive;
		_uint iNumBones;
		VTXMODEL* pModelVertices;
		VTXANIMMODEL* pVertices;
		FACEINDICES32* pIndices;
	}MESHTAG;

private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	virtual HRESULT Init_Prototype(CModel::TYPE eType, MeshLoadTag& tMeshLoad, class CModel* pModel);
	virtual HRESULT Init(void* pArg) override;

public:
	_uint	Get_MaterialIndex() { return m_iMaterialIndex; }

public:
	void Load_MeshBones(class CModel* pModel, HANDLE hFile);
	void SetUp_BoneMatrices(_float4x4* pBoneMatrices, _float4x4 PivotMatrix);

private:
	CModel::TYPE			m_eType;

	/* 이 메시는 m_iMaterialIndex번째 머테리얼을 사용한다. */
	_uint					m_iMaterialIndex = 0;

	/* 이 메시의 정점들에게 영향을 주는 뼈의 갯수. */
	_uint					m_iNumBones = 0;
	vector<class CBone*>	m_Bones;

private:
	HRESULT Ready_VertexBuffer_NonAnimModel(MeshLoadTag& tMeshLoad, class CModel* pModel);
	HRESULT Ready_VertexBuffer_AnimModel(MeshLoadTag& tMeshLoad, class CModel* pModel);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, MeshLoadTag tMeshLoad, CModel* pModel);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free();
};

END