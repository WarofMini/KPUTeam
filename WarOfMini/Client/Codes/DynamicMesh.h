#ifndef DynamicMesh_h__
#define DynamicMesh_h__

#include "Mesh.h"

class CGameObject;
class CAnimationInfo;
class CAnimation;

class CDynaicMesh
	: public CMesh
{
private:
	explicit CDynaicMesh(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);
	virtual ~CDynaicMesh(void);

public:
	static CDynaicMesh* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, const VTXBONE* pVB, const _uint& uiVtxCnt
		, const _uint* pIB, const _uint& uiIdxCnt, const _vec3& vMin, const _vec3& vMax, _tchar* pTexName, CAnimation* pAnimation);

public:
	virtual CResource* Clone_Resource(void);
	virtual void Render(_bool bColliderDraw = FALSE);
	void RenderAnim(CAnimationInfo* pAnimInfo, _bool bColliderDraw = FALSE);
	virtual void RenderInst(const vector<_matrix*>& vecObjWorld);
	virtual void Release(void);

public:
	_matrix Get_TransBoneMatrix(_int iIndex);
	void Add_ObbCheckList(const _matrix* pWorld, const CGameObject* pObj);

private:
	HRESULT Create_Buffer(const VTXBONE* pVB, const _uint& uiVtxCnt, const _uint* pIB, const _uint& uiIdxCnt
		, const _vec3& vMin, const _vec3& vMax, _tchar* pTexName, CAnimation* pAnimation);

private:
	virtual HRESULT Set_BoundingBox(void);

private:
	VTXBONE*		m_pOriVertex;
	VTXBONE*		m_pVertex;
	_uint*			m_pIndex;
	_matrix*		m_pBoneWorld;

private:
	CAnimation*		m_pAnimation;
	_vec3		m_vPivotPos;
};

#endif // DynamicMesh_h__