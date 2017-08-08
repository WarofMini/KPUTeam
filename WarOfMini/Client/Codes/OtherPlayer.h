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
	enum STATE_SOLDIER { SOLDIER_IDLE, SOLDIER_MOVE, SOLDIER_LYING, SOLDIER_ROLL, SOLDIER_JUMP, SOLDIER_DEAD, SOLDIER_END };

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

private:
	HRESULT		Prepare_StateMachine(void);
	void		Operate_StateMAchine(const FLOAT& fTimeDelta);
	void		KeyCheck(void);
	void		KeyState(const FLOAT& fTimeDelta);
	void		Soldier_Move(const FLOAT& fTimeDelta);
	void		Soldier_Iron_Move(const FLOAT& fTimeDelta);
	void		Soldier_Fire(const FLOAT& fTimeDelta);

public:
	int		GetID(void) { return m_iID; }
	void	SetPlayerData(XMFLOAT3 vPos, float fAngle);
	void	PlayAnimation(DWORD dwAniIdx, bool bImmediate = true);
	DWORD*		Get_State(void) { return &m_dwState; }
	bool	IsSoldier(void) { return m_bIsSoldier; }
	void	SoldierChange(void);
	MOVE_DIR*	GetMoveDir(void);
	DWORD*		Get_AniIdx(void) { return &m_dwAniIdx; }
	_bool	Check_AnimationFrame(void);
	_bool		IsOnGround(void);
	void		Soldier_Iron_AddVelocity(float fFallVel);
	FLOAT		Get_Time(void) { return m_fTimeDelta; }
	void		Set_KeyState(_short sBitKeyState, float fAngle, short sHP);
	_bool*		Get_KeyState(void) { return m_bKey; }

	void	PhysXUpdate(const FLOAT& fTimeDelta);
	void	UpdateBoost(const FLOAT& fTimeDelta); //변신을 하기 위한 부스트 게이지

	void			SetHP(void) { --m_iHP; }

private:
	int				m_iID;

	XMFLOAT3		m_vLook;
	XMFLOAT4X4		m_matEquipBone[2];
	CGun*			m_pEquipment[2];
	_int			m_iEquipBone;
	FLOAT			m_fTimeDelta;
	string			m_strName;

	//Player Animation
	DWORD			m_dwState;
	DWORD			m_dwAniIdx;
	CStateMachine*	m_pComStateMachine;
	_bool			m_bKey[KEY_END];
	_short			m_sBitKey;
	MOVE_DIR		m_eMoveDir;
	XMFLOAT3		m_vMoveDir;

	//Player Mesh
	_bool			m_bIsSoldier;
	CAnimationInfo* m_pAnimInfo_Normal;
	CAnimationInfo* m_pAnimInfo_Iron;
	MATNODE*		m_pMatBoneNode_Normal;
	MATNODE*		m_pMatBoneNode_Iron;
	_uint			m_uiObjNum_Normal;
	_uint			m_uiObjNum_Iron;

	
	_short			m_iHP;
	FLOAT			m_fRollSpeed;
	XMFLOAT3		m_fRollDir;
	FLOAT			m_fRateOfFire;
	_bool			m_bFire;
	_bool			m_bAbleReload;
	FLOAT			m_fFallvelocity;
	FLOAT			m_fFallAcceleration;

	_float			m_fBoost;    //부스트 게이지
	_float			m_fMaxBoost; //초기 부스트 게이지
	_bool			m_bBoostDelay;
	_float			m_fDelayCount;
	_uint			m_iGunFlashTexture;

//Physx SDK Member Variables =========================
private:
	PxController*		m_pPxCharacterController;

public:
	void	BuildObject(PxPhysics* pPxPhysics, PxScene* pPxScene, PxMaterial *pPxMaterial, PxControllerManager *pPxControllerManager);

};

#endif //