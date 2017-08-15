#include "stdafx.h"
#include "BoostTrail.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "Transform.h"
#include "Layer.h"
#include "Scene.h"
#include "Sound.h"
#include "Player.h"


CBoostTrail::CBoostTrail(ID3D11DeviceContext* pContext)
: CEffect(pContext)
, m_fAlpha(1.f)
, m_fAlphaSpeed(0.f)
, m_pTrailBuffer(nullptr)
, m_pPlayer(NULL)
, m_pTrailVtx(NULL)
, m_bJumpCheck(false)
{

}

CBoostTrail::~CBoostTrail(void)
{
}

CBoostTrail * CBoostTrail::Create(ID3D11DeviceContext * pContext)
{
	CBoostTrail* pEffect = new CBoostTrail(pContext);

	if (FAILED(pEffect->Initialize()))
		Safe_Release(pEffect);

	return pEffect;
}

HRESULT CBoostTrail::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTrailVtx = new VTXTEX[(_int)m_pTrailBuffer->GetSizeX() * (_int)m_pTrailBuffer->GetSizeZ()];

	m_pTrailBuffer->GetVtxInfo(m_pTrailVtx);

	return S_OK;
}

INT CBoostTrail::Update(const FLOAT & fTimeDelta)
{
	if (m_pPlayer == NULL)
		CheckPlayer();

	CEffect::Update(fTimeDelta);



	if (m_pPlayer && (!m_pPlayer->IsSoldier()))
	{
		//처음 점프할때
		if (((*m_pPlayer->Get_State()) == CPlayer::SOLDIER_JUMP) && (!m_bJumpCheck))
		{
			m_bJumpCheck = true;
			InitTrail();
		}
		if (((*m_pPlayer->Get_State()) != CPlayer::SOLDIER_JUMP))
		{
			m_bJumpCheck = false;
			InitTrail();
		}

		if (m_bJumpCheck)
		{
			m_vStart = XMFLOAT3(-2.0f, -2.0f, -8.0f);
			m_vMiddle = XMFLOAT3(3.0f, -1.0f, -8.0f);
			m_vEnd = XMFLOAT3(4.0f, 2.0f, -8.0f);


			XMStoreFloat3(&m_vStart, XMVector3TransformCoord(XMLoadFloat3(&m_vStart), XMLoadFloat4x4(&m_pPlayer->GetTransformMatWorld())));
			XMStoreFloat3(&m_vMiddle, XMVector3TransformCoord(XMLoadFloat3(&m_vMiddle), XMLoadFloat4x4(&m_pPlayer->GetTransformMatWorld())));
			XMStoreFloat3(&m_vEnd, XMVector3TransformCoord(XMLoadFloat3(&m_vEnd), XMLoadFloat4x4(&m_pPlayer->GetTransformMatWorld())));

			for (int i = 0; i < m_pTrailBuffer->GetSizeZ(); ++i)
			{
				XMFLOAT3 Temp, Temp1, Temp2;

				if (i == 0)
				{
					Temp = m_pTrailVtx[0].vPos;
					Temp1 = m_pTrailVtx[1].vPos;
					Temp2 = m_pTrailVtx[(_int)(m_pTrailBuffer->GetSizeX()) - 1].vPos;

					m_pTrailVtx[0].vPos = m_vStart;
					m_pTrailVtx[1].vPos = m_vMiddle;
					m_pTrailVtx[(_int)(m_pTrailBuffer->GetSizeX()) - 1].vPos = m_vEnd;
				}
				else
				{
					XMFLOAT3 vTemp, vTemp1, vTemp2;

					vTemp = m_pTrailVtx[((_int)(m_pTrailBuffer->GetSizeX())) * i].vPos;
					vTemp1 = m_pTrailVtx[(((_int)(m_pTrailBuffer->GetSizeX())) * (i + 1)) - 2].vPos;
					vTemp2 = m_pTrailVtx[(((_int)(m_pTrailBuffer->GetSizeX())) * (i + 1)) - 1].vPos;


					m_pTrailVtx[((_int)(m_pTrailBuffer->GetSizeX())) * i].vPos = Temp;
					m_pTrailVtx[(((_int)(m_pTrailBuffer->GetSizeX())) * (i + 1)) - 2].vPos = Temp1;
					m_pTrailVtx[(((_int)(m_pTrailBuffer->GetSizeX())) * (i + 1)) - 1].vPos = Temp2;

					Temp = vTemp;
					Temp1 = vTemp1;
					Temp2 = vTemp2;
				}
			}

			m_pTrailBuffer->SetVtxInfo(m_pTrailVtx);
		}

		}



	if (!m_bDead)
		CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_EFFECT_ALPHA, this, NULL);

	return m_bDead;
}

