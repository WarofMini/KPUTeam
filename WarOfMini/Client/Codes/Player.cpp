#include "stdafx.h"
#include "Include.h"
#include "Player.h"
#include "AnimationInfo.h"
#include "Transform.h"
#include "MeshMgr.h"
#include "Gun.h"
#include "Management.h"
#include "Input.h"
#include "StateMachine.h"
#include "SoldierDefine.h"
#include "CameraMgr.h"
#include "Layer.h"
#include "DefaultObject.h"
#include "Management.h"
#include "Scene.h"
#include "SphereMesh.h"
#include "Tank.h"
#include "Bomb.h"
#include "GunFlash.h"
#include "HitScreen.h"

XMFLOAT3		g_vPlayerPos;

CPlayer::CPlayer(ID3D11DeviceContext* pContext)
: CDynamicObject(pContext)
, m_dwState(SOLDIER_IDLE)
, m_dwAniIdx(PLAYER_idle)
, m_pComStateMachine(NULL)
, m_eMoveDir(DIR_END)
, m_vMoveDir(0.f, 0.f, 0.f)
, m_bIsSoldier(true)
, m_fTimeDelta(0.f)
, m_iEquipBone(0)
, m_fRollSpeed(0.f)
, m_fRateOfFire(0.f)
, m_bFire(false)
, m_bAbleReload(false)
, m_pPxCharacterController(NULL)
, m_fFallAcceleration(9.8f)
, m_fFallvelocity(0.f)
, m_iHP(20)
, m_iOriginHP(m_iHP)
, m_pTank(NULL)
, m_fBoost(0.0f)
, m_fMaxBoost(100.0f)
, m_bBoostDelay(false)
, m_fDelayCount(0.0f)
, m_iGunFlashTexture(1)
, m_fMoveAngleY(0.f)
{
	m_pInput = CInput::GetInstance();
	m_vLook = XMFLOAT3(0.f, 1.f, 0.f);

	XMStoreFloat4x4(&m_matEquipBone[0], XMMatrixIdentity());
	XMStoreFloat4x4(&m_matEquipBone[1], XMMatrixIdentity());

	ZeroMemory(m_pEquipment, sizeof(CEquipment*) * 2);
	ZeroMemory(&m_bKey, sizeof(bool) * KEY_END);
	ZeroMemory(&m_ColllayData, sizeof(Ser_COLLLAY_DATA));
	m_ColllayData.iCollPlayerID = -1;

	m_pServer_PlayerData = new Ser_PLAYER_DATA;

	m_fSpeed = 40.f;


	XMStoreFloat4x4(&m_matBone, XMMatrixIdentity());
	m_iBoneNum = 0;
	//pSphereMesh = CSphereMesh::Create(m_pContext, 1.f);
}

CPlayer::~CPlayer(void)
{
}

CPlayer* CPlayer::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
{
	CPlayer* pObject = new CPlayer(pContext);

	if (FAILED(pObject->Initialize(pGraphicDev)))
		Safe_Release(pObject);

	return pObject;
}

CPlayer* CPlayer::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, XMFLOAT3 vPos)
{
	CPlayer* pObject = new CPlayer(pContext);
	if (FAILED(pObject->Initialize(pGraphicDev)))
		Safe_Release(pObject);

	pObject->m_pTransform->m_vPos = vPos;

	return pObject;
}

HRESULT CPlayer::Initialize(ID3D11Device* pGraphicDev)
{
	m_uiObjNum		= MESHNUM_PLAYER;
	m_uiObjNum_Iron = MESHNUM_PLAYER2;

	if (FAILED(Ready_Component(pGraphicDev)))
		return E_FAIL;

	FAILED_CHECK(Prepare_StateMachine());

	m_pTransform->m_vScale = XMFLOAT3(1.f, 1.f, 1.f);
	m_pTransform->m_vAngle.x = 90.f;

	//m_pTransform->m_vPos = g_vPos;

	m_pTransform->m_vPos = XMFLOAT3(g_vPos);

	m_pTransform->m_vDir = XMFLOAT3(0.f, 0.f, -1.f);
	m_pAnimInfo->Set_Key((_ushort)m_dwAniIdx);

	m_pComStateMachine->Enter_State(SOLDIER_IDLE);

	// Equipment
	m_pEquipment[0] = CGun::Create(pGraphicDev, m_pContext);

	if(g_myid % 2 == 0)
		m_iTextureNumber = 0;
	else 
		m_iTextureNumber = 1;

	return S_OK;
}

INT CPlayer::Update(const FLOAT& fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;

	if (g_GameState == 1)
		UpdateBoost(fTimeDelta);
	else
		UpdateBoost(0);

	CDynamicObject::Update(fTimeDelta);
	
	//PhysX 함수
	PhysXUpdate(fTimeDelta);

	//==========================

	//Dynamic카메라 체크 함수(Dynamic 카메라일시 Update 안돌린다.
	if (!DynamicCameraCheck())
	{
		Operate_StateMAchine(fTimeDelta);
		KeyState(fTimeDelta);

		SendPacketAlways();
	}	

	// 본행렬 찾기 위한 디버그
	/*
	if (GetAsyncKeyState('G') & 1)
	{
		m_iBoneNum += 1;
		cout << "BoneNum" << m_iBoneNum << endl;
	}
	if (GetAsyncKeyState('H') & 1)
	{
		m_iBoneNum -= 1;
		cout << "BoneNum" << m_iBoneNum << endl;
	}
	*/
	

	// Update
	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);

	UpdateDir();

	Update_Equipment(fTimeDelta);


	//pSphereMesh->Update(fTimeDelta);

	return 0;
}

void CPlayer::Release(void)
{

	if (m_pPxCharacterController)
		m_pPxCharacterController->release();


	CDynamicObject::Release();
	Safe_Release(m_pEquipment[0]);
}

