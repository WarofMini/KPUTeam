#include "stdafx.h"
#include "StaticMesh.h"
#include "Texture.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "SphereMesh.h"

CStaticMesh::CStaticMesh(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
: CMesh(pGraphicDev, pContext)
{
}

CStaticMesh::~CStaticMesh(void)
{
}

CStaticMesh* CStaticMesh::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, const VTXTEX* pVB, const _uint& uiVtxCnt
	, const _uint* pIB, const _uint& uiIdxCnt, const XMFLOAT3& vMin, const XMFLOAT3& vMax, _tchar* pTexName)
{
	CStaticMesh* pMesh = new CStaticMesh(pGraphicDev, pContext);

	if (FAILED(pMesh->Create_Buffer(pVB, uiVtxCnt, pIB, uiIdxCnt, vMin, vMax, pTexName)))
	{
		MSG_BOX(L"CStaticMesh Create_Buffer Failed");
		Safe_Release(pMesh);
	}

	return pMesh;
}

HRESULT CStaticMesh::Create_Buffer(const VTXTEX* pVB, const _uint& uiVtxCnt, const _uint* pIB, const _uint& uiIdxCnt
	, const XMFLOAT3& vMin, const XMFLOAT3& vMax, _tchar* pTexName)
{
	m_vMin = vMin;
	m_vMax = vMax;

	m_uiVtxCnt = uiVtxCnt;
	m_uiIdxCnt = uiIdxCnt;


	HRESULT hr = E_FAIL;

	if (m_uiVtxCnt)
	{

		m_pVtxTex =  new VTXTEX[m_uiVtxCnt];
		ZeroMemory(m_pVtxTex, sizeof(VTXTEX) * m_uiVtxCnt);

		memcpy(m_pVtxTex, pVB, sizeof(VTXTEX) * m_uiVtxCnt);


		D3D11_BUFFER_DESC tBufferDesc;

		ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));

		tBufferDesc.ByteWidth = sizeof(VTXTEX) * m_uiVtxCnt;
		tBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA tSubData;

		ZeroMemory(&tSubData, sizeof(D3D11_SUBRESOURCE_DATA));

		tSubData.pSysMem = pVB;

		hr = m_pGraphicDev->CreateBuffer(&tBufferDesc, &tSubData, &m_pVB);

		if (FAILED(hr) == TRUE)
		{
			MSG_BOX(L"CMesh VB CreateBuffer Failed");
			return E_FAIL;
		}


		//tBufferDesc.ByteWidth = sizeof(_uint) * m_uiIdxCnt;
		//tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		//tSubData.pSysMem = pIB;

		//hr = m_pGraphicDev->CreateBuffer(&tBufferDesc, &tSubData, &m_pIB);

		//if (FAILED(hr) == TRUE)
		//{
		//	MSG_BOX(L"CMesh IB CreateBuffer Failed");
		//	return E_FAIL;
		//}

		wstring wstrTexName = L"Texture_";

		wstrTexName += CompareTexture(pTexName);


		if (wstrTexName != L"Texture_")
			m_pTexture = dynamic_cast<CTextures*>(CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, wstrTexName.c_str()));
	}

	hr = Set_BoundingBox();

	if (FAILED(hr) == TRUE)
	{
		MSG_BOX(L"CMesh Set_BoundingBox Failed");
		return E_FAIL;
	}

	return S_OK;
}

CResource* CStaticMesh::Clone_Resource(void)
{
	CStaticMesh* pMesh = new CStaticMesh(*this);

	++(*m_dwRefCount);

	return pMesh;
}

void CStaticMesh::Render(_uint uiTextureNumber, _bool bColliderDraw)
{
	
		m_iTextureNumber = uiTextureNumber;

		_uint uiStride = sizeof(VTXTEX);
		_uint uiOffset = 0;

		if (m_uiVtxCnt != 0)
		{
			
			// Texture
			if (m_pTexture) 
				m_pTexture->Render(0, m_iTextureNumber);

			// Mesh
			m_pContext->IASetVertexBuffers(0, 1, &m_pVB, &uiStride, &uiOffset);
			//m_pContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

			//m_pContext->DrawIndexed(m_uiIdxCnt, 0, 0);

			m_pContext->Draw(m_uiVtxCnt, 0);
		}

		//bColliderDraw가 FALSE이면 바운딩박스를 Render하지 않는다.
		if (bColliderDraw == TRUE)
		{
			bColliderDraw = g_bCollisionDraw;
		}

		// Bounding Box
		if (bColliderDraw == TRUE)
		{

			CGraphicDev::GetInstance()->SetWireFrame(TRUE);

			m_pContext->IASetVertexBuffers(0, 1, &m_pBBoxVB, &uiStride, &uiOffset);
			m_pContext->IASetIndexBuffer(m_pBBoxIB, DXGI_FORMAT_R32_UINT, 0);

			m_pContext->DrawIndexed(36, 0, 0);
	
			CGraphicDev::GetInstance()->SetWireFrame(FALSE);

		}


	for (_uint uiSize = 0; uiSize < m_vecChild.size(); ++uiSize)
		m_vecChild[uiSize]->Render(uiTextureNumber, bColliderDraw);
}

