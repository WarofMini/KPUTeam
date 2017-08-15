#include "stdafx.h"
#include "Particle.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "Transform.h"
#include "Sound.h"


CParticle::CParticle(ID3D11DeviceContext * pContext)
:CEffect(pContext)
, m_fAlpha(1.0f)
, m_fAlphaSpeed(0.8f)
, m_fParticleTime(0.0f)
{
	m_bAllBillboardCheck = true;
}

CParticle::~CParticle(void)
{
}

CParticle* CParticle::Create(ID3D11DeviceContext * pContext)
{
	CParticle* pParticle = new CParticle(pContext);

	if (FAILED(pParticle->Initialize()))
		Safe_Release(pParticle);

	return pParticle;
}

HRESULT CParticle::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	InitParticle();

	return S_OK;
}

INT CParticle::Update(const FLOAT & fTimeDelta)
{
	m_fAlpha = max((_float)(m_fAlpha - (fTimeDelta * m_fAlphaSpeed)), 0.0f);

	m_fParticleTime = fTimeDelta * 100.f;

	// 공기저항을 구한다.
	XMFLOAT3	vcAirR = m_vCrnV;					// 공기저항의 방향 벡터
	XMFLOAT3	vcTemp;

	XMStoreFloat3(&vcTemp, XMVector3LengthSq(XMLoadFloat3(&vcAirR)));

	// 속도의 제곱(Vx*Vx + Vy*Vy + Vz*Vz)크기 구함
	_float		fLenV = vcTemp.x;
	
	// 공기저항의 방향 벡터를 구한다.									
	XMStoreFloat3(&vcAirR, XMVector3Normalize(XMLoadFloat3(&vcAirR)));

	//이동 속도와 반대로 설정
	vcAirR = XMFLOAT3(vcAirR.x * (-1.f), vcAirR.y * (-1.f), vcAirR.z * (-1.f));

	// 속력제곱 * 공기 저항 계수를 곱함.
	vcAirR = XMFLOAT3(vcAirR.x * (fLenV * m_fDamp), vcAirR.y * (fLenV * m_fDamp), vcAirR.z * (fLenV * m_fDamp));

	// 1. 가속도에 공기저항을 더한다.
	m_vCrnA = XMFLOAT3(m_vIntA.x + vcAirR.x, m_vIntA.y + vcAirR.y, m_vIntA.z + vcAirR.z);

	// 2. 현재 속도 갱신
	m_vCrnV = XMFLOAT3(m_vCrnV.x + (m_vCrnA.x * m_fParticleTime), m_vCrnV.y + (m_vCrnA.y * m_fParticleTime), m_vCrnV.z + (m_vCrnA.z * m_fParticleTime));

	// 3. 현재 위치 갱신
	m_vCrnP = XMFLOAT3(m_vCrnP.x + (m_vCrnV.x * m_fParticleTime), m_vCrnP.y + m_vCrnV.y * m_fParticleTime, m_vCrnP.z + m_vCrnV.z * m_fParticleTime);


	m_pTransform->m_vPos = m_vCrnP;

	CEffect::Update(fTimeDelta);


	XMMATRIX matView = XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView());
	XMFLOAT4X4 matViewWorld;

	XMStoreFloat4x4(&matViewWorld, XMLoadFloat4x4(&m_pTransform->m_matWorld) * matView);

	ComputeBillboard();

	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_EFFECT_ALPHA, this, matViewWorld._43);

	return 0;
}

void CParticle::Render(void)
{
	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_GunFlash"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11Buffer* pAlphaShaderCB = CGraphicDev::GetInstance()->GetAlphaShaderCB();

	ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

	BASESHADER_CB tBaseShaderCB;

	tBaseShaderCB.matWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_pTransform->m_matWorld));
	tBaseShaderCB.matView = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView()));
	tBaseShaderCB.matProj = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraProj()));
	m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);

	ALPHA_CB tAlphaShaderCB;
	tAlphaShaderCB.fAlpha = m_fAlpha;
	m_pContext->UpdateSubresource(pAlphaShaderCB, 0, NULL, &tAlphaShaderCB, 0, 0);


	m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_GunFlash"), NULL, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
	m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_GunFlash"), NULL, 0);
	m_pContext->PSSetConstantBuffers(1, 1, &pAlphaShaderCB);
	m_pContext->PSSetSamplers(0, 1, &pBaseSampler);

	m_pTexture->Render(0, m_iTextureNumber);
	m_pBuffer->Render();

}

void CParticle::Release(void)
{
	CEffect::Release();
}

HRESULT CParticle::Ready_Component(void)
{
	CComponent* pComponent = NULL;

	//Buffer
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Buffer_RcTex");
	m_pBuffer = dynamic_cast<CRcTex*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Texture_Particle");
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

void CParticle::InitParticle(void)
{
	_float		fTheta; // 각도 θ
	_float		fPhi;	// 각도 φ
	_float		fSpdR;	// 속도 크기

						//초기 가속도
	m_vIntA = XMFLOAT3(0, -0.2f, 0);

	//초기 속도와 위치를 설정하기 위한 변수
	fTheta = _float(rand() % 61);
	fTheta -= 30.f;

	fPhi = _float(rand() % 360);

	fSpdR = 100.f + rand() % 101;
	fSpdR *= 0.1f;

	// 라디안으로 변경
	fTheta = XMConvertToRadians(fTheta);
	fPhi = XMConvertToRadians(fPhi);

	// 초기 속도
	m_vIntV.x = fSpdR * sinf(fTheta) * sinf(fPhi);
	m_vIntV.y = fSpdR * cosf(fTheta);
	m_vIntV.z = fSpdR * sinf(fTheta) * cosf(fPhi);

	// 초기 위치		
	m_vIntP.x = 0.f;
	m_vIntP.y = 0.f;
	m_vIntP.z = 0.f;

	// 탄성 계수 설정
	m_fElst = (50.f + rand() % 51)*0.01f;

	// 공기저항 계수
	m_fDamp = (100.f + rand() % 101)*0.0001f;

	// 초기 위치, 속도, 가속도를 현재의 값들의 초기 값으로 설정
	m_vCrnP = m_vIntP;
	m_vCrnV = m_vIntV;
	m_vCrnA = m_vIntA;
}