HRESULT CPlayer::Ready_Component(ID3D11Device* pGraphicDev)
{
	if (FAILED(CDynamicObject::Ready_Component()))
		return E_FAIL;

	CComponent* pComponent = NULL;

	m_pAnimInfo_Normal = m_pAnimInfo;
	m_pMatBoneNode_Normal = m_pMatBoneNode;
	m_uiObjNum_Normal = m_uiObjNum;

	// AnimationInfo	Iron
	pComponent = CAnimationInfo::Create(MESHNUM(m_uiObjNum_Iron));
	m_pAnimInfo_Iron = dynamic_cast<CAnimationInfo*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_AnimInfo_Iron", pComponent));
	// Matrix Bone		Iron
	m_pMatBoneNode_Iron = new MATNODE;
	CMeshMgr::GetInstance()->CreateBoneNode(m_uiObjNum_Iron, m_pMatBoneNode_Iron);

	//StateMachine
	pComponent = m_pComStateMachine = CStateMachine::Create(SOLDIER_END);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"StateMachine", pComponent));

	return S_OK;
}

void CPlayer::Update_Equipment(const FLOAT& fTimeDelta)
{
	m_matEquipBone[0] = CMeshMgr::GetInstance()->Get_TransMeshBone(m_uiObjNum, 0, m_iEquipBone, m_pMatBoneNode);

	if (XMMatrixIsIdentity(XMLoadFloat4x4(&m_matEquipBone[0])))
		return;

	m_matBone = CMeshMgr::GetInstance()->Get_TransMeshBone(m_uiObjNum, 0, m_iBoneNum, m_pMatBoneNode);

	XMMATRIX matWorld = XMLoadFloat4x4(&m_pTransform->m_matWorld);
	XMStoreFloat4x4(&m_matEquipBone[0], XMLoadFloat4x4(&m_matEquipBone[0]) * matWorld);

	//XMStoreFloat4x4(&m_matBone, XMLoadFloat4x4(&m_matBone) * matWorld);
	//((CSphereMesh*)pSphereMesh)->SetmatWorld(&m_matBone);

	m_pEquipment[0]->SetParent(m_matEquipBone[0]);
	m_pEquipment[0]->Update(fTimeDelta);
}

HRESULT CPlayer::Prepare_StateMachine(void)
{
	NULL_CHECK_RETURN(m_pComStateMachine, E_FAIL);
	CState*	pState = NULL;

	pState = CSoldierIdle::Create(this);
	FAILED_CHECK(m_pComStateMachine->Add_State(pState));

	pState = CSoldierMove::Create(this);
	FAILED_CHECK(m_pComStateMachine->Add_State(pState));

	pState = CSoldierLying::Create(this);
	FAILED_CHECK(m_pComStateMachine->Add_State(pState));

	pState = CSoldierRoll::Create(this);
	FAILED_CHECK(m_pComStateMachine->Add_State(pState));

	pState = CSoldierJump::Create(this);
	FAILED_CHECK(m_pComStateMachine->Add_State(pState));

	return S_OK;
}

void CPlayer::Operate_StateMAchine(const FLOAT& fTimeDelta)
{
	if (m_iHP <= 0)
	{
		if (m_dwState != SOLDIER_DEAD)
		{
			m_dwState = SOLDIER_DEAD;

			if (m_bIsSoldier)
				PlayAnimation(PLAYER_Death2);
			else
				PlayAnimation(PLAYER_Iron_Death2);
		}
		else
		{
			if (m_pAnimInfo->Get_PlayAni())
			{
				if (Check_AnimationFrame())
					m_pAnimInfo->Set_PlayAni(false);
			}
		}
	}
	else
	{
		if (m_dwState == SOLDIER_DEAD && Check_AnimationFrame())
		{
			m_pAnimInfo->Set_PlayAni(true);
			m_dwState = SOLDIER_IDLE;
			if (m_bIsSoldier)
				PlayAnimation(PLAYER_idle);
			else
				PlayAnimation(PLAYER_Iron_Idle);
			m_pComStateMachine->Enter_State(SOLDIER_IDLE);
		}

		KeyCheck();

		DWORD dwState = m_pComStateMachine->Get_State();

		switch (dwState)
		{
		case SOLDIER_IDLE:
			if (m_dwState == SOLDIER_MOVE)
				m_pComStateMachine->Enter_State(SOLDIER_MOVE);
			if (m_dwState == SOLDIER_LYING)
				m_pComStateMachine->Enter_State(SOLDIER_LYING);
			if (m_dwState == SOLDIER_JUMP)
			{
				m_pComStateMachine->Enter_State(SOLDIER_JUMP);
				m_fFallvelocity = 120.f;
			}
			break;
		case SOLDIER_MOVE:
			if (m_dwState == SOLDIER_IDLE)
				m_pComStateMachine->Enter_State(SOLDIER_IDLE);
			if (m_dwState == SOLDIER_ROLL)
			{
				m_pComStateMachine->Enter_State(SOLDIER_ROLL);
				m_fFallvelocity = 60.f;
				m_fRollSpeed = m_fSpeed * 1.7f;
				m_fRollDir = m_vMoveDir;
			}

			if (m_dwState == SOLDIER_JUMP)
			{
				m_pComStateMachine->Enter_State(SOLDIER_JUMP);
				m_fFallvelocity = 120.f;
			}

			break;

		case SOLDIER_LYING:
			if (m_dwState == SOLDIER_IDLE)
				m_pComStateMachine->Enter_State(SOLDIER_IDLE);
			break;
		case SOLDIER_ROLL:
			if (m_dwState == SOLDIER_IDLE)
				m_pComStateMachine->Enter_State(SOLDIER_IDLE);
			if (m_dwState == SOLDIER_MOVE)
				m_pComStateMachine->Enter_State(SOLDIER_MOVE);
			break;
		case SOLDIER_JUMP:
			if (m_dwState == SOLDIER_IDLE)
				m_pComStateMachine->Enter_State(SOLDIER_IDLE);
			break;
		default:
			break;
		}
		m_pComStateMachine->Update_State(dwState);
	}
}

