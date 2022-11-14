#pragma once

#include "Component.h"

/* 객체의 월드 스페이스 상에서의 상태를 저장해준다.(m_WorldMatrix) */
/* 상태변환을 위한 인터페이스를 제공해준다.(Go_Straight, Turn, Chase, LookAt) */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	/* STATE_TRANSLATION : (원점 또는 부모) 기준으로부터 얼마나 떨어져있어. */
	/* Position : 절대적인 월드공간에서의 위치. */
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_TRANSLATION, STATE_END };

public:
	typedef struct tagTransformDesc
	{
		/* 초당 움직여야하는 속도. */
		float		fSpeedPerSec;
		/* 초당 회전해야하는 속도. */
		float		fRotationPerSec;
	}TRANSFORMDESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	
	_vector Get_State(STATE eState) const {	return XMLoadFloat4x4(&m_WorldMatrix).r[eState]; }

	/* 리턴받은 행렬을 이용해 연산을 해야할 때. */
	_matrix Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }

	_matrix Get_WorldMatrixInverse() const { return XMMatrixInverse(nullptr, Get_WorldMatrix()); }

	/* 리턴받은 행렬보관해야할 때  */
	_float4x4 Get_World4x4() const { return m_WorldMatrix; }

	/* 리턴받은 행렬을 셰이더에 던지기위해.  */
	/*_float4x4 Get_World4x4_TP() const 
	{
		_float4x4	TransposeMatrix;
		XMStoreFloat4x4(&TransposeMatrix, XMMatrixTranspose(Get_WorldMatrix()));
		return TransposeMatrix;
	}*/

	_float3 Get_Scale()	{ return m_Scale; }

	_float Get_Scale(STATE eState) { return XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_WorldMatrix).r[eState]));}

	void Set_State(STATE eState, _fvector vState) 
	{
		_float4		vTmp;
		XMStoreFloat4(&vTmp, vState);
		memcpy(&m_WorldMatrix.m[eState][0], &vTmp, sizeof vTmp);
	}
	
	void Set_Scale(STATE eState, _float fScale);
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

public:
	void Turn(_fvector vAxis, _double TimeDelta);
	void LookAt(_fvector vAt);
	void Record_Scale(_float3 _Scale) { m_Scale = _Scale; }

private:
	_float4x4				m_WorldMatrix;
	TRANSFORMDESC			m_TransformDesc;
	_float3					m_Scale = { 1.0f,1.0f,1.0f };

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END