#ifndef Station_h__
#define Station_h__

#include "GameObject.h"

class CCloth;
class CPlayer;
class CCircle;

class CStation
	: public CGameObject
{
private:
	explicit CStation(ID3D11DeviceContext* pContext);
	virtual ~CStation(void);

public:
	static CStation* Create(ID3D11DeviceContext* pContext, BYTE byStationID);

public:
	enum eFlagState { FLAG_EMPTY, FLAG_TEAM1, FLAG_TEAM2, FLAG_END };

public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

private:
	virtual HRESULT Ready_Component(void);

private:
	BYTE			m_byStationID;
	bool			m_bPlayerInStation;

	CCloth*			m_pFlag;
	PxRigidStatic*	m_pPxActor;
	CPlayer*		m_pPlayer;
	_float			m_fFlagDist;
	eFlagState		m_eFlagState;
	CCircle*		m_pCircleGage;
	CCircle*		m_pCircle;
	_float			m_fCircleRadius;
	_float			m_fCircleGageRadius;
	_float			m_fGage;
	BYTE			m_byATeamCnt;
	BYTE			m_byBTeamCnt;
	_float			m_fClientTime;

public:
	void			BuildObject(PxPhysics* pPxPhysics, PxScene* pPxScene, PxMaterial *pPxMaterial, XMFLOAT3 vScale, PxCooking* pCooking, const char* name);
	void			SetPosition(XMFLOAT3 vPosition);
	void			SetRotate(XMFLOAT3 vRot);
	PxRigidStatic*  GetPxActor(void);
	void			SetFlag(CCloth* pFlag) { m_pFlag = pFlag; };

	void			CollisionObject(const _float& fTimeDelta);
	_float			Length(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2);
	void			SetFlagState(eFlagState eState) { m_eFlagState = eState; }
	eFlagState		GetFlagState(void) { return m_eFlagState; }

	void			SetCircleGage(CCircle* pGage);
	void			SetCircle(CCircle* pCircle);

	void			SerSetStation(BYTE flagState, BYTE ATeamCnt, BYTE BTeamCnt, float fTime);
};

#endif // Station_h__