void CPlayer::PlayAnimation(DWORD dwAniIdx, bool bImmediate)
{
	if (bImmediate)
		m_pAnimInfo->Set_KeyImm((_ushort)dwAniIdx);
	m_pAnimInfo->Set_Key((_ushort)dwAniIdx);
	m_dwAniIdx = dwAniIdx;

	/*Ser_ANIMATION_DATA m_AnimationData;
	m_AnimationData.size = sizeof(Ser_ANIMATION_DATA);
	m_AnimationData.type = CLIENT_ANIMATION;
	m_AnimationData.ID = g_myid;
	m_AnimationData.bImmediate = bImmediate;
	m_AnimationData.dwAniIdx = dwAniIdx;
	m_AnimationData.bIsSoldier = m_bIsSoldier;
	g_Client->sendPacket(sizeof(Ser_ANIMATION_DATA), CLIENT_ANIMATION, reinterpret_cast<BYTE*>(&m_AnimationData));*/
}

bool CPlayer::Check_AnimationFrame(void)
{
	if (m_pAnimInfo->Get_CurFrame() >= m_pAnimInfo->Get_LastFrame())
		return true;
	return false;
}

bool CPlayer::IsOnGround(void)
{
	PxControllerState   m_pPxState;

	//피직스 객체의 상태값을 m_pPxState에 넣어준다.
	m_pPxCharacterController->getState(m_pPxState);

	/*return m_pComGravity->Get_OnGround();*/
	if (m_pPxState.collisionFlags == PxControllerCollisionFlag::eCOLLISION_DOWN)
		return true;
	return false;
}

MOVE_DIR* CPlayer::GetMoveDir(void)
{
	return &m_eMoveDir;
}

void CPlayer::SoldierChange(void) //솔져 변신
{
	if (m_fBoost < m_fMaxBoost)
		return;

	if (m_bIsSoldier) //일반상태 -> 변신으로 변환
	{
		m_bIsSoldier = false;
		m_uiObjNum = m_uiObjNum_Iron;
		m_pAnimInfo = m_pAnimInfo_Iron;
		m_pMatBoneNode = m_pMatBoneNode_Iron;
		PlayAnimation((_ushort)PLAYER_Iron_Idle);
		((CGun*)m_pEquipment[0])->ChangeWeapon(MESHNUM_SPECIALGUN);
		m_iEquipBone = 2;
	}
	/*
	else //변환상태->일반상태로 변환
	{
		m_bIsSoldier = true;
		m_uiObjNum = m_uiObjNum_Normal;
		m_pAnimInfo = m_pAnimInfo_Normal;
		m_pMatBoneNode = m_pMatBoneNode_Normal;
		PlayAnimation((_ushort)PLAYER_idle);
		((CGun*)m_pEquipment[0])->ChangeWeapon(MESHNUM_GUN);
		m_iEquipBone = 0;
	}
	*/
	m_pAnimInfo->Update(1.f);
}

void CPlayer::KeyCheck(void)
{
	m_eMoveDir = DIR_END;
	m_vMoveDir = XMFLOAT3(0.f, 0.f, 0.f);
	//ZeroMemory(&m_bKey, sizeof(bool) * KEY_END);
	m_sBitKey = 0;

	if (m_pInput->Get_DIKeyState(DIK_W))
		m_sBitKey += 1;//m_bKey[KEY_UP] = true;
	if (m_pInput->Get_DIKeyState(DIK_S))
		m_sBitKey += 2;//m_bKey[KEY_DOWN] = true;
	if (m_pInput->Get_DIKeyState(DIK_A))
		m_sBitKey += 4;//m_bKey[KEY_LEFT] = true;
	if (m_pInput->Get_DIKeyState(DIK_D))
		m_sBitKey += 8;//m_bKey[KEY_RIGHT] = true;
	if (m_pInput->Get_DIKeyState(DIK_Q))
		m_sBitKey += 16;//m_bKey[KEY_Q] = true;
	if (m_pInput->Get_DIKeyState(DIK_R))
		m_sBitKey += 32;//m_bKey[KEY_R] = true;

	if (m_pInput->Get_DIKeyState(DIK_LSHIFT))
		m_sBitKey += 64;//m_bKey[KEY_SHIFT] = true;
	if (m_pInput->Get_DIKeyState(DIK_LCONTROL))
		m_sBitKey += 128;//m_bKey[KEY_CTRL] = true;
	if (m_pInput->Get_DIKeyState(DIK_SPACE))
		m_sBitKey += 256;//m_bKey[KEY_SPACE] = true;
	if (m_pInput->Get_DIMouseState(CInput::DIM_LB))
		m_sBitKey += 512;//m_bKey[KEY_LCLICK] = true;
	if (m_pInput->Get_DIMouseState(CInput::DIM_RB))
		m_sBitKey += 1024;//m_bKey[KEY_RCLICK] = true;

	if (m_bKey[KEY_UP] == true && m_bKey[KEY_DOWN] == true)
	{
		m_bKey[KEY_UP] = false;
		m_bKey[KEY_DOWN] = false;
	}
	if (m_bKey[KEY_LEFT] == true && m_bKey[KEY_RIGHT] == true)
	{
		m_bKey[KEY_LEFT] = false;
		m_bKey[KEY_RIGHT] = false;
	}

	if (!m_bKey[KEY_UP] && !m_bKey[KEY_DOWN] && !m_bKey[KEY_LEFT] && !m_bKey[KEY_RIGHT])
		return;

	XMVECTOR vDir = XMVectorSet(m_pTransform->m_vDir.x, 0.f, m_pTransform->m_vDir.z, 0.0f);
	XMVECTOR vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR vRight = XMVector3Cross(vUp, vDir);
	XMVECTOR vMoveDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	vDir = XMVector3Normalize(vDir);
	vRight = XMVector3Normalize(vRight);

	if (m_bKey[KEY_UP])
	{
		vMoveDir += vDir;

		if (m_bKey[KEY_LEFT])
		{
			vMoveDir -= vRight;
			m_eMoveDir = DIR_UL;
		}
		else if (m_bKey[KEY_RIGHT])
		{
			vMoveDir += vRight;
			m_eMoveDir = DIR_UR;
		}
		else
			m_eMoveDir = DIR_U;
	}
	else if (m_bKey[KEY_DOWN])
	{
		vMoveDir -= vDir;
		if (m_bKey[KEY_LEFT])
		{
			vMoveDir -= vRight;
			m_eMoveDir = DIR_DL;
		}
		else if (m_bKey[KEY_RIGHT])
		{
			vMoveDir += vRight;
			m_eMoveDir = DIR_DR;
		}
		else
			m_eMoveDir = DIR_D;
	}
	else if (m_bKey[KEY_LEFT])
	{
		vMoveDir -= vRight;
		m_eMoveDir = DIR_L;
	}
	else if (m_bKey[KEY_RIGHT])
	{
		vMoveDir += vRight;
		m_eMoveDir = DIR_R;
	}
	vMoveDir = XMVector3Normalize(vMoveDir);
	XMStoreFloat3(&m_vMoveDir, vMoveDir);
}

