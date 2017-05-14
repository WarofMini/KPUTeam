#ifndef OtherPlayer_h__
#define OtherPlayer_h__

#include "DynamicObject.h"

class CEquipment;

class COtherPlayer
	: public CDynamicObject
{
protected:
	explicit COtherPlayer(ID3D11DeviceContext* pContext);
	virtual ~COtherPlayer(void);

public:
	static COtherPlayer* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);
	static COtherPlayer* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, XMFLOAT3 vPos, int iID);


public:
	virtual HRESULT	Initialize(ID3D11Device* pGraphicDev);
	virtual INT     Update(const FLOAT& fTimeDelta);
	virtual void    Release(void);
	void		    UpdateDir(void);
protected:
	virtual HRESULT Ready_Component(ID3D11Device* pGraphicDev);
	virtual void    Update_Equipment(const FLOAT& fTimeDelta);

public:
	int		GetID(void) { return m_iID; }
	void	SetPlayerData(XMFLOAT3 vPos, XMFLOAT3 vDir);
	void	PlayAnimation(DWORD dwAniIdx, bool bImmediate);
	bool	IsSoldier(void) { return m_bIsSoldier; }
	void	SoldierChange(void);

private:
	int				m_iID;

	XMFLOAT3		m_vLook;

	XMFLOAT4X4		m_matEquipBone[2];
	CGun*			m_pEquipment[2];
	_int			m_iEquipBone;
	DWORD			m_dwAniIdx;
	bool			m_bIsSoldier;
	CAnimationInfo* m_pAnimInfo_Normal;
	CAnimationInfo* m_pAnimInfo_Iron;
	MATNODE*		m_pMatBoneNode_Normal;
	MATNODE*		m_pMatBoneNode_Iron;
	_uint			m_uiObjNum_Normal;
	_uint			m_uiObjNum_Iron;

	string			m_strName;

//Physx SDK Member Variables =========================
private:
	PxRigidDynamic*		m_pPxActor;
	PxController*		m_pPxCharacterController;

public:
	void	BuildObject(PxPhysics* pPxPhysics, PxScene* pPxScene, PxMaterial *pPxMaterial, PxControllerManager *pPxControllerManager);

};

#endif //