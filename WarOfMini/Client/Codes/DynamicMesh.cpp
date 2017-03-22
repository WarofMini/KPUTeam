#include "stdafx.h"
#include "DynamicMesh.h"
#include "Texture.h"
#include "Animation.h"
#include "AnimationInfo.h"
#include "ResourcesMgr.h"
#include "GraphicDev.h"

CDynaicMesh::CDynaicMesh(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
: CMesh(pGraphicDev, pContext)
, m_pOriVertex(nullptr)
, m_pVertex(nullptr)
, m_pIndex(nullptr)
, m_pAnimation(nullptr)
{
	ZeroMemory(&m_vPivotPos, sizeof(XMFLOAT3));
}

CDynaicMesh::~CDynaicMesh(void)
{
}

CDynaicMesh* CDynaicMesh::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, const VTXBONE* pVB, const _uint& uiVtxCnt
	, const _uint* pIB, const _uint& uiIdxCnt, const XMFLOAT3& vMin, const XMFLOAT3& vMax, _tchar* pTexName, CAnimation* pAnimation)
{
	CDynaicMesh* pMesh = new CDynaicMesh(pGraphicDev, pContext);

	if (FAILED(pMesh->Create_Buffer(pVB, uiVtxCnt, pIB, uiIdxCnt, vMin, vMax, pTexName, pAnimation)))
	{
		MSG_BOX(L"CDynaicMesh Create_Buffer Failed");
		Safe_Release(pMesh);
	}

	return pMesh;
}

HRESULT CDynaicMesh::Create_Buffer(const VTXBONE* pVB, const _uint& uiVtxCnt, const _uint* pIB, const _uint& uiIdxCnt
	, const XMFLOAT3& vMin, const XMFLOAT3& vMax, _tchar* pTexName, CAnimation* pAnimation)
{
	m_pAnimation = pAnimation;

	m_vMin = vMin;
	m_vMax = vMax;

	m_uiVtxCnt = uiVtxCnt;
	m_uiIdxCnt = uiIdxCnt;

	HRESULT hr = E_FAIL;

	if (m_uiVtxCnt && m_uiIdxCnt)
	{
		m_pOriVertex = new VTXBONE[m_uiVtxCnt];
		memcpy(m_pOriVertex, pVB, sizeof(VTXBONE) * m_uiVtxCnt);

		m_pVertex = new VTXBONE[m_uiVtxCnt];
		memcpy(m_pVertex, pVB, sizeof(VTXBONE) * m_uiVtxCnt);

		m_pIndex = new UINT[m_uiIdxCnt];
		memcpy(m_pIndex, pIB, sizeof(UINT) * m_uiIdxCnt);

		m_vPivotPos = pVB[0].vPos;

		D3D11_BUFFER_DESC tBufferDesc;

		ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));

		tBufferDesc.ByteWidth = sizeof(VTXBONE) * m_uiVtxCnt;
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

CResource* CDynaicMesh::Clone_Resource(void)
{
	CDynaicMesh* pMesh = new CDynaicMesh(*this);

	++(*m_dwRefCount);

	return pMesh;
}