void CPlayer::KeyState(const FLOAT& fTimeDelta)
{
	if (m_dwState == SOLDIER_DEAD)
		return;
	_long lMouseMove = 0;

	if (lMouseMove = m_pInput->Get_DIMouseMove(CInput::DIMS_X))
	{
		if (m_fMoveAngleY >= 360.f)
			m_fMoveAngleY = 0.f;
		if (m_fMoveAngleY < 0.f)
			m_fMoveAngleY = 360.f;
		m_fMoveAngleY += lMouseMove * fTimeDelta * 5.f;
		/*if (m_pTransform->m_vAngle.y >= 360.f)
			m_pTransform->m_vAngle.y = 0.f;
		if (m_pTransform->m_vAngle.y < 0.f)
			m_pTransform->m_vAngle.y = 360.f;
		m_pTransform->m_vAngle.y += lMouseMove * fTimeDelta * 5.f;*/
	}

	if(m_bIsSoldier)
		Soldier_Move(fTimeDelta);
	else
		Soldier_Iron_Move(fTimeDelta);

	Soldier_Fire(fTimeDelta);

	if (m_bAbleReload)
	{
		m_sBitKey &= 0b10111111111;
	}

	/*if (m_pInput->GetDIKeyStateOnce(DIK_RETURN))
	{
		if (m_pTank)
		{ 
			((CTank*)m_pTank)->SetUse(false, g_myid);
			m_pTank = NULL;

		}
		else
		{
			CScene* pScene = NULL;
			CLayer* pLayer = NULL;

			pScene = CManagement::GetInstance()->GetScene();

			pLayer = pScene->FindLayer(L"Layer_GameLogic");
			list<CGameObject*>* pObjList = pLayer->Find_ObjectList(L"NPC");
			m_pTank = *pObjList->begin();
			if (m_pTank != NULL)
			{
				((CTank*)m_pTank)->SetUse(true, g_myid);
			}
		}
	}*/

	/*if (m_pTank)
		SetPosition(((CTank*)m_pTank)->GetPos());*/
}

void CPlayer::Soldier_Move(const FLOAT& fTimeDelta)
{
	//XMVECTOR vDir;// = XMLoadFloat3(&m_vMoveDir);
	//XMVECTOR vPos = XMLoadFloat3(&m_pTransform->m_vPos);
	switch (m_dwState)
	{
	case SOLDIER_MOVE:
		//vDir = XMLoadFloat3(&m_vMoveDir);
		if (m_dwAniIdx == PLAYER_sprint)
		m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta * 1.4f, 0, fTimeDelta, PxControllerFilters());
		else
		{
			if(m_eMoveDir == DIR_D || m_eMoveDir == DIR_DL || m_eMoveDir == DIR_DR)
				m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta * 0.7f, 0, fTimeDelta, PxControllerFilters());
			else
				m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta, 0, fTimeDelta, PxControllerFilters());
		}
		break;
	case SOLDIER_LYING:
		//vDir = XMLoadFloat3(&m_vMoveDir);
		if (m_dwAniIdx != PLAYER_Lying && m_dwAniIdx != PLAYER_LyingShoot)
		{
			m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta * 0.25f, 0, fTimeDelta, PxControllerFilters());
		}
		break;
	case SOLDIER_JUMP:
		//vDir = XMLoadFloat3(&m_vMoveDir);
		m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta, 0, fTimeDelta, PxControllerFilters());
		break;
	case SOLDIER_ROLL:
		//vDir = XMLoadFloat3(&m_fRollDir);
		m_pPxCharacterController->move(PxVec3(m_fRollDir.x, m_fRollDir.y, m_fRollDir.z) * m_fRollSpeed * fTimeDelta, 0, fTimeDelta, PxControllerFilters());

		m_fRollSpeed -= 20.f * fTimeDelta;
		break;
	}
}

void CPlayer::Soldier_Iron_Move(const FLOAT& fTimeDelta)
{
	//XMVECTOR vDir;// = XMLoadFloat3(&m_vMoveDir);
	//XMVECTOR vPos = XMLoadFloat3(&m_pTransform->m_vPos);
	switch (m_dwState)
	{
	case SOLDIER_MOVE:
		//vDir = XMLoadFloat3(&m_vMoveDir);
		if (m_dwAniIdx == PLAYER_Iron_Sprint)
			m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta * 1.4f, 0, fTimeDelta, PxControllerFilters());
		else
			m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta, 0, fTimeDelta, PxControllerFilters());
		break; 
	case SOLDIER_LYING:
		//vDir = XMLoadFloat3(&m_vMoveDir);
		if (m_dwAniIdx != PLAYER_Iron_Lying && m_dwAniIdx != PLAYER_Iron_LyingShoot)
		{
			m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta * 0.25f, 0, fTimeDelta, PxControllerFilters());
		}
		break;
	case SOLDIER_JUMP:
		//vDir = XMLoadFloat3(&m_vMoveDir);
		m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta, 0, fTimeDelta, PxControllerFilters());
		break;
	case SOLDIER_ROLL:
		//vDir = XMLoadFloat3(&m_fRollDir);
		m_pPxCharacterController->move(PxVec3(m_fRollDir.x, m_fRollDir.y, m_fRollDir.z) * m_fRollSpeed * fTimeDelta, 0, fTimeDelta, PxControllerFilters());
		m_fRollSpeed -= 20.f * fTimeDelta;
		break;
	}
}

