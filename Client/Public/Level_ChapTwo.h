#pragma once

#include "Client_Define.h"
#include "Level.h"

BEGIN(Client)

class CLevel_ChapTwo final : public CLevel
{
private:
	CLevel_ChapTwo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_ChapTwo() = default;

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
	HRESULT Ready_Layer_Ally(const wstring& pLayerTag);
	HRESULT Ready_Layer_Monster(const wstring& pLayerTag);
	HRESULT Ready_Layer_MapObject(const wstring& pLayerTag);
	HRESULT	Ready_Layer_Tree(const wstring& pLayerTag);
	HRESULT Ready_Layer_TorchLight(const wstring& pLayerTag);
	HRESULT Ready_Layer_Effect(const wstring& pLayerTag);

	wstring m_pSaveMapObjectFilePath;
	wstring m_pLoadMapObjectFilePath;

	_bool	m_bDemonCreate = false;

	_double m_TimeDelta = 0;

public:
	static CLevel_ChapTwo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free() override;
};

END