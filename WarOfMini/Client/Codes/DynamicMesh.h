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
		, const _uint* pIB, const _uint& uiIdxCnt, const XMFLOAT3& vMin, const XMFLOAT3& vMax, _tchar* pTexName, CAnimation* pAnimation);

public:
	virtual CResource* Clone_Resource(void);
	virtual void Render(_uint uiTextureNumber, _bool bColliderDraw = FALSE);
	void RenderAnim(CAnimationInfo* pAnimInfo, MATNODE* pMatNode, _uint uiTextureNumber, _ubyte byColor = 0, _bool bColliderDraw = FALSE);
	virtual void RenderInst(const vector<XMFLOAT4X4*>& vecObjWorld);
	virtual void Release(void);

public:
	void CreateBoneNode(MATNODE* pMatNode);
	XMFLOAT4X4 Get_TransBoneMatrix(_int iIndex, _int iIndex2, MATNODE* pMatNode);
	void Add_ObbCheckList(const XMFLOAT4X4* pWorld, const CGameObject* pObj, MATNODE* pMatNode);

private:
	HRESULT Create_Buffer(const VTXBONE* pVB, const _uint& uiVtxCnt, const _uint* pIB, const _uint& uiIdxCnt
		, const XMFLOAT3& vMin, const XMFLOAT3& vMax, _tchar* pTexName, CAnimation* pAnimation);

private:
	virtual HRESULT Set_BoundingBox(void);

private:
	VTXBONE*		m_pOriVertex;
	VTXBONE*		m_pVertex;
	_uint*			m_pIndex;

private:
	CAnimation*		m_pAnimation;
	XMFLOAT3		m_vPivotPos;
};

#endif // DynamicMesh_h__