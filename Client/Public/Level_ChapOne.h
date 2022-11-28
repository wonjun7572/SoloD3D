#pragma once

#include "Client_Define.h"
#include "Level.h"

BEGIN(Client)

class CLevel_ChapOne final : public CLevel
{
private:
	CLevel_ChapOne(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_ChapOne() = default;

public:
	virtual HRESULT Init() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_BackGround(const wstring& pLayerTag);
	HRESULT Ready_Layer_Camera(const wstring& pLayerTag);
	HRESULT Ready_Layer_Player(const wstring& pLayerTag);
	HRESULT Ready_Layer_Statue(const wstring& pLayerTag);

public:
	static CLevel_ChapOne* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free() override;
};

END