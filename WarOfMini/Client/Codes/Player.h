#ifndef Player_h__
#define Player_h__

#include "DynamicObject.h"

class CEquipment;
class CStateMachine;
class CInput;
class CGun;

class CPlayer
	: public CDynamicObject,  public PxUserControllerHitReport, public PxControllerBehaviorCallback
{
protected:
	explicit CPlayer(ID3D11DeviceContext* pContext);
	virtual ~CPlayer(void);

public:
	enum STATE_SOLDIER { SOLDIER_IDLE, SOLDIER_MOVE, SOLDIER_LYING, SOLDIER_ROLL, SOLDIER_JUMP, SOLDIER_DEAD, SOLDIER_END };

public:
	static CPlayer* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);
	static CPlayer* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, XMFLOAT3 vPos);

public:
	virtual HRESULT	Initialize(ID3D11Device* pGraphicDev);
	virtual INT Update(const FLOAT& fTimeDelta);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(ID3D11Device* pGraphicDev);
	virtual void Update_Equipment(const FLOAT& fTimeDelta);

private:
	HRESULT		Prepare_StateMachine(void);
	void		Operate_StateMAchine(const FLOAT& fTimeDelta);
	void		KeyCheck(void);
	void		KeyState(const FLOAT& fTimeDelta);
	void		Soldier_Move(const FLOAT& fTimeDelta);
	void		Soldier_Iron_Move(const FLOAT& fTimeDelta);
	void		Soldier_Fire(const FLOAT& fTimeDelta);

public:
	CInput*		GetInput(void) { return m_pInput; }
	void		PlayAnimation(DWORD dwAniIdx, bool bImmediate = true);
	DWORD*		Get_State(void) { return &m_dwState; }
	DWORD*		Get_AniIdx(void) { return &m_dwAniIdx; }
	_bool		Check_AnimationFrame(void);
	_bool		IsOnGround(void);
	MOVE_DIR*	GetMoveDir(void);
	void		SoldierChange(void);
	_bool		IsSoldier(void) { return m_bIsSoldier; }
	void		Soldier_Iron_AddVelocity(float fFallVel);
	void		Set_Fire(_bool bFire) { m_bFire = bFire; }
	_bool		IsAbleReload(void) { return m_bAbleReload; }
	void		Reload(void);
	FLOAT		Get_Time(void) { return m_fTimeDelta; }
	void		Set_KeyState(_short sBitKeyState, float fAngle, short sHP);
	_bool*		Get_KeyState(void) { return m_bKey; }
public:
	void		UpdateDir(void);

	//현재 카메라 상태가 Dynamic일 경우  Player Update 실행X
	_bool		DynamicCameraCheck(void); 
	XMFLOAT3	m_vtestpos; //레이캐스팅 테스트를 위한 변수
	//server
	void		SendPacketAlways(void);

private:
	CInput*			m_pInput;
	XMFLOAT3		m_vLook;
	XMFLOAT4X4		m_matEquipBone[2];
	CGun*			m_pEquipment[2];
	_int			m_iEquipBone;
	FLOAT			m_fTimeDelta;
	FLOAT			m_fMoveAngleY;

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

	FLOAT			m_fRollSpeed;
	XMFLOAT3		m_fRollDir;
	FLOAT			m_fRateOfFire;
	_bool			m_bFire;
	_bool			m_bAbleReload;
	FLOAT			m_fFallvelocity;
	FLOAT			m_fFallAcceleration;

	PxScene*		m_pScene;

	//test
	_short			m_iHP;
	_int			m_iOriginHP; //초기 HP값
	_float			m_fBoost;    //부스트 게이지
	_float			m_fMaxBoost; //초기 부스트 게이지
	_bool			m_bBoostDelay;
	_float			m_fDelayCount;

	CGameObject*	m_pTank;

	Ser_COLLLAY_DATA m_ColllayData;


	_uint			m_iGunFlashTexture;


	//Physx SDK Member Variables =========================
private:
	PxController*		m_pPxCharacterController;

	//객체에 충돌박스 입히기===============================
	CGameObject*		pSphereMesh;
private:
	XMFLOAT4X4			m_matBone;
	_int				m_iBoneNum;

public:
	virtual void		Render(void);


public:
	void	BuildObject(PxPhysics* pPxPhysics, PxScene* pPxScene, PxMaterial *pPxMaterial, PxControllerManager *pPxControllerManager);
	void	SetPosition(XMFLOAT3 vPosition);
	void	SetRotate(XMFLOAT3 vRot);
	void	PhysXUpdate(const FLOAT& fTimeDelta);
	
	CGun*	GetEquipment(void) { return m_pEquipment[0]; }

	void	UpdateBoost(const FLOAT& fTimeDelta); //변신을 하기 위한 부스트 게이지

	//test
	void			SetHP(void) { --m_iHP; }
	void			SetHP(_short sHp) { m_iHP = sHp; }
	bool			UseTank(void);
	_int			GetHP(void) { return m_iHP; }
	_int			GetOriginHP(void) { return m_iOriginHP;  }
	_float			GetBoost(void) { return m_fBoost; }
	_float			GetMaxBoost(void) { return m_fMaxBoost; }

	_float			GunReactionRand(_float a, _float b);

public:
	virtual void							onShapeHit(const PxControllerShapeHit& hit);
	virtual void							onControllerHit(const PxControllersHit& hit) {}
	virtual void							onObstacleHit(const PxControllerObstacleHit& hit) {}

	void AddForceAtLocalPos(PxRigidBody& body, const PxVec3& force, const PxVec3& pos, PxForceMode::Enum mode, bool wakeup = true);
	void AddForceAtPosInternal(PxRigidBody& body, const PxVec3& force, const PxVec3& pos, PxForceMode::Enum mode, bool wakeup);

	// Implements PxControllerBehaviorCallback
	virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxShape& shape, const PxActor& actor);
	virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxController& controller);
	virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxObstacle& obstacle);

};

#endif //