void CPlayer::Soldier_Fire(const FLOAT& fTimeDelta)
{
	ZeroMemory(&m_ColllayData, sizeof(Ser_COLLLAY_DATA));
	m_ColllayData.iCollPlayerID = -1;

	if (!m_bFire)
	{
		g_fGunReaction = max(g_fGunReaction - (fTimeDelta * 4.f), 0.0f);
		m_fRateOfFire = 0.f;
		return;
	}
	m_fRateOfFire += fTimeDelta;

	if (m_fRateOfFire >= 0.2f)
	{
		m_fRateOfFire = 0.1f;
		if (((CGun*)m_pEquipment[0])->Fire())
		{
			//총기반동
			g_fGunReaction = min(g_fGunReaction + (fTimeDelta * 2.f), 10.f);

			XMFLOAT3 m_vCameraEye = CCameraMgr::GetInstance()->Get_CurCameraEye();
			XMFLOAT3 m_vCameraLookAt = CCameraMgr::GetInstance()->Get_CurCameraLookAt();

			PxVec3 OriginPos = PxVec3(m_vCameraEye.x, m_vCameraEye.y, m_vCameraEye.z);
			PxVec3 Dir = PxVec3(m_vCameraLookAt.x, m_vCameraLookAt.y, m_vCameraLookAt.z);

			PxReal maxDistance = 1000.f;
			PxRaycastBuffer Onehit;
			PxQueryFilterData Onefd;
			Onefd.flags |= PxQueryFlag::ePOSTFILTER;

			bool m_bOneCheck = false;

			m_bOneCheck = m_pScene->raycast(OriginPos, Dir, maxDistance, Onehit, PxHitFlags(PxHitFlag::eDEFAULT), Onefd);

			//Player의 월드 총구 포지션 설정
			XMFLOAT4X4 vmatWorld = ((CTransform*)m_pEquipment[0]->Get_Component(L"Com_Transform"))->m_matWorld;


			XMFLOAT3 m_vWorldGunPos;

			if (m_bIsSoldier)
			{
				//x 가 z축의 역할을하네   z+가 x-
				m_vWorldGunPos = XMFLOAT3(50.f, -27.f, 40.f);
			}
			else
			{
				//x+가 z+축 z축 -x
				m_vWorldGunPos = XMFLOAT3(45.f, 5.f, 0.f);
			}

			XMStoreFloat3(&m_vWorldGunPos, XMVector3TransformCoord(XMLoadFloat3(&m_vWorldGunPos), XMLoadFloat4x4(&vmatWorld)));

			CLayer* pLayer = CManagement::GetInstance()->GetScene()->FindLayer(L"Layer_GameLogic");

			//GunFlash
			CGameObject* pGunFlash = CGunFlash::Create(m_pContext);

			pGunFlash->SetTransformPosition(XMFLOAT3(m_vWorldGunPos.x, m_vWorldGunPos.y, m_vWorldGunPos.z));

			if (m_iGunFlashTexture == 1)
			{
				m_iGunFlashTexture = 0;
				pGunFlash->SetTransformScale(XMFLOAT3(70.f, 50.f, 50.f));
				((CGunFlash*)pGunFlash)->SetAlphaSpeed(15.f);
			}
			else
			{
				m_iGunFlashTexture = 1;
				pGunFlash->SetTransformScale(XMFLOAT3(30.f, 30.f, 30.f));
				((CGunFlash*)pGunFlash)->SetAlphaSpeed(15.f);
			}

			pGunFlash->Set_TextureNumber(m_iGunFlashTexture);
			pLayer->Ready_Object(L"Effect", pGunFlash);


			//최종 검사
			if (m_bOneCheck == true)
			{
				XMFLOAT3 GunReaction = XMFLOAT3(GunReactionRand(-g_fGunReaction, g_fGunReaction), 
												GunReactionRand(-g_fGunReaction, g_fGunReaction), 
												GunReactionRand(-g_fGunReaction, g_fGunReaction));

				XMFLOAT3 m_vGoalPos = XMFLOAT3(Onehit.block.position.x + GunReaction.x, Onehit.block.position.y + GunReaction.y, Onehit.block.position.z + GunReaction.z);


				//Bullet의 방향벡터
				XMFLOAT3 m_vBulletDir;

				XMStoreFloat3(&m_vBulletDir, XMVector3Normalize(XMLoadFloat3(&m_vGoalPos)- XMLoadFloat3(&m_vWorldGunPos)));


				PxReal maxGunDistance = 1000.f;
				PxRaycastBuffer Gunhit;
				PxQueryFilterData Gunfd;
				Gunfd.flags |= PxQueryFlag::ePOSTFILTER;

				bool m_bTwoCheck = false;

				PxVec3 StartPos = PxVec3(m_vWorldGunPos.x, m_vWorldGunPos.y, m_vWorldGunPos.z);


				PxVec3 BulletDir = PxVec3(m_vBulletDir.x, m_vBulletDir.y, m_vBulletDir.z);

				m_bTwoCheck = m_pScene->raycast(StartPos, BulletDir, maxGunDistance, Gunhit, PxHitFlags(PxHitFlag::eDEFAULT), Gunfd);

				if (m_bTwoCheck == true)
				{
					m_vtestpos = XMFLOAT3(Gunhit.block.position.x, Gunhit.block.position.y, Gunhit.block.position.z);

					list<CGameObject*>* pObjList = pLayer->Find_ObjectList(L"OtherPlayer");

					const char* pName = NULL;// Gunhit.block.actor->getName();
					string strFullName;
					int iStartIdx = -1;

				
					if (pObjList)
					{
						pName = Gunhit.block.actor->getName();
						if (pName)
						{
							strFullName = pName;
							iStartIdx = strFullName.find("OtherPlayer_");
						}
					}

					//조명을 맞추는 경우====================================
					//CeilingLight
					const char* pLightName = NULL;
					string strLightName;
					pLightName = Gunhit.block.actor->getName();
					if (pLightName)
					{
						strLightName = pLightName;

						if ((strLightName == "CeilingLight") && (g_bBlackOut == false))
						{
							g_fLightPower = 0.2f;
							g_bBlackOut = true;
						}
					}
					//======================================================



					int intValue = -1;
					//m_ColllayData.bShoot = true;
					m_ColllayData.xmf3CollPos = m_vtestpos;
					
					if (iStartIdx != -1)
					{
						string strID = strFullName.substr(12, strFullName.length() - 11);
						intValue = atoi(strID.c_str());
						m_ColllayData.iCollPlayerID = intValue;

						//Hit Reaction==========================================
						CGameObject* pGameObject = CHitScreen::Create(m_pContext);

						m_vtestpos.y += 10.f;

						pGameObject->SetTransformPosition(m_vtestpos);
						pGameObject->SetTransformScale(XMFLOAT3(20.f, 20.f, 20.f));
						((CHitScreen*)pGameObject)->SetAlphaSpeed(2.f);
						pGameObject->Set_TextureNumber(0); //0 : 공격표시(히트) 1 : 데미지받았을때 (피격) OOPS
						pLayer->Ready_Object(L"Effect", pGameObject);
					}

					/*m_ColllayData.size = sizeof(Ser_COLLLAY_DATA);
					m_ColllayData.type = COLLISION_LAY;
					g_Client->sendPacket(sizeof(Ser_COLLLAY_DATA), COLLISION_LAY, reinterpret_cast<BYTE*>(&m_ColllayData));*/

					//Effect
					CGameObject* pGameObject = CBomb::Create(m_pContext);
					pGameObject->SetTransformPosition(m_vtestpos);
					pLayer->Ready_Object(L"Effect", pGameObject);

					
					////Hit Reaction==========================================
					//pGameObject = CHitScreen::Create(m_pContext);

					//m_vtestpos.y += 10.f;

					//pGameObject->SetTransformPosition(m_vtestpos);
					//pGameObject->SetTransformScale(XMFLOAT3(20.f, 20.f, 20.f));
					//((CHitScreen*)pGameObject)->SetAlphaSpeed(2.f);
					//pGameObject->Set_TextureNumber(0); //0 : 공격표시(히트) 1 : 데미지받았을때 (피격) OOPS
					//pLayer->Ready_Object(L"Effect", pGameObject);
					////=======================================================


	
					//Dynamic Actor(::레이에 맞은 다이나믹 오브젝트 리액션)
					PxRigidDynamic* actor = Gunhit.block.actor->is<PxRigidDynamic>();

					if (actor)
					{
						
						if (actor->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC)
							return;
						
						const PxTransform globalPose = actor->getGlobalPose();
						const PxVec3 localPos = globalPose.transformInv((Gunhit.block.position));

						
						TCHAR szUniCode[256] = { 0, };
						MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, actor->getName(), strlen(actor->getName()), szUniCode, 256);

						CGameObject* pObject = pLayer->Find_Object(szUniCode);
		
						if(pObject)
							AddForceAtLocalPos(*actor, BulletDir * pObject->GetWeight(), localPos, PxForceMode::eACCELERATION);
					}

				}

			}
			
		}
		else
		{
			m_bAbleReload = true;
		}
	}
}

