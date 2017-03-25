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

HRESULT COtherPlayer::Initialize(ID3D11Device* pGraphicDev)
{
	if (FAILED(Ready_Component(pGraphicDev)))
		return E_FAIL;


	m_uiObjNum = MESHNUM_PLAYER;


	m_pTransform->m_vScale = XMFLOAT3(0.06f, 0.06f, 0.06f);
	m_pTransform->m_vAngle.x = 90.f;
	m_pTransform->m_vPos = XMFLOAT3(10.f, 10.f, 10.f);
	m_pTransform->m_vDir = XMFLOAT3(0.f, 0.f, -1.f);
	m_pAnimInfo->Set_Key(PLAYER_idle);

	return S_OK;
}

INT COtherPlayer::Update(const FLOAT& fTimeDelta)
{
	CDynamicObject::Update(fTimeDelta);

	// Temp	-------------------------------------------------------------------------------

	g_vPlayerPos = m_pTransform->m_vPos;


	// Update
	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);

	m_pTransform->m_vDir = XMFLOAT3(m_pTransform->m_matWorld._31, m_pTransform->m_matWorld._32, m_pTransform->m_matWorld._33);
	XMStoreFloat3(&m_pTransform->m_vDir, XMVector3Normalize(XMLoadFloat3(&m_pTransform->m_vDir)));

	//m_pTransform->Update_MatrixNotXRot();

	Update_Equipment(fTimeDelta);

	return 0;
}
void COtherPlayer::Release(void)
{
	CDynamicObject::Release();
	Safe_Release(m_pEquipment[0]);
}

HRESULT COtherPlayer::Ready_Component(ID3D11Device* pGraphicDev)
{
	if (FAILED(CDynamicObject::Ready_Component()))
		return E_FAIL;
	// Equipment
	m_pEquipment[0] = CGun::Create(pGraphicDev, m_pContext);

	return S_OK;
}

void COtherPlayer::Update_Equipment(const FLOAT& fTimeDelta)
{
	m_matEquipBone[0] = CMeshMgr::GetInstance()->Get_TransMeshBone(m_uiObjNum, 0, 24, m_pMatBoneNode);

	XMMATRIX matWorld = XMLoadFloat4x4(&m_pTransform->m_matWorld);
	XMStoreFloat4x4(&m_matEquipBone[0], XMLoadFloat4x4(&m_matEquipBone[0]) * matWorld);

	m_pEquipment[0]->SetParent(m_matEquipBone[0]);
	m_pEquipment[0]->Update(fTimeDelta);


}

void COtherPlayer::InputKey(const FLOAT& fTimeDelta)
{

}

void COtherPlayer::Move(const FLOAT& fTimeDelta)
{

}