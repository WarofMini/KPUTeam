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
, m_iHP(5)
, m_pTank(NULL)
{
	m_pInput = CInput::GetInstance();
	m_vLook = XMFLOAT3(0.f, 1.f, 0.f);

	XMStoreFloat4x4(&m_matEquipBone[0], XMMatrixIdentity());
	XMStoreFloat4x4(&m_matEquipBone[1], XMMatrixIdentity());

	ZeroMemory(m_pEquipment, sizeof(CEquipment*) * 2);
	ZeroMemory(&m_bKey, sizeof(bool) * KEY_END);

	m_pServer_PlayerData = new Ser_PLAYER_DATA;

	m_fSpeed = 40.f;


	XMStoreFloat4x4(&m_matBone, XMMatrixIdentity());
	m_iBoneNum = 0;
	pSphereMesh = CSphereMesh::Create(m_pContext, 1.f);
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
	


	// Update
	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);

	UpdateDir();

	Update_Equipment(fTimeDelta);


	pSphereMesh->Update(fTimeDelta);

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

	XMStoreFloat4x4(&m_matBone, XMLoadFloat4x4(&m_matBone) * matWorld);
	((CSphereMesh*)pSphereMesh)->SetmatWorld(&m_matBone);

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
	if (m_iHP < 0)
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
			if (Check_AnimationFrame())
			{
				m_iHP = 5;
				m_dwState = SOLDIER_IDLE;
				if (m_bIsSoldier)
					PlayAnimation(PLAYER_idle);
				else
					PlayAnimation(PLAYER_Iron_Idle);
				m_pComStateMachine->Enter_State(SOLDIER_IDLE);
			}
		}
	}
	else
	{
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

	Ser_ANIMATION_DATA m_AnimationData;
	m_AnimationData.size = sizeof(Ser_ANIMATION_DATA);
	m_AnimationData.type = CLIENT_ANIMATION;
	m_AnimationData.ID = g_myid;
	m_AnimationData.bImmediate = bImmediate;
	m_AnimationData.dwAniIdx = dwAniIdx;
	m_AnimationData.bIsSoldier = m_bIsSoldier;
	g_Client->sendPacket(sizeof(Ser_ANIMATION_DATA), CLIENT_ANIMATION, reinterpret_cast<BYTE*>(&m_AnimationData));

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

void CPlayer::SoldierChange(void)
{
	if (m_bIsSoldier)
	{
		m_bIsSoldier = false;
		m_uiObjNum = m_uiObjNum_Iron;
		m_pAnimInfo = m_pAnimInfo_Iron;
		m_pMatBoneNode = m_pMatBoneNode_Iron;
		PlayAnimation((_ushort)PLAYER_Iron_Idle);
		((CGun*)m_pEquipment[0])->ChangeWeapon(MESHNUM_SPECIALGUN);
		m_iEquipBone = 2;
	}
	else
	{
		m_bIsSoldier = true;
		m_uiObjNum = m_uiObjNum_Normal;
		m_pAnimInfo = m_pAnimInfo_Normal;
		m_pMatBoneNode = m_pMatBoneNode_Normal;
		PlayAnimation((_ushort)PLAYER_idle);
		((CGun*)m_pEquipment[0])->ChangeWeapon(MESHNUM_GUN);
		m_iEquipBone = 0;
	}

	m_pAnimInfo->Update(1.f);
}

void CPlayer::KeyCheck(void)
{
	m_eMoveDir = DIR_END;
	m_vMoveDir = XMFLOAT3(0.f, 0.f, 0.f);
	ZeroMemory(&m_bKey, sizeof(bool) * KEY_END);

	if (m_pInput->Get_DIKeyState(DIK_W))
		m_bKey[KEY_UP] = true;
	if (m_pInput->Get_DIKeyState(DIK_S))
		m_bKey[KEY_DOWN] = true;
	if (m_pInput->Get_DIKeyState(DIK_A))
		m_bKey[KEY_LEFT] = true;
	if (m_pInput->Get_DIKeyState(DIK_D))
		m_bKey[KEY_RIGHT] = true;

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
	_long lMouseMove = 0;

	if (lMouseMove = m_pInput->Get_DIMouseMove(CInput::DIMS_X))
	{
		if (m_pTransform->m_vAngle.y >= 360.f)
			m_pTransform->m_vAngle.y = 0.f;
		if (m_pTransform->m_vAngle.y < 0.f)
			m_pTransform->m_vAngle.y = 360.f;
		m_pTransform->m_vAngle.y += lMouseMove * fTimeDelta * 5.f;
	}

	if(m_bIsSoldier)
		Soldier_Move(fTimeDelta);
	else
		Soldier_Iron_Move(fTimeDelta);

	Soldier_Fire(fTimeDelta);

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
	}

	if (m_pTank)
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
	if (!m_bFire)
	{
		m_fRateOfFire = 0.f;
		return;
	}
	m_fRateOfFire += fTimeDelta;
	if (m_fRateOfFire >= 0.2f)
	{
		m_fRateOfFire = 0.1f;
		if (((CGun*)m_pEquipment[0])->Fire())
		{
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

			//최종 검사
			if (m_bOneCheck == true)
			{
				XMFLOAT3 m_vGoalPos = XMFLOAT3(Onehit.block.position.x, Onehit.block.position.y, Onehit.block.position.z);

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
					CLayer* pLayer = CManagement::GetInstance()->GetScene()->FindLayer(L"Layer_GameLogic");


					m_vtestpos = XMFLOAT3(Gunhit.block.position.x, Gunhit.block.position.y, Gunhit.block.position.z);


					const char* pName = Gunhit.block.actor->getName();
					string strFullName = pName;
					int iStartIdx = strFullName.find("OtherPlayer_");

					if (iStartIdx != -1)
					{
						string strID = strFullName.substr(12, strFullName.length() - 11);
						int intValue = atoi(strID.c_str());

						Ser_COLLLAY_DATA strColData;
						strColData.size = sizeof(Ser_COLLLAY_DATA);
						strColData.type = COLLISION_LAY;
						strColData.ID = intValue;
						g_Client->sendPacket(sizeof(Ser_COLLLAY_DATA), COLLISION_LAY, reinterpret_cast<BYTE*>(&strColData));
					}


					//Effect
					CGameObject* pGameObject = CBomb::Create(m_pContext);

					pGameObject->SetTransformPosition(m_vtestpos);
				
					pLayer->Ready_Object(L"Effect", pGameObject);
					
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

	pSphereMesh->Render();
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
	PxCapsuledesc.contactOffset = 0.1f; //접촉 오프셋
	PxCapsuledesc.material = pPxMaterial;

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

bool CPlayer::UseTank(void)
{
	if (m_pTank)
		return true;
	else return false;	
}

//타 객체 충돌감지
void CPlayer::onShapeHit(const PxControllerShapeHit & hit)
{
	PxRigidDynamic* actor = hit.shape->getActor()->is<PxRigidDynamic>();

	if (actor)
	{
		if (actor->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC)
			return;

		const PxVec3 upVector = hit.controller->getUpDirection();
		const PxF32 dp = hit.dir.dot(upVector);

		if (fabsf(dp) < 1e-3f)
		{
			const PxTransform globalPose = actor->getGlobalPose();
			const PxVec3 localPos = globalPose.transformInv(toVec3(hit.worldPos));
			AddForceAtLocalPos(*actor, hit.dir * hit.length * 0.1f, localPos, PxForceMode::eACCELERATION);
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
	//const char* actorName = actor.getName();

	if(actor.getType() == PxActorType::eRIGID_DYNAMIC)
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

PxQueryHitType::Enum CPlayer::preFilter(const PxFilterData & filterData, const PxShape * shape, const PxRigidActor * actor, PxSceneQueryFlags & queryFlags)
{
	return PxSceneQueryHitType::eBLOCK;
}

PxQueryHitType::Enum CPlayer::postFilter(const PxFilterData & filterData, const PxSceneQueryHit & hit)
{
	return PxSceneQueryHitType::eBLOCK;
}

void CPlayer::SendPacketAlways(void)
{
	Ser_PLAYER_DATA m_pPlayerData;
	m_pPlayerData.size = sizeof(Ser_PLAYER_DATA);
	m_pPlayerData.type = CLIENT_POSITION;
	m_pPlayerData.ID = g_myid;
	m_pPlayerData.vPos = m_pTransform->m_vPos;
	m_pPlayerData.vDir = m_pTransform->m_vAngle;
	g_Client->sendPacket(sizeof(Ser_PLAYER_DATA), CLIENT_POSITION, reinterpret_cast<BYTE*>(&m_pPlayerData));
}
