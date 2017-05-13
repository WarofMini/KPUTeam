#ifndef SphereMesh_h__
#define SphereMesh_h__

#include "GameObject.h"

class CTransform;

class CSphereMesh
	: public CGameObject
{
private:
	explicit CSphereMesh(ID3D11DeviceContext* pContext, _float fRadius, XMFLOAT3* pPos);
	explicit CSphereMesh(ID3D11DeviceContext* pContext, _float fRadius);
	virtual ~CSphereMesh(void);

public:
	static CSphereMesh* Create(ID3D11DeviceContext* pContext, _float fRadius, XMFLOAT3* pPos);

	static CSphereMesh* Create(ID3D11DeviceContext* pContext, _float fRadius);

public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);


private:
	virtual HRESULT Ready_Component(void);

private:
	_uint			m_uiObjNum;
	_float			m_fRadius;
	CTransform*		m_pTransform;
	XMFLOAT3*		m_vPos;
	XMFLOAT4X4*		m_matWorld;
public:
	void			SetObjNum(_uint uNum);
	void			SetmatWorld(XMFLOAT4X4* matWorld);
	void			SetPosition(XMFLOAT3 vPos);

};

#endif // CSphereMesh
