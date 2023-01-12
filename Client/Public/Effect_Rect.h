#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
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

	void	Imgui_RenderProperty() override;

	void	LinkObject(_double TimeDelta, _fvector targetpos);

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_Rect*				m_pVIBufferCom = nullptr;


	_float						m_fAlpha = 0.f;
	_float2						m_UVMoveFactor;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CEffect_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END