#include "stdafx.h"
#include "Shader.h"
#include "ShaderMgr.h"

CShader::CShader(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
: m_pGraphicDev(pGraphicDev)
, m_pContext(pContext)
, m_pVertexShader(NULL)
, m_pVertexLayout(NULL)
, m_pPixelShader(NULL)
{
}

CShader::~CShader(void)
{
}

CShader* CShader::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, _tchar* szFileName, _ubyte byLayoutFlag)
{
	CShader* pShader = new CShader(pGraphicDev, pContext);

	if (FAILED(pShader->Ready_Shader(szFileName, byLayoutFlag)))
	{
		MSG_BOX(L"CShader Ready_Shader Failed");
		Safe_Release(pShader);
	}

	return pShader;
}

HRESULT CShader::Ready_Shader(_tchar* szFileName, _ubyte byLayoutFlag)
{
	// Create Vertex Shader
	ID3DBlob* pVSBlob = NULL;

	if (FAILED(Load_ShaderFromFile(szFileName, "VS", "vs_4_0", &pVSBlob)))
	{
		MSG_BOX(L"Load_ShaderFromFile VS Failed");
		return E_FAIL;
	}

	if (FAILED(m_pGraphicDev->CreateVertexShader(pVSBlob->GetBufferPointer()
		, pVSBlob->GetBufferSize(), NULL, &m_pVertexShader)))
	{
		MSG_BOX(L"CreateVertexShader Failed");
		return E_FAIL;
	}

	// Create InputLayout
	_uint uiIndex = 2;
	_uint uiOffset = 32;
	D3D11_INPUT_ELEMENT_DESC tLayout[5];

	tLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	tLayout[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	tLayout[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	if (byLayoutFlag & CShaderMgr::LAYOUT_FLAG_BONE)
	{
		++uiIndex;
		tLayout[uiIndex] = { "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, uiOffset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		uiOffset += 16;

		++uiIndex;
		tLayout[uiIndex] = { "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, uiOffset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		uiOffset += 16;
	}

	if (FAILED(m_pGraphicDev->CreateInputLayout(tLayout, uiIndex + 1
		, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pVertexLayout)))
	{
		MSG_BOX(L"CreateInputLayout Failed");
		return E_FAIL;
	}

	pVSBlob->Release();

	// Create Pixel Shader
	ID3DBlob* pPSBlob = NULL;
	if (FAILED(Load_ShaderFromFile(szFileName, "PS", "ps_4_0", &pPSBlob)))
	{
		MSG_BOX(L"Load_ShaderFromFile PS Failed");
		return E_FAIL;
	}

	if (FAILED(m_pGraphicDev->CreatePixelShader(pPSBlob->GetBufferPointer()
		, pPSBlob->GetBufferSize(), NULL, &m_pPixelShader)))
	{
		MSG_BOX(L"CreatePixelShader Failed");
		return E_FAIL;
	}

	pPSBlob->Release();

	return S_OK;
}

HRESULT CShader::Load_ShaderFromFile(_tchar* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	_ulong dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

void CShader::Release(void)
{
	if (Safe_Com_Release(m_pVertexShader))
		MSG_BOX(L"m_pVertexShader Release Failed");

	if (Safe_Com_Release(m_pVertexLayout))
		MSG_BOX(L"m_pVertexLayout Release Failed");

	if (Safe_Com_Release(m_pPixelShader))
		MSG_BOX(L"m_pPixelShader Release Failed");

	delete this;
}

ID3D11VertexShader* CShader::Get_VertexShader(void)
{
	return m_pVertexShader;
}

ID3D11InputLayout* CShader::Get_InputLayout(void)
{
	return m_pVertexLayout;
}

ID3D11PixelShader* CShader::Get_PixelShader(void)
{
	return m_pPixelShader;
}
