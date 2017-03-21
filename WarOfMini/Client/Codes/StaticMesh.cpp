#include "stdafx.h"
#include "StaticMesh.h"
#include "Texture.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "CameraMgr.h"

CStaticMesh::CStaticMesh(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
: CMesh(pGraphicDev, pContext)
{
}

CStaticMesh::~CStaticMesh(void)
{
}

CStaticMesh* CStaticMesh::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, const VTXTEX* pVB, const _uint& uiVtxCnt
	, const _uint* pIB, const _uint& uiIdxCnt, const _vec3& vMin, const _vec3& vMax, _tchar* pTexName)
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
	, const _vec3& vMin, const _vec3& vMax, _tchar* pTexName)
{
	m_vMin = vMin;
	m_vMax = vMax;

	m_uiVtxCnt = uiVtxCnt;
	m_uiIdxCnt = uiIdxCnt;

	HRESULT hr = E_FAIL;

	if (m_uiVtxCnt && m_uiIdxCnt)
	{
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

		tBufferDesc.ByteWidth = sizeof(_uint) * m_uiIdxCnt;
		tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		tSubData.pSysMem = pIB;

		hr = m_pGraphicDev->CreateBuffer(&tBufferDesc, &tSubData, &m_pIB);

		if (FAILED(hr) == TRUE)
		{
			MSG_BOX(L"CMesh IB CreateBuffer Failed");
			return E_FAIL;
		}

		wstring wstrTexName = L"Texture_";
		wstrTexName += pTexName;

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

void CStaticMesh::Render(_bool bColliderDraw)
{
	{
		_uint uiStride = sizeof(VTXTEX);
		_uint uiOffset = 0;

		if (m_uiVtxCnt != 0)
		{
			// Texture
			if (m_pTexture) 
				m_pTexture->Render(0, 0);

			// Mesh
			m_pContext->IASetVertexBuffers(0, 1, &m_pVB, &uiStride, &uiOffset);
			m_pContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

			m_pContext->DrawIndexed(m_uiIdxCnt, 0, 0);
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
	}

	for (_uint uiSize = 0; uiSize < m_vecChild.size(); ++uiSize)
		m_vecChild[uiSize]->Render(bColliderDraw);
}

void CStaticMesh::RenderInst(const vector<_matrix*>& vecObjWorld)
{
	_uint uiStride = sizeof(VTXTEX);
	_uint uiOffset = 0;

	if (m_uiVtxCnt != 0)
	{
		m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Instancing") );


		ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
		ID3D11Buffer* pInstShaderCB = CGraphicDev::GetInstance()->GetInstShaderCB();

		ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

		// Shader Set
		m_pContext->VSSetShader( CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Instancing"), NULL, 0);
		m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
		m_pContext->VSSetConstantBuffers(1, 1, &pInstShaderCB);
		m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Instancing"), NULL, 0);
		m_pContext->PSSetSamplers(0, 1, &pBaseSampler);

		//Texture
		if (m_pTexture) 
			m_pTexture->Render(0, 0);

		// Mesh
		m_pContext->IASetVertexBuffers(0, 1, &m_pVB, &uiStride, &uiOffset);
		m_pContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

		// Base
		BASESHADERCB tBaseShaderCB;

		_matrix matWorld;


		D3DXMatrixIdentity(&matWorld);

		//나중에 카메라 만들면 다시 주석 풀어야함
		tBaseShaderCB.matWorld = matWorld;
		tBaseShaderCB.matView = (*CCameraMgr::GetInstance()->Get_CurCameraView());
		tBaseShaderCB.matProj = (*CCameraMgr::GetInstance()->Get_CurCameraProj());


		m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);

		_uint uiCnt = _uint(vecObjWorld.size() / INSTCNT) + 1;

		// Instancing
		for (_uint uiDrawCnt = 0; uiDrawCnt < _uint(vecObjWorld.size() / INSTCNT) + 1; ++uiDrawCnt)
		{
			INSTANCINGSHADERCB tInstShaderCB;
			
			_uint uiInstSize = vecObjWorld.size() - (uiDrawCnt * INSTCNT);

			if (uiInstSize > INSTCNT)
				uiInstSize = INSTCNT;

			for (_uint uiIndex = 0; uiIndex < uiInstSize; ++uiIndex)
				tInstShaderCB.matWorld[uiIndex] = *(vecObjWorld[uiIndex + (uiDrawCnt * INSTCNT)]); //체크

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

	if (m_dwRefCount == NULL)
	{
	}

	delete this;
}

HRESULT CStaticMesh::Set_BoundingBox(void)
{
	// Vertex
	VTXTEX pVtxTex[] =
	{
		// Pos										TexUV				Normal
		{ _vec3(m_vMin.x, m_vMax.y, m_vMin.z), _vec2(0.f, 0.f), _vec3(0.f, 0.f, 0.f) },
		{ _vec3(m_vMax.x, m_vMax.y, m_vMin.z), _vec2(0.f, 0.f), _vec3(0.f, 0.f, 0.f) },
		{ _vec3(m_vMax.x, m_vMin.y, m_vMin.z), _vec2(0.f, 0.f), _vec3(0.f, 0.f, 0.f) },
		{ _vec3(m_vMin.x, m_vMin.y, m_vMin.z), _vec2(0.f, 0.f), _vec3(0.f, 0.f, 0.f) },
		{ _vec3(m_vMin.x, m_vMax.y, m_vMax.z), _vec2(0.f, 0.f), _vec3(0.f, 0.f, 0.f) },
		{ _vec3(m_vMax.x, m_vMax.y, m_vMax.z), _vec2(0.f, 0.f), _vec3(0.f, 0.f, 0.f) },
		{ _vec3(m_vMax.x, m_vMin.y, m_vMax.z), _vec2(0.f, 0.f), _vec3(0.f, 0.f, 0.f) },
		{ _vec3(m_vMin.x, m_vMin.y, m_vMax.z), _vec2(0.f, 0.f), _vec3(0.f, 0.f, 0.f) }
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
		1, 5, 6,
		1, 6, 2,
		4, 0, 3,
		4, 3, 7,
		4, 5, 1,
		4, 1, 0,
		3, 2, 6,
		3, 6, 7,
		7, 6, 5,
		7, 5, 4,
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