void CPlayer::Soldier_Iron_AddVelocity(float fFallVel)
{
	m_fFallvelocity += 1000.f * m_fTimeDelta;
}

void CPlayer::Reload(void)
{
	((CGun*)m_pEquipment[0])->Reload();
	m_bAbleReload = false;
}

void CPlayer::Set_KeyState(_short sBitKeyState, float fAngle, short sHP)
{
	if (m_iHP != sHP)
	{
		//Hit Reaction==========================================
		CGameObject* pGameObject = CHitScreen::Create(m_pContext);

		XMFLOAT3 xmf3Pos = m_pTransform->m_vPos;
		xmf3Pos.y += 20.f;

		pGameObject->SetTransformPosition(xmf3Pos);
		pGameObject->SetTransformScale(XMFLOAT3(20.f, 20.f, 20.f));
		((CHitScreen*)pGameObject)->SetAlphaSpeed(2.f);
		pGameObject->Set_TextureNumber(1); //0 : 공격표시(히트) 1 : 데미지받았을때 (피격) OOPS
		CLayer* pLayer = CManagement::GetInstance()->GetScene()->FindLayer(L"Layer_GameLogic");
		pLayer->Ready_Object(L"Effect", pGameObject);
	}

	m_bKey[KEY_UP] = sBitKeyState & 1;
	m_bKey[KEY_DOWN] = sBitKeyState & 2;
	m_bKey[KEY_LEFT] = sBitKeyState & 4;
	m_bKey[KEY_RIGHT] = sBitKeyState & 8;
	m_bKey[KEY_Q] = sBitKeyState & 16;
	m_bKey[KEY_R] = sBitKeyState & 32;
	m_bKey[KEY_SHIFT] = sBitKeyState & 64;
	m_bKey[KEY_CTRL] = sBitKeyState & 128;
	m_bKey[KEY_SPACE] = sBitKeyState & 256;
	m_bKey[KEY_LCLICK] = sBitKeyState & 512;
	m_bKey[KEY_RCLICK] = sBitKeyState & 1024;

	m_pTransform->m_vAngle.y = fAngle;
	m_iHP = sHP;
}

