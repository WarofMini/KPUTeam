#ifndef Mesh_h__
#define Mesh_h__

#include "Resources.h"

class CTextures;

class CMesh
	: public CResource
{
protected:
	explicit CMesh(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);
	virtual ~CMesh(void);

public:
	void	Set_RootMinMax(void);
	const XMFLOAT3* Get_Min(void);
	const XMFLOAT3* Get_Max(void);

public:
	void Reserve_ChildSize(const _ushort& wSize);
	void Add_Child(CMesh* pChild);
	void Clear_NullChild(void);
	_bool Check_Remove(void);

public:
	virtual CResource* Clone_Resource(void) PURE;
	virtual void Render(_bool bColliderDraw = FALSE) PURE;
	virtual void RenderInst(const vector<XMFLOAT4X4*>& vecObjWorld) PURE;
	virtual void Release(void);

protected:
	virtual HRESULT Set_BoundingBox(void)	PURE;

protected:
	vector<CMesh*> m_vecChild;


protected:
	CTextures*		m_pTexture;
	ID3D11Buffer*	m_pVB;
	ID3D11Buffer*	m_pIB;
	_uint			m_uiVtxCnt;
	_uint			m_uiIdxCnt;
	VTXTEX*			m_pVtxTex;

protected:
	XMFLOAT3		m_vMin;
	XMFLOAT3		m_vMax;
	ID3D11Buffer*	m_pBBoxVB;
	ID3D11Buffer*	m_pBBoxIB;

public:
	VTXTEX*			GetVtxTex(void);
	_uint			GetVtxCnt(void);
};

#endif // Mesh_h__