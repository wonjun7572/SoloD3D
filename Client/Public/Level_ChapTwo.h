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

public:
	static CLevel_ChapTwo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free() override;
};

END