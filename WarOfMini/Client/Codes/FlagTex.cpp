#include "stdafx.h"
#include "FlagTex.h"
#include "Texture.h"

CFlagTex::CFlagTex(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
: CResource(pGraphicDev, pContext)
, m_pVB(nullptr)
, m_pIB(nullptr)
, m_uiVtxCnt(0)
, m_uiIdxCnt(0)
, m_pVertex(nullptr)
{
}

CFlagTex::~CFlagTex(void)
{
}

CFlagTex* CFlagTex::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext
	, const WORD& wCntX, const WORD& wCntZ, const WORD& wItv)
{
	CFlagTex* pFlagTex = new CFlagTex(pGraphicDev, pContext);

	if (FAILED(pFlagTex->Create_Buffer(wCntX, wCntZ, wItv)))
	{
		MSG_BOX(L"FlagTex Create_Buffer Failed");
		Safe_Release(pFlagTex);
	}

	return pFlagTex;
}

CFlagTex* CFlagTex::Clone_Resource(void)
{
	CFlagTex* pFlagTex = new CFlagTex(*this);

	++(*m_dwRefCount);

	return pFlagTex;
}

HRESULT CFlagTex::Create_Buffer(const WORD& wCntX, const WORD& wCntZ, const WORD& wItv)
{
	HRESULT hr = E_FAIL;
	//HANDLE hFile = nullptr;

	//DWORD dwByte;
	XMFLOAT3 vVtxPos;

	//hFile = CreateFile(L"../bin/Data/Data_Ter.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	// Vertex
	m_uiVtxCnt = wCntX * wCntZ;
	m_pVertex = new VTXTEX[m_uiVtxCnt];

	int	iIndex = 0;

	for (int z = 0; z < wCntZ; ++z)
	{
		for (int x = 0; x < wCntX; ++x)
		{
			iIndex = z * wCntX + x;

			/*ReadFile(hFile, &vVtxPos, sizeof(XMFLOAT3), &dwByte, NULL);

			if (dwByte == 0)
				break;*/

			m_pVertex[iIndex].vPos = XMFLOAT3(
				float(x) * wItv,
				0.0f,
				float(z) * wItv);;
			m_pVertex[iIndex].vTexUV = XMFLOAT2(x / (wCntX - 1.f), z / (wCntZ - 1.f));
			m_pVertex[iIndex].vNormal = XMFLOAT3(0.f, 0.f, 0.f);
		}
	}

//	CloseHandle(hFile);

	// Index
	m_uiIdxCnt = (wCntX - 1) * (wCntZ - 1) * 2 * 3;
	UINT* pIndex = new UINT[m_uiIdxCnt];

	iIndex = 0;
	int iIBNum = 0;

	XMVECTOR vDest, vSour, vNormal;

	for (int z = 0; z < wCntZ - 1; ++z)
	{
		for (int x = 0; x < wCntX - 1; ++x)
		{
			iIBNum = z * wCntX + x;

			pIndex[iIndex] = iIBNum + wCntX;
			++iIndex;
			pIndex[iIndex] = iIBNum + wCntX + 1;
			++iIndex;
			pIndex[iIndex] = iIBNum + 1;
			++iIndex;

			vDest = XMLoadFloat3(&m_pVertex[pIndex[iIndex - 2]].vPos) - XMLoadFloat3(&m_pVertex[pIndex[iIndex - 3]].vPos);
			vSour = XMLoadFloat3(&m_pVertex[pIndex[iIndex - 1]].vPos) - XMLoadFloat3(&m_pVertex[pIndex[iIndex - 2]].vPos);
			vNormal = XMVector3Cross(vDest, vSour);

			XMStoreFloat3(&m_pVertex[pIndex[iIndex - 3]].vNormal, XMLoadFloat3(&m_pVertex[pIndex[iIndex - 3]].vNormal) + vNormal);
			XMStoreFloat3(&m_pVertex[pIndex[iIndex - 2]].vNormal, XMLoadFloat3(&m_pVertex[pIndex[iIndex - 2]].vNormal) + vNormal);
			XMStoreFloat3(&m_pVertex[pIndex[iIndex - 1]].vNormal, XMLoadFloat3(&m_pVertex[pIndex[iIndex - 1]].vNormal) + vNormal);

			pIndex[iIndex] = iIBNum + wCntX;
			++iIndex;
			pIndex[iIndex] = iIBNum + 1;
			++iIndex;
			pIndex[iIndex] = iIBNum;
			++iIndex;

			vDest = XMLoadFloat3(&m_pVertex[pIndex[iIndex - 2]].vPos) - XMLoadFloat3(&m_pVertex[pIndex[iIndex - 3]].vPos);
			vSour = XMLoadFloat3(&m_pVertex[pIndex[iIndex - 1]].vPos) - XMLoadFloat3(&m_pVertex[pIndex[iIndex - 2]].vPos);
			vNormal = XMVector3Cross(vDest, vSour);

			XMStoreFloat3(&m_pVertex[pIndex[iIndex - 3]].vNormal, XMLoadFloat3(&m_pVertex[pIndex[iIndex - 3]].vNormal) + vNormal);
			XMStoreFloat3(&m_pVertex[pIndex[iIndex - 2]].vNormal, XMLoadFloat3(&m_pVertex[pIndex[iIndex - 2]].vNormal) + vNormal);
			XMStoreFloat3(&m_pVertex[pIndex[iIndex - 1]].vNormal, XMLoadFloat3(&m_pVertex[pIndex[iIndex - 1]].vNormal) + vNormal);
		}
	}

	for (UINT uiIndex = 0; uiIndex < m_uiVtxCnt; ++uiIndex)
		XMStoreFloat3(&m_pVertex[uiIndex].vNormal, XMVector3Normalize(XMLoadFloat3(&m_pVertex[uiIndex].vNormal)));

	// Create Vertex Buffer
	D3D11_BUFFER_DESC tBufferDesc;

	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));

	tBufferDesc.ByteWidth = sizeof(VTXTEX) * m_uiVtxCnt;
	tBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA tSubData;

	ZeroMemory(&tSubData, sizeof(D3D11_SUBRESOURCE_DATA));

	tSubData.pSysMem = m_pVertex;

	hr = m_pGraphicDev->CreateBuffer(&tBufferDesc, &tSubData, &m_pVB);

	if (FAILED(hr) == TRUE)
	{
		MSG_BOX(L"RcTex VB CreateBuffer Failed");
		return E_FAIL;
	}

	// Create Index Buffer
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));

	tBufferDesc.ByteWidth = sizeof(UINT) * m_uiIdxCnt;
	tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	tSubData.pSysMem = pIndex;

	hr = m_pGraphicDev->CreateBuffer(&tBufferDesc, &tSubData, &m_pIB);

	if (FAILED(hr) == TRUE)
	{
		MSG_BOX(L"RcTex IB CreateBuffer Failed");
		return E_FAIL;
	}

	Safe_Delete_Array(pIndex);

	return S_OK;
}

void CFlagTex::Render(void)
{
	UINT uiStride = sizeof(VTXTEX);
	UINT uiOffset = 0;

	m_pContext->IASetVertexBuffers(0, 1, &m_pVB, &uiStride, &uiOffset);
	m_pContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

	m_pContext->DrawIndexed(m_uiIdxCnt, 0, 0);
}

void CFlagTex::Release(void)
{
	CResource::Release();

	if (m_dwRefCount == NULL)
	{
		Safe_Delete_Array(m_pVertex);
		Safe_Release(m_pVB);
		Safe_Release(m_pIB);
	}

	delete this;
}