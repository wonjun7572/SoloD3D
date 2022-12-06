#pragma once

#include "VIBuffer.h"
#include "Model.h"
/* ���� ��ü������ �� ����. */
/* �� �޽ø� �׷��������� ����, �ε��� ���۸� �����Ѵ�. */
BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	virtual HRESULT Init_Prototype(CModel::TYPE eType, aiMesh* pAIMesh, class CModel* pModel);
	virtual HRESULT Init(void* pArg) override;

public:
	_uint	Get_MaterialIndex() { return m_iMaterialIndex; }
	const char* Get_Name() const { return m_szName; }

public:
	void SetUp_MeshBones(class CModel* pModel);
	void SetUp_BoneMatrices(_float4x4* pBoneMatrices, _float4x4 PivotMatrix);

private:
	aiMesh*				m_pAIMesh = nullptr;
	CModel::TYPE		m_eType;

	/* �� �޽ô� m_iMaterialIndex��° ���׸����� ����Ѵ�. */
	_uint				m_iMaterialIndex = 0;

	/* �� �޽��� �����鿡�� ������ �ִ� ���� ����. */
	_uint					m_iNumBones = 0;
	vector<class CBone*>	m_Bones;

	char					m_szName[MAX_PATH];

private:
	HRESULT Ready_VertexBuffer_NonAnimModel(aiMesh* pAIMesh, class CModel* pModel);
	HRESULT Ready_VertexBuffer_AnimModel(aiMesh* pAIMesh, class CModel* pModel);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, aiMesh* pAIMesh, class CModel* pModel);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free();
};

END