void CBoostTrail::Render(void)
{

	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Trail"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();

	ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

	BASESHADER_CB tBaseShaderCB;

	tBaseShaderCB.matWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_pTransform->m_matWorld));
	tBaseShaderCB.matView = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView()));
	tBaseShaderCB.matProj = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraProj()));
	m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);



	m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Trail"), NULL, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
	m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Trail"), NULL, 0);
	m_pContext->PSSetSamplers(0, 1, &pBaseSampler);

	m_pTexture->Render(0, m_iTextureNumber);
	m_pTrailBuffer->Render();

}

void CBoostTrail::Release(void)
{
	CEffect::Release();
}

HRESULT CBoostTrail::Ready_Component(void)
{
	CComponent* pComponent = NULL;

	//Buffer
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Buffer_BoostTrail");
	m_pTrailBuffer = dynamic_cast<CTrailTex*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Texture_Trail");
	m_pTexture = dynamic_cast<CTextures*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));


	// Transform
	pComponent = CTransform::Create();
	m_pTransform = dynamic_cast<CTransform*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));

	return S_OK;
}

void CBoostTrail::CheckPlayer(void)
{
	CLayer* pLayer = CManagement::GetInstance()->GetScene()->FindLayer(L"Layer_GameLogic");
	list<CGameObject*>* pObjList = pLayer->Find_ObjectList(L"Player");

	if (pObjList != NULL)
		m_pPlayer = (CPlayer*)(*pObjList->begin());
}

void CBoostTrail::InitTrail(void)
{
	m_vStart = XMFLOAT3(-2.0f, -2.0f, -8.0f);
	m_vMiddle = XMFLOAT3(3.0f, -1.0f, -8.0f);
	m_vEnd = XMFLOAT3(4.0f, 2.0f, -8.0f);

	XMStoreFloat3(&m_vStart, XMVector3TransformCoord(XMLoadFloat3(&m_vStart), XMLoadFloat4x4(&m_pPlayer->GetTransformMatWorld())));
	XMStoreFloat3(&m_vMiddle, XMVector3TransformCoord(XMLoadFloat3(&m_vMiddle), XMLoadFloat4x4(&m_pPlayer->GetTransformMatWorld())));
	XMStoreFloat3(&m_vEnd, XMVector3TransformCoord(XMLoadFloat3(&m_vEnd), XMLoadFloat4x4(&m_pPlayer->GetTransformMatWorld())));

	for (int i = 0; i < m_pTrailBuffer->GetSizeZ(); ++i)
	{
		XMFLOAT3 Temp, Temp1, Temp2;

		if (i == 0)
		{
			m_pTrailVtx[0].vPos = m_vStart;
			m_pTrailVtx[1].vPos = m_vMiddle;
			m_pTrailVtx[(_int)(m_pTrailBuffer->GetSizeX()) - 1].vPos = m_vEnd;
		}
		else
		{
			m_pTrailVtx[((_int)(m_pTrailBuffer->GetSizeX())) * i].vPos = m_vStart;
			m_pTrailVtx[(((_int)(m_pTrailBuffer->GetSizeX())) * (i + 1)) - 2].vPos = m_vMiddle;
			m_pTrailVtx[(((_int)(m_pTrailBuffer->GetSizeX())) * (i + 1)) - 1].vPos = m_vEnd;

		}
	}

	m_pTrailBuffer->SetVtxInfo(m_pTrailVtx);
}
