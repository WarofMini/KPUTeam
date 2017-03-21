#ifndef StaticMesh_h__
#define StaticMesh_h__

#include "Mesh.h"

class CStaticMesh
	: public CMesh
{
private:
	explicit CStaticMesh(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);
	virtual ~CStaticMesh(void);

public:
	static CStaticMesh* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, const VTXTEX* pVB, const _uint& uiVtxCnt
		, const _uint* pIB, const _uint& uiIdxCnt, const _vec3& vMin, const _vec3& vMax, _tchar* pTexName);

public:
	virtual CResource* Clone_Resource(void);
	virtual void Render(_bool bColliderDraw = FALSE);
	virtual void RenderInst(const vector<_matrix*>& vecObjWorld);
	virtual void Release(void);

private:
	HRESULT Create_Buffer(const VTXTEX* pVB, const _uint& uiVtxCnt, const _uint* pIB, const _uint& uiIdxCnt
		, const _vec3& vMin, const _vec3& vMax, _tchar* pTexName);

private:
	virtual HRESULT Set_BoundingBox(void);
};

#endif // StaticMesh_h__