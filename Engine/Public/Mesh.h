#pragma once

#include "VIBuffer.h"

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
	virtual HRESULT Init_Prototype(aiMesh* pAIMesh);
	virtual HRESULT Init(void* pArg) override;

public:
	_uint	Get_MaterialIndex() { return m_iMaterialIndex; }

private:
	/* �� �޽ô� m_iMaterialIndex��° ���׸����� ����Ѵ�. */
	_uint				m_iMaterialIndex = 0;

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, aiMesh* pAIMesh);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free();
};

END