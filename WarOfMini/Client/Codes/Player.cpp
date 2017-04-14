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
#include "Gravity.h"
#include "Layer.h"
#include "DefaultObject.h"
#include "Management.h"
#include "Scene.h"

XMFLOAT3		g_vPlayerPos;

CPlayer::CPlayer(ID3D11DeviceContext* pContext)
	: CDynamicObject(pContext)
	, m_dwState(SOLDIER_IDLE)
	, m_dwAniIdx(PLAYER_idle)
	, m_pComStateMachine(NULL)
	, m_pCalculator(NULL)
	, m_pRigidBody(NULL)
	, m_pComGravity(NULL)
	, m_eMoveDir(DIR_END)
	, m_vMoveDir(0.f, 0.f, 0.f)
	, m_bIsSoldier(true)
	, m_fTimeDelta(0.f)
	, m_iEquipBone(0)
	, m_fRollSpeed(0.f)
	, m_fRateOfFire(0.f)
	, m_bFire(false)
	, m_bAbleReload(false)
{
	m_pInput = CInput::GetInstance();
	m_vLook = XMFLOAT3(0.f, 1.f, 0.f);

	XMStoreFloat4x4(&m_matEquipBone[0], XMMatrixIdentity());
	XMStoreFloat4x4(&m_matEquipBone[1], XMMatrixIdentity());

	ZeroMemory(m_pEquipment, sizeof(CEquipment*) * 2);
	ZeroMemory(&m_bKey, sizeof(bool) * KEY_END);

	m_pServer_PlayerData = new Ser_PLAYER_DATA;

	m_fSpeed = 50.f;
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
	m_uiObjNum = MESHNUM_PLAYER;
	m_uiObjNum_Iron = MESHNUM_PLAYER2;

	if (FAILED(Ready_Component(pGraphicDev)))
		return E_FAIL;

	FAILED_CHECK(Prepare_StateMachine());

	m_pTransform->m_vScale = XMFLOAT3(1.f, 1.f, 1.f);
	m_pTransform->m_vAngle.x = 90.f;

	m_pTransform->m_vDir = XMFLOAT3(0.f, 0.f, -1.f);
	m_pAnimInfo->Set_Key((_ushort)m_dwAniIdx);

	m_pComStateMachine->Enter_State(SOLDIER_IDLE);

	//m_pServer_PlayerData->vPos = m_pTransform->m_vPos;

	//g_Client.sendPacket(sizeof(XMFLOAT3),INIT_CLIENT,)

	// Equipment
	m_pEquipment[0] = CGun::Create(pGraphicDev, m_pContext);

	return S_OK;
}

INT CPlayer::Update(const FLOAT& fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;

	Collision_Field(fTimeDelta);
	
	CDynamicObject::Update(fTimeDelta);
	
	//Dynamic카메라 체크 함수(Dynamic 카메라일시 Update 안돌린다.
	if (!DynamicCameraCheck())
	{
		Operate_StateMAchine(fTimeDelta);
		KeyState(fTimeDelta);
	}

	// Temp	----------------------------------------------------------------------------
	
	// Update
	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);


	UpdateDir();


	Update_Equipment(fTimeDelta);

	return 0;
}

void CPlayer::Release(void)
{
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

	//Gravity
	pComponent = m_pComGravity = CGravity::Create(100.f);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Gravity", pComponent));

	return S_OK;
}

