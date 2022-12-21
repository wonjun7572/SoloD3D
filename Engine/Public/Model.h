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

	class CBone*		Get_SelectedBone() { return m_pSelectedBone; }
	_float4x4			Get_PivotMatrix() { return m_PivotMatrix; }

public:
	virtual HRESULT Init_Prototype(TYPE eType, const _tchar* pModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Init(void* pArg);

public:
	void	Play_Animation(_double TimeDelta);
	
	HRESULT Bind_Material(class CShader * pShader, _uint iMeshIndex, TextureType eType, const char* pConstantName);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex,  _uint iPassIndex = 0, const char* pBoneConstantName = nullptr);

	void	Set_AnimationIndex(_uint iIndex);
	void	Set_UpperAnimationIndex(_uint iUpperIndex, _uint iUnderIndex);
	
	vector<const char*>&	Get_AnimationName() { return m_strAnimationName; }
	_uint	Get_AnimationsNum() { return m_iNumAnimations; }

	virtual void Imgui_RenderProperty() override;

	HRESULT  Load_MesheContainers(HANDLE hFile, TYPE eType, _fmatrix PivotMatrix);
	HRESULT  Load_Materials(HANDLE hFile);
	HRESULT  Load_Bones(HANDLE hFile, class CBone* pParent);
	HRESULT  Load_Animations(HANDLE hFile);

	class CAnimation* FindAnim(const string& strAnim);
	class CAnimation* Get_IndexAnim(_uint iIndex) { return m_Animations[iIndex]; }
	class CAnimation* Get_CurAnim() { return m_Animations[m_iCurrentAnimIndex]; }

	_bool	Check_AnimationSet(const _float& fTime);
	void	TurnOn_UpperAnim(_bool isUpper) { m_isUpper = isUpper; }

	_float4				Get_MovePos(void) { return m_vMovePos; }

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

	_uint									m_iCurUpperAnimIndex = 0;
	_uint									m_iCurUnderAnimIndex = 0;

	_uint									m_iPreAnimIndex = 0;

	_uint									m_iPreUpperAnimIndex = 0;
	_uint									m_iPreUnderAnimIndex = 0;

	_uint									m_iNumAnimations = 0;
	vector<class CAnimation*>				m_Animations;
	vector<const char*>						m_strAnimationName;

	class CBone*							m_pSelectedBone = nullptr;

	_float4x4								m_PivotMatrix;

	_tchar									m_FilePath[MAX_PATH];

	_float									m_fBlendCurTime = 0.2f;

	_float									m_fBlendCurUpperTime = 0.2f;
	_float									m_fBlendCurUnderTime = 0.2f;

	_float									m_fBlendDuration = 0.2f;

	_bool									m_isUpper = false;

	_float4									m_vMovePos = _float4(0.f, 0.f, 0.f, 0.1f);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _tchar* pModelFilePath, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END