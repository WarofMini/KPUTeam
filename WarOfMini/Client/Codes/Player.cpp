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


XMFLOAT3		g_vPlayerPos;

CPlayer::CPlayer(ID3D11DeviceContext* pContext)
	: CDynamicObject(pContext)
	, m_dwState(SOLDIER_IDLE)
	, m_dwAniIdx(PLAYER_idle)
	, m_pComStateMachine(NULL)
	, m_pComGravity(NULL)
{
	m_pInput = CInput::GetInstance();
	m_vLook = XMFLOAT3(0.f, 1.f, 0.f);

	XMStoreFloat4x4(&m_matEquipBone[0], XMMatrixIdentity());
	XMStoreFloat4x4(&m_matEquipBone[1], XMMatrixIdentity());

	ZeroMemory(m_pEquipment, sizeof(CEquipment*) * 1);

	m_pServer_PlayerData = new Ser_PLAYER_DATA;

	m_fSpeed = 200.f;
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

HRESULT CPlayer::Initialize(ID3D11Device* pGraphicDev)
{
	if (FAILED(Ready_Component(pGraphicDev)))
		return E_FAIL;

	FAILED_CHECK(Prepare_StateMachine());

	m_uiObjNum = MESHNUM_PLAYER;

	m_pTransform->m_vScale = XMFLOAT3(1.f, 1.f, 1.f);
	m_pTransform->m_vAngle.x = 90.f;

	m_pTransform->m_vPos = XMFLOAT3(20.f, 0.f, 20.f);
	m_pTransform->m_vDir = XMFLOAT3(0.f, 0.f, -1.f);
	m_pAnimInfo->Set_Key((_ushort)m_dwAniIdx);

	m_pComStateMachine->Enter_State(SOLDIER_IDLE);

//	m_pServer_PlayerData->vPos = m_pTransform->m_vPos;

//	g_Client.sendPacket()

	return S_OK;
}

INT CPlayer::Update(const FLOAT& fTimeDelta)
{
	/*if (m_pInput->GetDIKeyStateOnce(DIK_1))
	{
		PlayAnimation(PLAYER_Jump);
	}
	if (m_pInput->GetDIKeyStateOnce(DIK_2))
	{
		PlayAnimation(PLAYER_JumpIn);
	}
	if (m_pInput->GetDIKeyStateOnce(DIK_3))
	{
		PlayAnimation(PLAYER_JumpLoop);
	}
	if (m_pInput->GetDIKeyStateOnce(DIK_4))
	{
		PlayAnimation(PLAYER_JumpOut);
	}*/
	if (m_pInput->Get_DIKeyState(DIK_1))
	{
		m_pComGravity->Add_Velocity(10.f);
		m_pComGravity->Set_OnGround(false);
	}
	Collision_Field(fTimeDelta);

	CDynamicObject::Update(fTimeDelta);

	//Dynamic카메라 체크 함수(Dynamic 카메라일시 Update 안돌린다.
	if (!DynamicCameraCheck())
	{
		Operate_StateMAchine(fTimeDelta);
		KeyState(fTimeDelta);
	}


	// Temp	-------------------------------------------------------------------------------
	
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
	// Equipment
	m_pEquipment[0] = CGun::Create(pGraphicDev, m_pContext);

	CComponent* pComponent = NULL;
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
	m_matEquipBone[0] = CMeshMgr::GetInstance()->Get_TransMeshBone(m_uiObjNum, 0, 0, m_pMatBoneNode);

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

	return S_OK;
}

void CPlayer::Operate_StateMAchine(const FLOAT& fTimeDelta)
{
	DWORD dwState = m_pComStateMachine->Get_State();

	switch (dwState)
	{
	case SOLDIER_IDLE:
 		if (m_dwState == SOLDIER_MOVE)
 		{
 			m_pComStateMachine->Enter_State(SOLDIER_MOVE);
 		}
		if (m_dwState == SOLDIER_LYING)
		{
			m_pComStateMachine->Enter_State(SOLDIER_LYING);
		}
		break;
	case SOLDIER_MOVE:
		if (m_dwState == SOLDIER_IDLE)
		{
 			m_pComStateMachine->Enter_State(SOLDIER_IDLE);
 		}
		if (m_dwState == SOLDIER_ROLL)
		{
			m_pComStateMachine->Enter_State(SOLDIER_ROLL);
		}
		break;
	case SOLDIER_LYING:
		if (m_dwState == SOLDIER_IDLE)
		{
			m_pComStateMachine->Enter_State(SOLDIER_IDLE);
		}
		break;
	case SOLDIER_ROLL:
		if (m_dwState == SOLDIER_IDLE)
		{
			m_pComStateMachine->Enter_State(SOLDIER_IDLE);
		}
		if (m_dwState == SOLDIER_MOVE)
		{
			m_pComStateMachine->Enter_State(SOLDIER_MOVE);
		}
		break;
	default:
		break;
	}
	m_pComStateMachine->Update_State(dwState);
}

void CPlayer::Collision_Field(const FLOAT& fTimeDelta)
{
	m_pComGravity->Move_Inertia(fTimeDelta, &m_pTransform->m_vPos);//로봇 날아ㅏ다닐때 쓰면 좋을듯. Add_Velocity

	if (m_pTransform->m_vPos.y <= 0)
	{
		m_pComGravity->Set_LandOn();
		m_pTransform->m_vPos.y = 0.f;
	}
	else
	{
		m_pComGravity->Set_OnGround(false);
	}
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

	Soldier_Move(fTimeDelta);
}

void CPlayer::Soldier_Move(const FLOAT& fTimeDelta)
{
	XMVECTOR vDir = XMVectorSet(m_pTransform->m_vDir.x, 0.f, m_pTransform->m_vDir.z, 0.0f);
	XMVECTOR vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR vRight = XMVector3Cross(vUp, vDir);
	XMVECTOR vMovePos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR vPos = XMLoadFloat3(&m_pTransform->m_vPos);

	vDir = XMVector3Normalize(vDir);
	vRight = XMVector3Normalize(vRight);

	switch (m_dwState)
	{
	case SOLDIER_MOVE:
		if (m_pInput->Get_DIKeyState(DIK_W))
			vMovePos += vDir * m_fSpeed * fTimeDelta;
		if (m_pInput->Get_DIKeyState(DIK_S))
			vMovePos -= vDir * m_fSpeed * fTimeDelta;
		if (m_pInput->Get_DIKeyState(DIK_A))
			vMovePos -= vRight * m_fSpeed * fTimeDelta;
		if (m_pInput->Get_DIKeyState(DIK_D))
			vMovePos += vRight * m_fSpeed * fTimeDelta;
		vMovePos = XMVector3Normalize(vMovePos);
		vPos += vMovePos;
		XMStoreFloat3(&m_pTransform->m_vPos, vPos);
		m_pTransform->Update(fTimeDelta);
		break;
	case SOLDIER_LYING:
		if (m_pInput->Get_DIKeyState(DIK_W))
			vMovePos += vDir * m_fSpeed * fTimeDelta * 0.5f;
		if (m_pInput->Get_DIKeyState(DIK_S))
			vMovePos -= vDir * m_fSpeed * fTimeDelta * 0.5f;
		if (m_pInput->Get_DIKeyState(DIK_A))
			vMovePos -= vRight * m_fSpeed * fTimeDelta * 0.5f;
		if (m_pInput->Get_DIKeyState(DIK_D))
			vMovePos += vRight * m_fSpeed * fTimeDelta * 0.5f;
		vMovePos = XMVector3Normalize(vMovePos);
		vPos += vMovePos;
		XMStoreFloat3(&m_pTransform->m_vPos, vPos);
		m_pTransform->Update(fTimeDelta);
		break;
	}
}

void CPlayer::UpdateDir(void)
{
	XMVECTOR vDir;

	vDir = XMLoadFloat3(&m_pTransform->m_vDir);

	XMVECTOR vLook = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	vDir = XMVector3TransformNormal(vLook, XMLoadFloat4x4(&m_pTransform->m_matWorld));

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