void CDynaicMesh::Render(_bool bColliderDraw)
{
	{
		_uint uiStride = sizeof(VTXBONE);
		_uint uiOffset = 0;

		if (m_uiVtxCnt != 0)
		{
			// Texture
			if (m_pTexture) m_pTexture->Render(0, 0);

			// Animation
			if (m_pAnimation)
				m_pAnimation->UpdateSubresource(nullptr);

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

void CDynaicMesh::RenderAnim(CAnimationInfo* pAnimInfo, MATNODE* pMatNode, _ubyte byColor /*= 0*/, _bool bColliderDraw /*= FALSE*/)
{
	{
		_uint uiStride = sizeof(VTXBONE);
		_uint uiOffset = 0;

		if (m_uiVtxCnt != 0)
		{
			// Texture
			if (m_pTexture) m_pTexture->Render(0, byColor);

			// Animation
			if (m_pAnimation)
				m_pAnimation->UpdateSubresource(pAnimInfo, pMatNode->matBone);

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

	for (_uint uiIndex = 0; uiIndex < m_vecChild.size(); ++uiIndex)
		dynamic_cast<CDynaicMesh*>(m_vecChild[uiIndex])->RenderAnim(pAnimInfo, pMatNode->vecNode[uiIndex], byColor, bColliderDraw);
}

void CDynaicMesh::RenderInst(const vector<XMFLOAT4X4*>& vecObjWorld)
{
	/*
	UINT uiStride = sizeof(VTXTEX);
	UINT uiOffset = 0;

	if (m_uiVtxCnt != 0)
	{
	m_pContext->IASetInputLayout(Get_InputLayout(L"Shader_Instancing"));

	ID3D11Buffer* pBaseShaderCB = Get_BaseShaderCB();
	ID3D11Buffer* pInstShaderCB = Get_InstShaderCB();

	ID3D11SamplerState* pBaseSampler = Get_BaseSampler();

	// Shader Set
	m_pContext->VSSetShader(Get_VertexShader(L"Shader_Instancing"), NULL, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
	m_pContext->VSSetConstantBuffers(1, 1, &pInstShaderCB);
	m_pContext->PSSetShader(Get_PixelShader(L"Shader_Instancing"), NULL, 0);
	m_pContext->PSSetSamplers(0, 1, &pBaseSampler);

	//Texture
	if (m_pTexture) m_pTexture->Render(0, 0);

	// Mesh
	m_pContext->IASetVertexBuffers(0, 1, &m_pVB, &uiStride, &uiOffset);
	m_pContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

	// Base
	BASESHADER_CB tBaseShaderCB;

	tBaseShaderCB.matWorld = XMMatrixTranspose(XMMatrixIdentity());
	tBaseShaderCB.matView = XMMatrixTranspose(XMLoadFloat4x4(CStaticCamera::Get_View()));
	tBaseShaderCB.matProj = XMMatrixTranspose(XMLoadFloat4x4(CStaticCamera::Get_Proj()));

	m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);

	UINT uiCnt = UINT(vecObjWorld.size() / INSTCNT) + 1;

	// Instancing
	for (UINT uiDrawCnt = 0; uiDrawCnt < UINT(vecObjWorld.size() / INSTCNT) + 1; ++uiDrawCnt)
	{
	INSTSHADER_CB tInstShaderCB;
	UINT uiInstSize = vecObjWorld.size() - (uiDrawCnt * INSTCNT);

	if (uiInstSize > INSTCNT)
	uiInstSize = INSTCNT;

	for (UINT uiIndex = 0; uiIndex < uiInstSize; ++uiIndex)
	tInstShaderCB.matWorld[uiIndex] = XMMatrixTranspose(XMLoadFloat4x4(vecObjWorld[uiIndex + (uiDrawCnt * INSTCNT)]));

	m_pContext->UpdateSubresource(pInstShaderCB, 0, NULL, &tInstShaderCB, 0, 0);

	// Draw
	m_pContext->DrawIndexedInstanced(m_uiIdxCnt, uiInstSize, 0, 0, 0);

	}
	}

	for (UINT uiSize = 0; uiSize < m_vecChild.size(); ++uiSize)
	m_vecChild[uiSize]->RenderInst(vecObjWorld);
	*/
}

void CDynaicMesh::Release(void)
{
	CMesh::Release();

	if (m_dwRefCount == NULL)
	{
		Safe_Delete(m_pOriVertex);
		Safe_Delete(m_pVertex);
		Safe_Delete(m_pIndex);

		Safe_Release(m_pAnimation);
	}

	delete this;
}

HRESULT CDynaicMesh::Set_BoundingBox(void)
{
	// Vertex
	// Vertex
	VTXBONE pVtxTex[] =
	{
		// Pos										TexUV				Normal					Bone			Weight
		{ XMFLOAT3(m_vMin.x, m_vMax.y, m_vMin.z), XMFLOAT2(0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f),{ 0, 0, 0, 0 },{ 1.f, 0.f, 0.f, 0.f } },
		{ XMFLOAT3(m_vMax.x, m_vMax.y, m_vMin.z), XMFLOAT2(0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f),{ 0, 0, 0, 0 },{ 1.f, 0.f, 0.f, 0.f } },
		{ XMFLOAT3(m_vMax.x, m_vMin.y, m_vMin.z), XMFLOAT2(0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f),{ 0, 0, 0, 0 },{ 1.f, 0.f, 0.f, 0.f } },
		{ XMFLOAT3(m_vMin.x, m_vMin.y, m_vMin.z), XMFLOAT2(0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f),{ 0, 0, 0, 0 },{ 1.f, 0.f, 0.f, 0.f } },
		{ XMFLOAT3(m_vMin.x, m_vMax.y, m_vMax.z), XMFLOAT2(0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f),{ 0, 0, 0, 0 },{ 1.f, 0.f, 0.f, 0.f } },
		{ XMFLOAT3(m_vMax.x, m_vMax.y, m_vMax.z), XMFLOAT2(0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f),{ 0, 0, 0, 0 },{ 1.f, 0.f, 0.f, 0.f } },
		{ XMFLOAT3(m_vMax.x, m_vMin.y, m_vMax.z), XMFLOAT2(0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f),{ 0, 0, 0, 0 },{ 1.f, 0.f, 0.f, 0.f } },
		{ XMFLOAT3(m_vMin.x, m_vMin.y, m_vMax.z), XMFLOAT2(0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f),{ 0, 0, 0, 0 },{ 1.f, 0.f, 0.f, 0.f } }
	};

	_uint uiVtxCnt = ARRAYSIZE(pVtxTex);

	D3D11_BUFFER_DESC tBufferDesc;

	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));

	tBufferDesc.ByteWidth = sizeof(VTXBONE) * uiVtxCnt;
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

void CDynaicMesh::CreateBoneNode(MATNODE* pMatNode)
{
	for (int iIndex = 0; iIndex < MAX_BONE_MATRICES; ++iIndex)
		XMStoreFloat4x4(&pMatNode->matBone[iIndex], XMMatrixIdentity());

	if (m_vecChild.empty())
		return;

	int iSize = m_vecChild.size();
	pMatNode->vecNode.reserve(iSize);

	for (int iIndex = 0; iIndex < iSize; ++iIndex)
	{
		pMatNode->vecNode.push_back(new MATNODE);

		CDynaicMesh* pChild = dynamic_cast<CDynaicMesh*>(m_vecChild[iIndex]);
		pChild->CreateBoneNode(pMatNode->vecNode[iIndex]);
	}
}


XMFLOAT4X4 CDynaicMesh::Get_TransBoneMatrix(_int iIndex, _int iIndex2, MATNODE* pMatNode)
{
	if (iIndex != -1)
		return dynamic_cast<CDynaicMesh*>(m_vecChild[iIndex])->Get_TransBoneMatrix(-1, iIndex2, pMatNode->vecNode[iIndex]);

	else
	{
		XMFLOAT4X4 matTransBone;
		XMStoreFloat4x4(&matTransBone, XMMatrixTranslationFromVector(XMLoadFloat3(&m_vPivotPos)) * XMLoadFloat4x4(&pMatNode->matBone[iIndex2]));
		return matTransBone;
	}
}


void CDynaicMesh::Add_ObbCheckList(const XMFLOAT4X4* pWorld, const CGameObject* pObj, MATNODE* pMatNode)
{
	for (size_t iIndex = 0; iIndex < m_vecChild.size(); ++iIndex)
	{
		CDynaicMesh* pChild = dynamic_cast<CDynaicMesh*>(m_vecChild[iIndex]);

		XMFLOAT4X4 matWorld;
		XMStoreFloat4x4(&matWorld, XMLoadFloat4x4(&pMatNode->matBone[0]) * XMLoadFloat4x4(pWorld));

		VTXBONE* pOriVertex = pChild->m_pOriVertex;

		for (_uint uiIndex = 0; uiIndex < pChild->m_uiVtxCnt; ++uiIndex)
		{
			XMFLOAT4X4 matTrans;
			ZeroMemory(&matTrans, sizeof(XMFLOAT4X4));

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					matTrans.m[i][j] += pMatNode->matBone[pOriVertex[uiIndex].uiBones[0]].m[i][j] * pOriVertex[uiIndex].fWeights[0];
					matTrans.m[i][j] += pMatNode->matBone[pOriVertex[uiIndex].uiBones[1]].m[i][j] * pOriVertex[uiIndex].fWeights[1];
					matTrans.m[i][j] += pMatNode->matBone[pOriVertex[uiIndex].uiBones[2]].m[i][j] * pOriVertex[uiIndex].fWeights[2];
					matTrans.m[i][j] += pMatNode->matBone[pOriVertex[uiIndex].uiBones[3]].m[i][j] * pOriVertex[uiIndex].fWeights[3];
				}
			}

			XMStoreFloat3(&pChild->m_pVertex[uiIndex].vPos, XMVector3TransformCoord(XMLoadFloat3(&pOriVertex[uiIndex].vPos)
				, XMLoadFloat4x4(&matTrans) * XMLoadFloat4x4(pWorld)));
		}

		XMFLOAT3 vMax = *m_vecChild[iIndex]->Get_Max();
		vMax = XMFLOAT3(vMax.x * 2.f, vMax.y * 2.f, vMax.z * 2.f);

		pChild->Add_ObbCheckList(pWorld, pObj, pMatNode->vecNode[iIndex]);
	}
}