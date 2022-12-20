#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect_Instancing;
END

BEGIN(Client)

class CEffect_Rect final : public CGameObject
{
private:
	CEffect_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Rect(const CEffect_Rect& rhs);
	virtual ~CEffect_Rect() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_Rect_Instancing*	m_pVIBufferCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CEffect_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END