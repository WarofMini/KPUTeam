#ifndef Player_h__
#define Player_h__

#include "DynamicObject.h"

class CEquipment;
class CPlayer
	: public CDynamicObject
{
protected:
	explicit CPlayer(ID3D11DeviceContext* pContext);
	virtual ~CPlayer(void);

public:
	virtual INT Update(const FLOAT& fTimeDelta);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(ID3D11Device* pGraphicDev);
	virtual void Update_Equipment(const FLOAT& fTimeDelta);
protected:
	UINT			m_uiNavIdx;

	XMFLOAT4X4		m_matEquipBone[2];
	CEquipment*		m_pEquipment[2];
	FLOAT			m_fTrailTime;
	BOOL			m_bJump;
	FLOAT			m_fJumpTime;
	//Temp
	BOOL			m_UseNavMesh;

};

#endif // Player_h__
