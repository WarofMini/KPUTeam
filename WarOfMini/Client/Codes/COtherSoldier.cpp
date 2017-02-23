#include "stdafx.h"
#include "COtherSoldier.h"
#include "RenderMgr.h"
#include "Texture.h"
#include "Info.h"
#include "VIBuffer.h"
#include "AnimationMgr.h"
#include "ResourcesMgr.h"
#include "DynamicMesh.h"
#include "ShaderMgr.h"
#include "Camera.h"
#include "Device.h"
#include "Shader.h"
#include "Input.h"

COtherSoldier::COtherSoldier()
	: m_dwAniIdx(0)
	, m_pInputDev(NULL)
{
	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;
	m_pVerTex = NULL;
}

COtherSoldier::~COtherSoldier()
{
	Release();
}

HRESULT COtherSoldier::Initialize(void)
{
	if (FAILED(AddComponent()))
		return E_FAIL;

	FAILED_CHECK(Prepare_StateMachine());

	m_pInfo->m_vScale = D3DXVECTOR3(0.1f, 0.1f, 0.1f);
	m_pInfo->m_vPos = D3DXVECTOR3(0.f, 0.f, 0.f);

	m_pInputDev = CInput::GetInstance();

	//m_pComStateMachine->Enter_State(SOLDIER_IDLE);
	CRenderMgr::GetInstance()->AddRenderGroup(TYPE_NONEALPHA, this);

	return S_OK;
}

int COtherSoldier::Update(void)
{
	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);

	Operate_StateMAchine(0);
	KeyInput();

	CObject::Update();

	return 0;
}

void COtherSoldier::Render(void)
{
	ConstantBuffer cb;
	D3DXMatrixTranspose(&cb.matWorld, &m_pInfo->m_matWorld);
	D3DXMatrixTranspose(&cb.matView, &CCamera::GetInstance()->m_matView);
	D3DXMatrixTranspose(&cb.matProjection, &CCamera::GetInstance()->m_matProj);
	m_pGrapicDevice->m_pDeviceContext->UpdateSubresource(m_pBuffer->m_ConstantBuffer, 0, NULL, &cb, 0, 0);

	m_pGrapicDevice->m_pDeviceContext->VSSetShader(m_pVertexShader->m_pVertexShader, NULL, 0);
	m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pBuffer->m_ConstantBuffer);
	//////////////////
	m_pGrapicDevice->m_pDeviceContext->PSSetShader(m_pPixelShader->m_pPixelShader, NULL, 0);
	m_pGrapicDevice->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_pTextureRV);
	m_pGrapicDevice->m_pDeviceContext->PSSetSamplers(0, 1, &m_pTexture->m_pSamplerLinear);

	//m_pBuffer->Render();
	dynamic_cast<CDynamicMesh*>(m_pBuffer)->PlayAnimation(m_dwAniIdx);
}

void COtherSoldier::KeyInput(void)
{
	// 	if (m_pInputDev->GetDIKeyState(DIK_1) & 0x80)
	// 	{
	// 		dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_fAniPlayTimer = 0.f;
	// 		m_dwAniIdx = 0;
	// 	}
	// 	if (m_dwAniIdx == 0 && m_pInputDev->GetDIKeyState(DIK_1) & 0x80)
	// 	{
	// 		dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd = false;
	// 		dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_fAniPlayTimer = 0.f;
	// 		m_dwAniIdx = 1;
	// 	}
	if (m_pInputDev->GetDIKeyStateOnce(DIK_RETURN))
	{
		++m_dwAniIdx;
		if (m_dwAniIdx >= 8)
			m_dwAniIdx = 0;
		dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd = false;
		dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_fAniPlayTimer = 0.f;
	}
	//if (m_dwAniIdx != 0 && dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd)
	//{
	//	m_dwAniIdx = 0;
	//}

	/*if (m_pInputDev->GetDIKeyStateOnce(DIK_1))
	{
	cout << "1처음 눌림" << endl;
	}
	if (m_pInputDev->GetDIKeyStateLeave(DIK_1))
	{
	cout << "1떨어짐" << endl;
	}
	if (m_pInputDev->GetDIKeyState(DIK_1))
	{
	cout << "1눌려있는거" << endl;
	}*/
}

HRESULT COtherSoldier::Prepare_StateMachine(void)
{
	/*NULL_CHECK_RETURN(m_pComStateMachine, E_FAIL);
	CState*	pState = NULL;

	pState = CSoldierIdle::Create(this);
	FAILED_CHECK(m_pComStateMachine->Add_State(pState));

	pState = CSoldierMove::Create(this);
	FAILED_CHECK(m_pComStateMachine->Add_State(pState));

	pState = CSoldierAttack::Create(this);
	FAILED_CHECK(m_pComStateMachine->Add_State(pState));*/

	return S_OK;
}

void COtherSoldier::Operate_StateMAchine(const float& fTimeDelta)
{
	/*DWORD dwState = m_pComStateMachine->Get_State();

	switch (dwState)
	{
	case SOLDIER_IDLE:
		break;
	case SOLDIER_MOVE:
		break;
	case SOLDIER_ATTACK:
		break;
	default:
		break;
	}
	m_pComStateMachine->Update_State(dwState);*/
}

COtherSoldier* COtherSoldier::Create(void)
{
	COtherSoldier* pInstance = new COtherSoldier;
	if (FAILED(pInstance->Initialize()))
		::Safe_Delete(pInstance);

	return pInstance;
}

HRESULT COtherSoldier::AddComponent(void)
{
	CComponent* pComponent = NULL;

	vecAniName = *(CAnimationMgr::GetInstance()->GetAnimaiton(L"Soldier_Animation"));

	//TransForm
	pComponent = m_pInfo = CInfo::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Transform", pComponent));


	//DynamicMesh
	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Mesh_Soldier");
	m_pBuffer = dynamic_cast<CDynamicMesh*>(pComponent);
	NULL_CHECK_RETURN(m_pBuffer, E_FAIL);
	m_mapComponent.insert(map<wstring, CComponent*>::value_type(L"Mesh", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_Soldier");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Texture", pComponent));

	m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS_ANI");
	m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS");;

	//StateMachine
	/*pComponent = m_pComStateMachine = CStateMachine::Create(SOLDIER_END);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"StateMachine", pComponent));*/

	return S_OK;
}

void COtherSoldier::Release(void)
{
	Safe_Delete(m_pBuffer);
	Safe_Delete(m_pInfo);
	Safe_Delete(m_pTexture);
}
