#include "stdafx.h"
#include "CubeTex.h"

CCubeTex::CCubeTex(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
: CResource(pGraphicDev, pContext)
, m_pVB(NULL)
, m_pIB(NULL)
{
}

CCubeTex::~CCubeTex(void)
{
}

CCubeTex* CCubeTex::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
{
	CCubeTex* pCubeTex = new CCubeTex(pGraphicDev, pContext);

	if (FAILED(pCubeTex->Create_Buffer()))
	{
		MSG_BOX(L"pCubeTex Create_Buffer Failed");
		Safe_Release(pCubeTex);
	}

	return pCubeTex;
}

CCubeTex* CCubeTex::Clone_Resource(void)
{
	CCubeTex* pCubeTex = new CCubeTex(*this);

	++(*m_dwRefCount);

	return pCubeTex;
}

HRESULT CCubeTex::Create_Buffer(void)
{
	HRESULT hr = E_FAIL;

	// Vertex
	VTXTEX pVtxTex[] =
	{
		// Pos							TexUV					Normal
		{ _vec3(-1.f, 1.f, -1.f),		_vec2(0.25f, 0.25f),	_vec3(0.f, 0.f, 0.f) },			// Back, Left, Front, Right
		{ _vec3(1.f, 1.f, -1.f),		_vec2(0.f, 0.25f),		_vec3(0.f, 0.f, 0.f) },
		{ _vec3(1.f, -1.f, -1.f),		_vec2(0.f, 0.5f),		_vec3(0.f, 0.f, 0.f) },
		{ _vec3(-1.f, -1.f, -1.f),		_vec2(0.25f, 0.5f),		_vec3(0.f, 0.f, 0.f) },
		{ _vec3(-1.f, 1.f, 1.f),		_vec2(0.5f,	0.25f),		_vec3(0.f, 0.f, 0.f) },
		{ _vec3(1.f, 1.f, 1.f),			_vec2(0.75f, 0.25f),	_vec3(0.f, 0.f, 0.f) },
		{ _vec3(1.f, -1.f, 1.f),		_vec2(0.75f, 0.5f),		_vec3(0.f, 0.f, 0.f) },
		{ _vec3(-1.f, -1.f, 1.f),		_vec2(0.5f,	0.5f),		_vec3(0.f, 0.f, 0.f) },
		{ _vec3(-1.f, 1.f, -1.f),		_vec2(0.5f, 0.f),		_vec3(0.f, 0.f, 0.f) },				// Top, Right, Bottom
		{ _vec3(1.f, 1.f, -1.f),		_vec2(0.75f, 0.f),		_vec3(0.f, 0.f, 0.f) },
		{ _vec3(1.f, 1.f, -1.f),		_vec2(1.f, 0.25f),		_vec3(0.f, 0.f, 0.f) },
		{ _vec3(1.f, -1.f, -1.f),		_vec2(1.f, 0.5f),		_vec3(0.f, 0.f, 0.f) },
		{ _vec3(1.f, -1.f, -1.f),		_vec2(0.75f, 0.75f),	_vec3(0.f, 0.f, 0.f) },
		{ _vec3(-1.f, -1.f, -1.f),		_vec2(0.5f, 0.75f),		_vec3(0.f, 0.f, 0.f) }
	};

	_uint uiVtxCnt = ARRAYSIZE(pVtxTex);

	D3D11_BUFFER_DESC tBufferDesc;

	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));

	tBufferDesc.ByteWidth = sizeof(VTXTEX) * uiVtxCnt;
	tBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA tSubData;

	ZeroMemory(&tSubData, sizeof(D3D11_SUBRESOURCE_DATA));

	tSubData.pSysMem = pVtxTex;

	hr = m_pGraphicDev->CreateBuffer(&tBufferDesc, &tSubData, &m_pVB);

	if (FAILED(hr) == TRUE)
	{
		MSG_BOX(L"RcTex VB CreateBuffer Failed");
		return E_FAIL;
	}

	// Index
	_uint pIndex[] =
	{
		6, 5, 10,
		11, 6, 10,
		3, 0, 4,
		7, 3, 4,
		9, 5, 4,
		8, 9, 4,
		6, 12, 13,
		7, 6, 13,
		5, 6, 7,
		4, 5, 7,
		2, 1, 0,
		3, 2, 0
	};

	_uint uiIdxCnt = ARRAYSIZE(pIndex);

	tBufferDesc.ByteWidth = sizeof(_uint) * uiIdxCnt;
	tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	tSubData.pSysMem = pIndex;

	hr = m_pGraphicDev->CreateBuffer(&tBufferDesc, &tSubData, &m_pIB);

	if (FAILED(hr) == TRUE)
	{
		MSG_BOX(L"RcTex IB CreateBuffer Failed");
		return E_FAIL;
	}

	return S_OK;
}

void CCubeTex::Render(void)
{
	_uint uiStride = sizeof(VTXTEX);
	_uint uiOffset = 0;

	m_pContext->IASetVertexBuffers(0, 1, &m_pVB, &uiStride, &uiOffset);
	m_pContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

	m_pContext->DrawIndexed(36, 0, 0);
}

void CCubeTex::Release(void)
{
	CResource::Release();

	if (m_dwRefCount == NULL)
	{
		Safe_Release(m_pVB);
		Safe_Release(m_pIB);
	}

	delete this;
}