void CPlayer::UpdateDir(void)
{
	XMVECTOR vDir;

	vDir = XMLoadFloat3(&m_pTransform->m_vDir);

	vDir = XMVector3TransformNormal(XMLoadFloat3(&m_vLook), XMLoadFloat4x4(&m_pTransform->m_matWorld));

	vDir = XMVector3Normalize(vDir);

	XMStoreFloat3(&m_pTransform->m_vDir,vDir);
}

_bool CPlayer::DynamicCameraCheck(void)
{
	if (CCameraMgr::GetInstance()->Get_CurCamera() == CCameraMgr::CAMERA_DYNAMIC)
	{
		return true;
	}

	return false;
}

void CPlayer::Render(void)
{
	CDynamicObject::Render();

	//pSphereMesh->Render();
}

void CPlayer::BuildObject(PxPhysics* pPxPhysics, PxScene* pPxScene, PxMaterial *pPxMaterial, PxControllerManager *pPxControllerManager)
{
	/*
	XMFLOAT3 vMin = *(CMeshMgr::GetInstance()->Get_MeshMin(m_uiObjNum));
	XMFLOAT3 vMax = *(CMeshMgr::GetInstance()->Get_MeshMax(m_uiObjNum));

	XMFLOAT3 _d3dxvExtents =
		XMFLOAT3((abs(vMin.x) + abs(vMax.x)) / 2, (abs(vMin.y) + abs(vMax.y)) / 2, (abs(vMin.z) + abs(vMax.z)) / 2);

	Player의 바운딩 박스 생성
	PxBoxControllerDesc PxBoxdesc;
	PxBoxdesc.position = PxExtendedVec3(0, 0, 0);
	PxBoxdesc.halfForwardExtent = _d3dxvExtents.y / 2;
	PxBoxdesc.halfSideExtent = _d3dxvExtents.z / 2;
	PxBoxdesc.halfHeight = _d3dxvExtents.x / 2;
	PxBoxdesc.slopeLimit = 10;
	PxBoxdesc.contactOffset = 0.00001;
	PxBoxdesc.upDirection = PxVec3(0, 1, 0);
	PxBoxdesc.material = pPxMaterial;
	*/

	m_pScene = pPxScene;

	PxCapsuleControllerDesc	PxCapsuledesc;
	PxCapsuledesc.position = PxExtendedVec3(0, 0, 0);
	PxCapsuledesc.radius = 5.0f;
	PxCapsuledesc.height = 10.0f;
	//캐릭터가 올라갈 수있는 장애물의 최대 높이를 정의합니다. 
	PxCapsuledesc.stepOffset = 2.f;
	
	//캐시 된 볼륨 증가.
	//성능을 향상시키기 위해 캐싱하는 컨트롤러 주변의 공간입니다.  이것은 1.0f보다 커야하지만 너무 크지 않아야하며, 2.0f보다 낮아야합니다.
	PxCapsuledesc.volumeGrowth = 1.9f;
	//캐릭터가 걸어 갈 수있는 최대 경사. 
	PxCapsuledesc.slopeLimit = cosf(XMConvertToRadians(15.f));
	//PxCapsuledesc.nonWalkableMode = PxControllerNonWalkableMode::eFORCE_SLIDING;
	PxCapsuledesc.upDirection = PxVec3(0, 1, 0);
	PxCapsuledesc.contactOffset = 0.001f; //접촉 오프셋-> 요게 타 객체와 부딪혔을때 영향을 주는 변수인듯(높을수록 덜덜떨림)
	PxCapsuledesc.material = pPxMaterial;
	PxCapsuledesc.behaviorCallback = this;
	PxCapsuledesc.reportCallback = this;

	m_pPxCharacterController = pPxControllerManager->createController(PxCapsuledesc);

}

void CPlayer::SetPosition(XMFLOAT3 vPosition)
{
	m_pPxCharacterController->setFootPosition(PxExtendedVec3(vPosition.x, vPosition.y, vPosition.z));
}

void CPlayer::SetRotate(XMFLOAT3 vRot)
{

}

//PhysX 함수
void CPlayer::PhysXUpdate(const FLOAT& fTimeDelta)
{
	//PhysX에 값을 전달해준다. 중력
	m_fFallvelocity -= m_fFallAcceleration * fTimeDelta * 50.f;

	if (m_fFallvelocity < -1000.f)
		m_fFallvelocity = -1000.f;

	m_pPxCharacterController->move(PxVec3(0, 1.f, 0) * fTimeDelta * m_fFallvelocity, 0, fTimeDelta, PxControllerFilters());

	PxControllerState   m_pPxState;

	//피직스 객체의 상태값을 m_pPxState에 넣어준다.
	m_pPxCharacterController->getState(m_pPxState);

	//윗쪽 충돌하거나 아랫쪽 충돌하면 m_fFallvelocity = 0.0f
	if (m_pPxState.collisionFlags == PxControllerCollisionFlag::eCOLLISION_DOWN || 
		m_pPxState.collisionFlags == PxControllerCollisionFlag::eCOLLISION_UP)
		m_fFallvelocity = 0.f;

	//현재 PhysX의 값으로 객체의 월드행렬을 만들어준다.
	m_pTransform->m_vPos = XMFLOAT3((_float)m_pPxCharacterController->getFootPosition().x, (_float)m_pPxCharacterController->getFootPosition().y,(_float) m_pPxCharacterController->getFootPosition().z);


	_float m_fRevice = 0.5f; //Player의 Y보정값(발이 지면에 안박히게 보정)

	if (!m_bIsSoldier)
		m_fRevice = 0.0f;

	XMMATRIX matTrans = XMMatrixTranslation(m_pTransform->m_vPos.x, m_pTransform->m_vPos.y + m_fRevice, m_pTransform->m_vPos.z);
	XMMATRIX matScale = XMMatrixScaling(m_pTransform->m_vScale.x, m_pTransform->m_vScale.y, m_pTransform->m_vScale.z);

	XMMATRIX matRotX = XMMatrixRotationX((_float)D3DXToRadian(m_pTransform->m_vAngle.x));
	XMMATRIX matRotY = XMMatrixRotationY((_float)D3DXToRadian(m_pTransform->m_vAngle.y));
	XMMATRIX matRotZ = XMMatrixRotationZ((_float)D3DXToRadian(m_pTransform->m_vAngle.z));

	XMStoreFloat4x4(&m_pTransform->m_matWorld, matScale * matRotX * matRotY * matRotZ * matTrans);

}

