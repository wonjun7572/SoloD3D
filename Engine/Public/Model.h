#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint			Get_NumMeshes() const { return m_iNumMeshes; }
	class CBone*	Get_BonePtr(const char* pBoneName);

	class CBone*	Get_SelectedBone() { return m_pSelectedBone; }
	_float4x4		Get_PivotMatrix() { return m_PivotMatrix; }

public:
	virtual HRESULT Init_Prototype(TYPE eType, const _tchar* pModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Init(void* pArg);

public:
	void	Play_Animation(_double TimeDelta);
	void	Play_AddtivieAnim(_double TimeDelta , _float fRatio);

	void	Set_AdditiveAnimIndex(_uint iIndex) {m_iAdditiveAnimIndex = iIndex; }
	
	HRESULT Bind_Material(class CShader * pShader, _uint iMeshIndex, TextureType eType, const char* pConstantName);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex,  _uint iPassIndex = 0, const char* pBoneConstantName = nullptr);

	void	Set_AnimationIndex(_uint iIndex , _double time = 1.0);
	
	vector<const char*>&	Get_AnimationName() { return m_strAnimationName; }
	_uint	Get_AnimationsNum() { return m_iNumAnimations; }

	virtual void Imgui_RenderProperty() override;

	HRESULT  Load_MesheContainers(HANDLE hFile, TYPE eType, _fmatrix PivotMatrix);
	HRESULT  Load_Materials(HANDLE hFile);
	HRESULT  Load_Bones(HANDLE hFile, class CBone* pParent);
	HRESULT  Load_Animations(HANDLE hFile);

	class CAnimation* FindAnim(const string& strAnim);
	class CAnimation* Get_IndexAnim(_uint iIndex);
	class CAnimation* Get_CurAnim();			

	void	Set_BlendCurTime(_float BlendCurTime) { m_fBlendCurTime = BlendCurTime; }
	void	Set_AnimPlaySpeed(_double fPlaySpeed);
	void	Last_AnimLoop(_uint iIndex);
	void    Reset_AnimPlayTime(_uint iIndex);

	_bool	Check_AnimationSet(const _float& fTime);
	//_float4	Get_MovePos(void) { return m_vMovePos; }
	void	Set_BlendDuration(_float fDuration) { m_fBlendDuration = fDuration; }

	/* Model Change */
	void	Model_Change(_uint iIndex, double playTime);
	void	Model_IndexChange(_uint iCurrent, _uint iPre, _uint iAdditive);

	_double	Get_AnimPlayTime(_uint iIndex);

	_uint Get_CurrentAnimIndex() { return m_iCurrentAnimIndex; }
	_uint Get_PreAnimIndex() { return m_iPreAnimIndex; }
	_uint Get_AdditiveAnimIndex() { return m_iAdditiveAnimIndex; }

private:
	TYPE									m_eType = TYPE_END;

	/* 하나의 모델은 교체가 가능한 여러개의 메시로 구성되어있다. */
	_uint									m_iNumMeshes = 0;
	vector<class CMesh*>					m_Meshes;

	_uint									m_iNumMaterials = 0;
	vector<MODELMATERIAL>					m_Materials;

	_uint									m_iNumBones = 0;
	vector<class CBone*>					m_Bones;

	_uint									m_iCurrentAnimIndex = 0;
	_uint									m_iPreAnimIndex = 0;
	_uint									m_iAdditiveAnimIndex = 0;

	_uint									m_iNumAnimations = 0;
	vector<class CAnimation*>				m_Animations;
	vector<const char*>						m_strAnimationName;

	class CBone*							m_pSelectedBone = nullptr;

	_float4x4								m_PivotMatrix;

	_tchar									m_FilePath[MAX_PATH];

	_float									m_fBlendCurTime = 0.2f;
	_float									m_fBlendDuration = 0.2f;

	//_float4								m_vMovePos = _float4(0.f, 0.f, 0.f, 0.1f);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _tchar* pModelFilePath, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END