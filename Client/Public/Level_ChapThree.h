#pragma once

#include "Client_Define.h"
#include "Level.h"

BEGIN(Client)

class CLevel_ChapThree final : public CLevel
{
private:
	CLevel_ChapThree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_ChapThree() = default;

public:
	virtual HRESULT Init() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void ImguiRenderTab();

private:
	HRESULT Ready_Ligths();
	HRESULT Ready_Layer_BackGround(const wstring& pLayerTag);
	HRESULT Ready_Layer_Camera(const wstring& pLayerTag);
	HRESULT Ready_Layer_Player(const wstring& pLayerTag);
	HRESULT Ready_Layer_Monster(const wstring& pLayerTag);
	HRESULT Ready_Layer_MapObject(const wstring& pLayerTag);

	wstring m_pSaveMapObjectFilePath;
	wstring m_pLoadMapObjectFilePath;

public:
	static CLevel_ChapThree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free() override;
};

END