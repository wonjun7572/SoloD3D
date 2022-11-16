#pragma once

#include "Component.h"

/* ��ü�� ���� �����̽� �󿡼��� ���¸� �������ش�.(m_WorldMatrix) */
/* ���º�ȯ�� ���� �������̽��� �������ش�.(Go_Straight, Turn, Chase, LookAt) */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	/* STATE_TRANSLATION : (���� �Ǵ� �θ�) �������κ��� �󸶳� �������־�. */
	/* Position : �������� ������������� ��ġ. */
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_TRANSLATION, STATE_END };

public:
	typedef struct tagTransformDesc
	{
		/* �ʴ� ���������ϴ� �ӵ�. */
		float		fSpeedPerSec;
		/* �ʴ� ȸ���ؾ��ϴ� �ӵ�. */
		float		fRotationPerSec;
	}TRANSFORMDESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	
	_vector Get_State(STATE eState) const {	return XMLoadFloat4x4(&m_WorldMatrix).r[eState]; }

	/* ���Ϲ��� ����� �̿��� ������ �ؾ��� ��. */
	_matrix Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }

	_matrix Get_WorldMatrixInverse() const { return XMMatrixInverse(nullptr, Get_WorldMatrix()); }

	/* ���Ϲ��� ��ĺ����ؾ��� ��  */
	_float4x4 Get_World4x4() const { return m_WorldMatrix; }

	_float3 Get_Scaled() const 
	{
		return _float3(XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT))),
					   XMVectorGetX(XMVector3Length(Get_State(STATE_UP))),
					   XMVectorGetX(XMVector3Length(Get_State(STATE_LOOK))));
	}
	
	void Set_State(STATE eState, _fvector vState) 
	{
		_float4		vTmp;
		XMStoreFloat4(&vTmp, vState);
		memcpy(&m_WorldMatrix.m[eState][0], &vTmp, sizeof vTmp);
	}
	
	void Set_Scaled(STATE eState, _float fScale); /* fScale������ ���̸� �����Ѵ�. */
	void Set_Scaled(_float3 vScale); /* fScale������ ���̸� �����Ѵ�. */
	void Scaling(STATE eState, _float fScale); /* fScale����� �ø���. */

	void Set_TransformDesc(const TRANSFORMDESC& TransformDesc) { m_TransformDesc = TransformDesc; }

public:
	virtual HRESULT Init_Prototype();
	virtual HRESULT Init(void* pArg);

public:
	void Go_Straight(_double TimeDelta);
	void Go_Backward(_double TimeDelta);
	void Go_Left(_double TimeDelta);
	void Go_Right(_double TimeDelta);
	void Go_Up(_double TimeDelta);

	// Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	void Turn(_fvector vAxis, _double TimeDelta); /* Dynamic */
	void Rotation(_fvector vAxis, _float fRadian); /* Static */

	/* �Ĵٺ���. */
	/*void LookAt(const CTransform* pTarget);*/
	void LookAt(_fvector vTargetPos);

	/* �����Ѵ� .*/
	void Chase(_fvector vTargetPos, _double TimeDelta, _float fLimit = 0.1f);

public:
	HRESULT Bind_ShaderResource(class CShader* pShaderCom, const char* pConstantName);

private:
	_float4x4				m_WorldMatrix;
	TRANSFORMDESC			m_TransformDesc;
	
public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END