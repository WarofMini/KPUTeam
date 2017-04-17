#include "stdafx.h"
#include "Include.h"
#include "OtherPlayer.h"
#include "AnimationInfo.h"
#include "Transform.h"
#include "MeshMgr.h"
#include "Gun.h"
#include "Management.h"

COtherPlayer::COtherPlayer(ID3D11DeviceContext* pContext)
	: CDynamicObject(pContext)
	, m_iEquipBone(0)
	, m_dwAniIdx(PLAYER_idle)
	, m_bIsSoldier(true)
{
	m_vLook = XMFLOAT3(0.f, 0.f, -1.f);

	XMStoreFloat4x4(&m_matEquipBone[0], XMMatrixIdentity());
	XMStoreFloat4x4(&m_matEquipBone[1], XMMatrixIdentity());

	ZeroMemory(m_pEquipment, sizeof(CEquipment*) * 1);

	m_pServer_PlayerData = new Ser_PLAYER_DATA;
}

COtherPlayer::~COtherPlayer(void)
{
}

COtherPlayer* COtherPlayer::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
{
	COtherPlayer* pObject = new COtherPlayer(pContext);

	if (FAILED(pObject->Initialize(pGraphicDev)))
		Safe_Release(pObject);

	return pObject;
}

COtherPlayer* COtherPlayer::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, XMFLOAT3 vPos, int iID)
{
	COtherPlayer* pObject = new COtherPlayer(pContext);

	if (FAILED(pObject->Initialize(pGraphicDev)))
		Safe_Release(pObject);

	pObject->m_pTransform->m_vPos = vPos;
	pObject->m_iID = iID;
	return pObject;
}

HRESULT COtherPlayer::Initialize(ID3D11Device* pGraphicDev)
{
	m_uiObjNum = MESHNUM_PLAYER;
	m_uiObjNum_Iron = MESHNUM_PLAYER2;

	if (FAILED(Ready_Component(pGraphicDev)))
		return E_FAIL;

	m_pTransform->m_vScale = XMFLOAT3(1.f, 1.f, 1.f);
	m_pTransform->m_vAngle.x = 90.f;
	m_pAnimInfo->Set_Key(m_dwAniIdx);

	// Equipment
	m_pEquipment[0] = CGun::Create(pGraphicDev, m_pContext);

	return S_OK;
}

INT COtherPlayer::Update(const FLOAT& fTimeDelta)
{
	CDynamicObject::Update(fTimeDelta);

	// Temp	-------------------------------------------------------------------------------

	// Update
	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);

	UpdateDir();

	Update_Equipment(fTimeDelta);

	return 0;
}
void COtherPlayer::Release(void)
{
	CDynamicObject::Release();
	Safe_Release(m_pEquipment[0]);
}

void COtherPlayer::UpdateDir(void)
{
	XMVECTOR vDir;

	vDir = XMLoadFloat3(&m_pTransform->m_vDir);

	vDir = XMVector3TransformNormal(XMLoadFloat3(&m_vLook), XMLoadFloat4x4(&m_pTransform->m_matWorld));

	vDir = XMVector3Normalize(vDir);

	XMStoreFloat3(&m_pTransform->m_vDir, vDir);
}

HRESULT COtherPlayer::Ready_Component(ID3D11Device* pGraphicDev)
{
	if (FAILED(CDynamicObject::Ready_Component()))
		return E_FAIL;

	m_pAnimInfo_Normal = m_pAnimInfo;
	m_pMatBoneNode_Normal = m_pMatBoneNode;
	m_uiObjNum_Normal = m_uiObjNum;

	CComponent* pComponent = NULL;

	// AnimationInfo	Iron
	pComponent = CAnimationInfo::Create(MESHNUM(m_uiObjNum_Iron));
	m_pAnimInfo_Iron = dynamic_cast<CAnimationInfo*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_AnimInfo_Iron", pComponent));
	// Matrix Bone		Iron
	m_pMatBoneNode_Iron = new MATNODE;
	CMeshMgr::GetInstance()->CreateBoneNode(m_uiObjNum_Iron, m_pMatBoneNode_Iron);

	return S_OK;
}

void COtherPlayer::Update_Equipment(const FLOAT& fTimeDelta)
{
	m_matEquipBone[0] = CMeshMgr::GetInstance()->Get_TransMeshBone(m_uiObjNum, 0, m_iEquipBone, m_pMatBoneNode);

	XMMATRIX matWorld = XMLoadFloat4x4(&m_pTransform->m_matWorld);
	XMStoreFloat4x4(&m_matEquipBone[0], XMLoadFloat4x4(&m_matEquipBone[0]) * matWorld);

	m_pEquipment[0]->SetParent(m_matEquipBone[0]);
	m_pEquipment[0]->Update(fTimeDelta);


}

void COtherPlayer::SetPlayerData(XMFLOAT3 vPos, XMFLOAT3 vDir)
{
	m_pTransform->m_vPos = vPos;
	m_pTransform->m_vAngle = vDir;
}

void COtherPlayer::PlayAnimation(DWORD dwAniIdx, bool bImmediate)
{
	if (bImmediate)
		m_pAnimInfo->Set_KeyImm((_ushort)dwAniIdx);
	m_pAnimInfo->Set_Key((_ushort)dwAniIdx);
	m_dwAniIdx = dwAniIdx;
}

void COtherPlayer::SoldierChange(void)
{
	if (m_bIsSoldier)
	{
		m_bIsSoldier = false;
		m_uiObjNum = m_uiObjNum_Iron;
		m_pAnimInfo = m_pAnimInfo_Iron;
		m_pMatBoneNode = m_pMatBoneNode_Iron;
		PlayAnimation((_ushort)PLAYER_Iron_Idle, true);
		((CGun*)m_pEquipment[0])->ChangeWeapon(MESHNUM_SPECIALGUN);
		m_iEquipBone = 2;
	}
	else
	{
		m_bIsSoldier = true;
		m_uiObjNum = m_uiObjNum_Normal;
		m_pAnimInfo = m_pAnimInfo_Normal;
		m_pMatBoneNode = m_pMatBoneNode_Normal;
		PlayAnimation((_ushort)PLAYER_idle, true);
		((CGun*)m_pEquipment[0])->ChangeWeapon(MESHNUM_GUN);
		m_iEquipBone = 0;
	}
	m_pAnimInfo->Update(1.f);
}
