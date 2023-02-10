#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

class CArcaneE final : public CGameObject
{
public:
	typedef struct ArcaneDesc
	{
		_float4 vPos;
	}DESC;

private:
	CArcaneE(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CArcaneE(const CArcaneE& rhs);
	virtual ~CArcaneE() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	void	Conversation(_double TimeDelta);

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	CShader*						m_pShaderCom = nullptr;
	CTexture*						m_pTextureCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

	CGameObject*	m_pConversationUI = nullptr;

	_double	TimeConversation = 0.0;
	wstring		m_strConversation;
	_bool		m_bConversation = true;

	_float m_fFrame = { 0.f };

	_bool	m_bFinished = false;

public:
	static CArcaneE* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