void CStaticMesh::RenderInst(const vector<XMFLOAT4X4*>& vecObjWorld)
{
	_uint uiStride = sizeof(VTXTEX);
	_uint uiOffset = 0;

	if (m_uiVtxCnt != 0)
	{
		m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Instancing"));

		ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
		ID3D11Buffer* pInstShaderCB = CGraphicDev::GetInstance()->GetInstShaderCB();

		ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

		// Shader Set
		m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Instancing"), NULL, 0);
		m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
		m_pContext->VSSetConstantBuffers(1, 1, &pInstShaderCB);
		m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Instancing"), NULL, 0);
		m_pContext->PSSetSamplers(0, 1, &pBaseSampler);

		//Texture
		if (m_pTexture) 
			m_pTexture->Render(0, m_iTextureNumber);

		// Mesh
		m_pContext->IASetVertexBuffers(0, 1, &m_pVB, &uiStride, &uiOffset);
		m_pContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

		// Base
		BASESHADER_CB tBaseShaderCB;

		tBaseShaderCB.matWorld = XMMatrixTranspose(XMMatrixIdentity());
		tBaseShaderCB.matView = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView()));
		tBaseShaderCB.matProj = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraProj()));
		tBaseShaderCB.vLightPos = XMLoadFloat3(&g_vLightPos) - XMLoadFloat3(&g_vPlayerPos);

		XMMATRIX matView = XMMatrixLookAtLH(XMLoadFloat3(&g_vPlayerPos) - XMLoadFloat3(&g_vLightPos)
			, XMLoadFloat3(&g_vPlayerPos), XMVectorSet(0.f, 1.f, 0.f, 0.f));

		tBaseShaderCB.matLightView[0] = XMMatrixTranspose(matView);

		m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);

		_uint uiCnt = _uint(vecObjWorld.size() / INSTCNT) + 1;

		// Instancing
		for (_uint uiDrawCnt = 0; uiDrawCnt < _uint(vecObjWorld.size() / INSTCNT) + 1; ++uiDrawCnt)
		{
			INSTSHADER_CB tInstShaderCB;
			_uint uiInstSize = vecObjWorld.size() - (uiDrawCnt * INSTCNT);

			if (uiInstSize > INSTCNT)
				uiInstSize = INSTCNT;

			for (_uint uiIndex = 0; uiIndex < uiInstSize; ++uiIndex)
				tInstShaderCB.matWorld[uiIndex] = XMMatrixTranspose(XMLoadFloat4x4(vecObjWorld[uiIndex + (uiDrawCnt * INSTCNT)]));

			m_pContext->UpdateSubresource(pInstShaderCB, 0, NULL, &tInstShaderCB, 0, 0);

			// Draw
			m_pContext->DrawIndexedInstanced(m_uiIdxCnt, uiInstSize, 0, 0, 0);

		}
	}

	for (_uint uiSize = 0; uiSize < m_vecChild.size(); ++uiSize)
		m_vecChild[uiSize]->RenderInst(vecObjWorld);
}

void CStaticMesh::Release(void)
{
	CMesh::Release();

	delete this;
}

HRESULT CStaticMesh::Set_BoundingBox(void)
{

	// Vertex
	VTXTEX pVtxTex[] =
	{
		// Pos										TexUV				Normal
		{ XMFLOAT3(m_vMin.x, m_vMax.y, m_vMin.z), XMFLOAT2(0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f) },
		{ XMFLOAT3(m_vMax.x, m_vMax.y, m_vMin.z), XMFLOAT2(0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f) },
		{ XMFLOAT3(m_vMax.x, m_vMin.y, m_vMin.z), XMFLOAT2(0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f) },
		{ XMFLOAT3(m_vMin.x, m_vMin.y, m_vMin.z), XMFLOAT2(0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f) },
		{ XMFLOAT3(m_vMin.x, m_vMax.y, m_vMax.z), XMFLOAT2(0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f) },
		{ XMFLOAT3(m_vMax.x, m_vMax.y, m_vMax.z), XMFLOAT2(0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f) },
		{ XMFLOAT3(m_vMax.x, m_vMin.y, m_vMax.z), XMFLOAT2(0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f) },
		{ XMFLOAT3(m_vMin.x, m_vMin.y, m_vMax.z), XMFLOAT2(0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f) }
	};

	_uint uiVtxCnt = ARRAYSIZE(pVtxTex);

	D3D11_BUFFER_DESC tBufferDesc;

	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));

	tBufferDesc.ByteWidth = sizeof(VTXTEX) * uiVtxCnt;
	tBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA tSubData;

	ZeroMemory(&tSubData, sizeof(D3D11_SUBRESOURCE_DATA));

	tSubData.pSysMem = pVtxTex;

	HRESULT hr = m_pGraphicDev->CreateBuffer(&tBufferDesc, &tSubData, &m_pBBoxVB);

	if (FAILED(hr) == TRUE)
	{
		MSG_BOX(L"Bouding Box VB CreateBuffer Failed");
		return E_FAIL;
	}

	// Index
	_uint pIndex[] =
	{
		//+x
		1, 6, 2,
		1, 5, 6,

		//-x
		4, 0, 3,
		4, 3, 7,

		//+y
		4, 5, 1,
		4, 1, 0,

		//-y
		3, 2, 6,
		3, 6, 7,

		//+z
		7, 6, 5,
		7, 5, 4,

		//-z
		0, 1, 2,
		0, 2, 3
	};

	_uint uiIdxCnt = ARRAYSIZE(pIndex);

	tBufferDesc.ByteWidth = sizeof(_uint) * uiIdxCnt;
	tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	tSubData.pSysMem = pIndex;

	hr = m_pGraphicDev->CreateBuffer(&tBufferDesc, &tSubData, &m_pBBoxIB);

	if (FAILED(hr) == TRUE)
	{
		MSG_BOX(L"Bounding Box IB CreateBuffer Failed");
		return E_FAIL;
	}

	return S_OK;
}