void CPlayer::UpdateBoost(const FLOAT& fTimeDelta)
{
	_float m_fSpeed = 0.0f;

	if (m_bIsSoldier) //일반상태
	{
		if(m_bBoostDelay)
		{
			m_fDelayCount += fTimeDelta;

			if (m_fDelayCount >= 5.0f)
			{
				m_fDelayCount = 0.0f;
				m_bBoostDelay = false;
			}
		}
		else
		{
			m_fSpeed = 10.f;

			m_fBoost += (fTimeDelta * m_fSpeed);

			if (m_fBoost >= m_fMaxBoost)
				m_fBoost = m_fMaxBoost;
		}
	}
	else //변신상태
	{
		m_fSpeed = 1.f;

		m_fBoost -= (fTimeDelta * m_fSpeed);

		if (m_fBoost <= 0.0f) //변신상태 -> 일반상태로 변환
		{
			//변신상태->일반상태로 변환될경우 Delay를 준다.
			m_bBoostDelay = true;

			m_fBoost = 0.0f;
			m_bIsSoldier = true;
			m_uiObjNum = m_uiObjNum_Normal;
			m_pAnimInfo = m_pAnimInfo_Normal;
			m_pMatBoneNode = m_pMatBoneNode_Normal;
			PlayAnimation((_ushort)PLAYER_idle);
			((CGun*)m_pEquipment[0])->ChangeWeapon(MESHNUM_GUN);
			m_iEquipBone = 0;
			m_pAnimInfo->Update(1.f);
		}
	}
}

bool CPlayer::UseTank(void)
{
	if (m_pTank)
		return true;
	else return false;	
}

_float CPlayer::GunReactionRand(_float a, _float b)
{
	return ((b - a)*((float)rand() / RAND_MAX)) + a;
}

//타 객체 충돌감지
void CPlayer::onShapeHit(const PxControllerShapeHit & hit)
{
	PxRigidDynamic* actor = hit.shape->getActor()->is<PxRigidDynamic>();

	if (actor)
	{
		if (actor->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC)
			return ;

		const PxVec3 upVector = hit.controller->getUpDirection();
		const PxF32 dp = hit.dir.dot(upVector);

		if (fabsf(dp) < 1e-3f)
		{
			const PxTransform globalPose = actor->getGlobalPose();
			const PxVec3 localPos = globalPose.transformInv(toVec3(hit.worldPos));
		
			
			TCHAR szUniCode[256] = { 0, };
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, actor->getName(), strlen(actor->getName()), szUniCode, 256);

			CLayer* pLayer = CManagement::GetInstance()->GetScene()->FindLayer(L"Layer_GameLogic");
			CGameObject* pObject = pLayer->Find_Object(szUniCode);


			if (pObject)
				AddForceAtLocalPos(*actor, hit.dir * hit.length * pObject->GetMass(), localPos, PxForceMode::eACCELERATION);

		}
	}
}

void CPlayer::AddForceAtLocalPos(PxRigidBody & body, const PxVec3 & force, const PxVec3 & pos, PxForceMode::Enum mode, bool wakeup)
{
	//transform pos to world space
	const PxVec3 globalForcePos = body.getGlobalPose().transform(pos);

	AddForceAtPosInternal(body, force, globalForcePos, mode, wakeup);
}

void CPlayer::AddForceAtPosInternal(PxRigidBody & body, const PxVec3 & force, const PxVec3 & pos, PxForceMode::Enum mode, bool wakeup)
{
	const PxTransform globalPose = body.getGlobalPose();
	const PxVec3 centerOfMass = globalPose.transform(body.getCMassLocalPose().p);

	const PxVec3 torque = (pos - centerOfMass).cross(force);
	body.addForce(force, mode, wakeup);
	body.addTorque(torque, mode, wakeup);
}

//세이프의 동작 플래그를 검색하는 함수
//CCT가 모양에 닿으면 CCT의 동작은 사용자가 사용자 정의 할 수 있습니다. 
//이 함수는 원하는 동작을 원하는 PxControllerBehaviorFlag플래그를 검색한다.
PxControllerBehaviorFlags CPlayer::getBehaviorFlags(const PxShape& shape, const PxActor& actor)
{
	const char* actorName = actor.getName();

	if (actor.getType() == PxActorType::eRIGID_DYNAMIC)
		//return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT | PxControllerBehaviorFlag::eCCT_SLIDE;
		return PxControllerBehaviorFlag::eCCT_SLIDE;

	return PxControllerBehaviorFlags(0);
}

PxControllerBehaviorFlags CPlayer::getBehaviorFlags(const PxController &)
{
	return PxControllerBehaviorFlags(0);
}

PxControllerBehaviorFlags CPlayer::getBehaviorFlags(const PxObstacle &)
{
	return PxControllerBehaviorFlags(0);
}

void CPlayer::SendPacketAlways(void)
{
	Ser_PLAYER_DATA m_pPlayerData;
	m_pPlayerData.size = sizeof(Ser_PLAYER_DATA);
	m_pPlayerData.type = CLIENT_POSITION;
	m_pPlayerData.ID = g_myid;
	m_pPlayerData.vPos = m_pTransform->m_vPos;
	m_pPlayerData.fAngle = m_fMoveAngleY;
	m_pPlayerData.SC_ID = g_CurrentScene;
	m_pPlayerData.strColllayData = m_ColllayData;
	m_pPlayerData.sBitKey = m_sBitKey;
	m_pPlayerData.sHP = m_iHP;
	g_Client->sendPacket(sizeof(Ser_PLAYER_DATA), CLIENT_POSITION, reinterpret_cast<BYTE*>(&m_pPlayerData));
}