void CPlayer::Update_Equipment(const FLOAT& fTimeDelta)
{
	m_matEquipBone[0] = CMeshMgr::GetInstance()->Get_TransMeshBone(m_uiObjNum, 0, m_iEquipBone, m_pMatBoneNode);

	XMMATRIX matWorld = XMLoadFloat4x4(&m_pTransform->m_matWorld);
	XMStoreFloat4x4(&m_matEquipBone[0], XMLoadFloat4x4(&m_matEquipBone[0]) * matWorld);
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
			m_pComGravity->Set_LandOff(50.f);				// 일단 이렇게
		}
		break;
	case SOLDIER_MOVE:
		if (m_dwState == SOLDIER_IDLE)
 			m_pComStateMachine->Enter_State(SOLDIER_IDLE);
		if (m_dwState == SOLDIER_ROLL)
		{
			m_pComStateMachine->Enter_State(SOLDIER_ROLL);
			m_pComGravity->Set_LandOff(30.f);				// 일단 이렇게
			m_fRollSpeed = m_fSpeed * 2.f;
			m_fRollDir = m_vMoveDir;
		}
		if (m_dwState == SOLDIER_JUMP)
		{
			m_pComStateMachine->Enter_State(SOLDIER_JUMP);
			m_pComGravity->Set_LandOff(50.f);				// 일단 이렇게
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

void CPlayer::Collision_Field(const FLOAT& fTimeDelta)
{
	m_pComGravity->Move_Inertia(fTimeDelta, &m_pTransform->m_vPos);//로봇 날아다닐때 쓰면 좋을듯. Add_Velocity

	
	if (m_pTransform->m_vPos.y <= 0.f)
	{
		m_pTransform->m_vPos.y = 0.f;
		m_pComGravity->Set_LandOn();
	}



	////반직선의 원점, 방향, 삼각형 정점0, 1, 2, 교점의 매개변수

	//CLayer* pLayer = CManagement::GetInstance()->GetScene()->FindLayer(L"Layer_GameLogic");

	//if (pLayer == NULL)
	//	return;

	//list<CGameObject*>* pObjList = pLayer->Find_ObjectList(L"StaticObject");

	//if (pObjList == NULL)
	//	return;

	//list<CGameObject*>::iterator iter = pObjList->begin();
	//list<CGameObject*>::iterator iter_end = pObjList->end();


	//_float tmin = 1000.0f;
	//XMVECTOR vRayDir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	//XMVECTOR vOrigin = XMLoadFloat3(&m_pTransform->m_vPos);


	//for (iter; iter != iter_end; ++iter) //배치된 오브젝트를 순회
	//{

	//	int nOffset = 3;

	//	int nPrimitives = CMeshMgr::GetInstance()->Get_MeshVtxCnt(((CDefaultObj*)(*iter))->GetObjNum()) / 3;

	//	VTXTEX*	m_pTex = CMeshMgr::GetInstance()->Get_MeshVtxTex(((CDefaultObj*)(*iter))->GetObjNum());


	//	if (m_pTex == NULL)
	//		continue;

	//	XMVECTOR v0, v1, v2;


	//	XMMATRIX	matWorld;
	//	XMVECTOR    vecTest, vecLocalPos, vecLocalDir;

	//	//객체의 로컬행렬을 구한다
	//	matWorld = XMLoadFloat4x4(&((CTransform*)((CDefaultObj*)(*iter))->Get_Component(L"Com_Transform"))->m_matWorld);
	//	matWorld = XMMatrixInverse(&vecTest, matWorld);

	//	//로컬 위치, 방향을 구한다
	//	vecLocalPos = XMVector3TransformCoord(vOrigin, matWorld);
	//	vecLocalDir = XMVector3TransformNormal(vRayDir, matWorld);
	//	vecLocalDir = XMVector3Normalize(vecLocalDir);




	//	//객체의 폴리곤을 순회
	//	for (int i = 0; i < nPrimitives; ++i)
	//	{

	//		v0 = XMLoadFloat3(&m_pTex[i * nOffset + 0].vPos);
	//		v1 = XMLoadFloat3(&m_pTex[i * nOffset + 1].vPos);
	//		v2 = XMLoadFloat3(&m_pTex[i * nOffset + 2].vPos);

	//		
	//		float fDist = 0.0f;


	//		if (XNA::IntersectRayTriangle(vecLocalPos, vecLocalDir, v0, v1, v2, &fDist))
	//		{
	//			if (fDist < tmin)
	//			{
	//				tmin = fDist;
	//			
	//				m_pComGravity->Set_GroundDist(tmin);


	//				m_pComGravity->Set_BeforePos(vOrigin);
	//			}
	//		}
	//	}

	//}

	//if (m_pComGravity->Get_GroundDist() != 0.0f && tmin == 1000.0f)
	//{
	//	m_pComGravity->Set_LandOn(); //착지 한걸로 하자

	//	XMVECTOR dist = (vRayDir * m_pComGravity->Get_GroundDist()) * 0.3f;
	//	XMVECTOR beforepos = m_pComGravity->Get_BeforePos();

	//	XMVECTOR vPos = beforepos + dist;
	//	XMStoreFloat3(&m_pTransform->m_vPos, vPos);
	//	
	//	_float fdist = 0.0f;
	//	m_pComGravity->Set_GroundDist(fdist);
	//}
	//else
	//{
	//	//이부분을 메시충돌로 하자
	//	if (tmin <= 0.1f)
	//	{
	//		_float fdist = 0.0f;
	//		m_pComGravity->Set_GroundDist(fdist);

	//		tmin = 0.0f;
	//		m_pComGravity->Set_LandOn(); //착지 한걸로 하자
	//		XMVECTOR vPos = vOrigin + (vRayDir * tmin);
	//		XMStoreFloat3(&m_pTransform->m_vPos, vPos);
	//	}
	//	else
	//	{
	//		m_pComGravity->Set_OnGround(false); //아직 착지 안함
	//	}
	//}

}

void CPlayer::PlayAnimation(DWORD dwAniIdx, bool bImmediate)
{
	if (bImmediate)
		m_pAnimInfo->Set_KeyImm((_ushort)dwAniIdx);
	m_pAnimInfo->Set_Key((_ushort)dwAniIdx);
	m_dwAniIdx = dwAniIdx;
}

bool CPlayer::Check_AnimationFrame(void)
{
	if (m_pAnimInfo->Get_CurFrame() >= m_pAnimInfo->Get_LastFrame())
		return true;
	return false;
}

bool CPlayer::IsOnGround(void)
{
	return m_pComGravity->Get_OnGround();
}

MOVE_DIR* CPlayer::GetMoveDir(void)
{
	return &m_eMoveDir;
}

void CPlayer::SoldierChange(void)
{//처음에 좆같은거는 어떻게?
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

	if (m_eMoveDir != DIR_END)
	{
		Ser_PLAYER_DATA m_pPlayerData;
		m_pPlayerData.size = sizeof(Ser_PLAYER_DATA);
		m_pPlayerData.type = CLIENT_POSITION;
		m_pPlayerData.ID = g_myid;
		m_pPlayerData.vPos = m_pTransform->m_vPos;
		g_Client->sendPacket(sizeof(Ser_PLAYER_DATA), CLIENT_POSITION, reinterpret_cast<BYTE*>(&m_pPlayerData));
	}

	Soldier_Fire(fTimeDelta);
}

void CPlayer::Soldier_Move(const FLOAT& fTimeDelta)
{
	m_fSpeed = 50.f;
	XMVECTOR vDir;// = XMLoadFloat3(&m_vMoveDir);
	XMVECTOR vPos = XMLoadFloat3(&m_pTransform->m_vPos);

	switch (m_dwState)
	{
	case SOLDIER_MOVE:
		vDir = XMLoadFloat3(&m_vMoveDir);
		if (m_dwAniIdx == PLAYER_sprint)
			vPos += vDir * m_fSpeed * fTimeDelta * 1.5f;
		else
			vPos += vDir * m_fSpeed * fTimeDelta;
		XMStoreFloat3(&m_pTransform->m_vPos, vPos);
		m_pTransform->Update(fTimeDelta);
		break;
	case SOLDIER_LYING:
		vDir = XMLoadFloat3(&m_vMoveDir);
		if (m_dwAniIdx != PLAYER_Lying && m_dwAniIdx != PLAYER_LyingShoot)
		{
			vPos += vDir * m_fSpeed * fTimeDelta * 0.5f;
			XMStoreFloat3(&m_pTransform->m_vPos, vPos);
			m_pTransform->Update(fTimeDelta);
		}
		break;
	case SOLDIER_JUMP:
		vDir = XMLoadFloat3(&m_vMoveDir);
		vPos += vDir * m_fSpeed * fTimeDelta;
		XMStoreFloat3(&m_pTransform->m_vPos, vPos);
		m_pTransform->Update(fTimeDelta);
		break;
	case SOLDIER_ROLL:
		vDir = XMLoadFloat3(&m_fRollDir);
		vPos += vDir * m_fRollSpeed * fTimeDelta;
		m_fRollSpeed -= 20.f * fTimeDelta;
		XMStoreFloat3(&m_pTransform->m_vPos, vPos);
		m_pTransform->Update(fTimeDelta);
		break;
	}
	//g_Client.sendPacket(sizeof(char), CLIENT_POSITION, reinterpret_cast<BYTE*>(&m_pTransform->m_vPos));
}

void CPlayer::Soldier_Iron_Move(const FLOAT& fTimeDelta)
{
	m_fSpeed = 70.f;
	XMVECTOR vDir;// = XMLoadFloat3(&m_vMoveDir);
	XMVECTOR vPos = XMLoadFloat3(&m_pTransform->m_vPos);

	switch (m_dwState)
	{
	case SOLDIER_MOVE:
		vDir = XMLoadFloat3(&m_vMoveDir);
		if (m_dwAniIdx == PLAYER_Iron_Sprint)
			vPos += vDir * m_fSpeed * fTimeDelta * 1.5f;
		else
			vPos += vDir * m_fSpeed * fTimeDelta;
		XMStoreFloat3(&m_pTransform->m_vPos, vPos);
		m_pTransform->Update(fTimeDelta);
		break;
	case SOLDIER_LYING:
		vDir = XMLoadFloat3(&m_vMoveDir);
		if (m_dwAniIdx != PLAYER_Iron_Lying && m_dwAniIdx != PLAYER_Iron_LyingShoot)
		{
			vPos += vDir * m_fSpeed * fTimeDelta * 0.5f;
			XMStoreFloat3(&m_pTransform->m_vPos, vPos);
			m_pTransform->Update(fTimeDelta);
		}
		break;
	case SOLDIER_JUMP:
		vDir = XMLoadFloat3(&m_vMoveDir);
		vPos += vDir * m_fSpeed * fTimeDelta;
		XMStoreFloat3(&m_pTransform->m_vPos, vPos);
		m_pTransform->Update(fTimeDelta);
		break;
	case SOLDIER_ROLL:
		vDir = XMLoadFloat3(&m_fRollDir);
		vPos += vDir * m_fRollSpeed * fTimeDelta;
		m_fRollSpeed -= 20.f * fTimeDelta;
		XMStoreFloat3(&m_pTransform->m_vPos, vPos);
		m_pTransform->Update(fTimeDelta);
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
		}
		else
		{
			m_bAbleReload = true;
		}
	}
}

void CPlayer::Soldier_Iron_AddVelocity(float fFallVel)
{
	m_pComGravity->Add_Velocity(fFallVel * m_fTimeDelta);
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
