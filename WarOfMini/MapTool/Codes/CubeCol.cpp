#include "stdafx.h"
#include "CubeCol.h"
#include "Device.h"


CCubeCol::CCubeCol()
{
	ZeroMemory(&m_pVtxCol, sizeof(VTXCOL) * 8);
}


CCubeCol::~CCubeCol()
{
}

HRESULT CCubeCol::CreateBuffer(void)
{
	//����
	HRESULT hr = E_FAIL;
	m_iVertexStrides = sizeof(VTXCOL);
	m_iVertexOffsets = 0;

	D3DXVECTOR3 vMin, vMax;


	VTXCOL Vertex[] =
	{
		{ D3DXVECTOR3(vMin.x,vMax.y, vMin.z), D3DXCOLOR(1,0,0,1) },
		{ D3DXVECTOR3(vMax.x, vMax.y, vMin.z), D3DXCOLOR(1,0,0,1) },
		{ D3DXVECTOR3(vMax.x, vMin.y, vMin.z), D3DXCOLOR(1,0,0,1) },
		{ D3DXVECTOR3(vMin.x, vMin.y, vMin.z), D3DXCOLOR(1,0,0,1) },
		{ D3DXVECTOR3(vMin.x, vMax.y, vMax.z), D3DXCOLOR(1,0,0,1) },
		{ D3DXVECTOR3(vMax.x, vMax.y, vMax.z), D3DXCOLOR(1,0,0,1) },
		{ D3DXVECTOR3(vMax.x, vMin.y, vMax.z), D3DXCOLOR(1,0,0,1) },
		{ D3DXVECTOR3(vMin.x, vMin.y, vMax.z), D3DXCOLOR(1,0,0,1) },
	};

	D3D11_BUFFER_DESC vbd; // ���� ��ũ = ������ �뵵�� ũ��� ���ٱ���
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DEFAULT; // ���۴� ������ ���� �ʿ��� ���δ� �ϵ����� ������ ������ ��ü�� 2�������� CPU, GPU
	vbd.ByteWidth = sizeof(VTXCOL) * 8; // ũ��
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // �뵵 = ���۴� 3���� ����. ���ؽ�, �ε���, �ܽ���Ʈ 
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vBufferData; // ���긮�ҽ��� �������δ� = �������� ���� �ǹ���
	ZeroMemory(&vBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vBufferData.pSysMem = Vertex;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&vbd, &vBufferData, &m_VertexBuffer); 

	if (FAILED(hr))
		return E_FAIL;

	
	UINT Index[] =
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
		0, 2, 3,
	};

	m_iIndex = 36;

	D3D11_BUFFER_DESC ibd; // ����/ũ��/�뵵
	ibd.Usage = D3D11_USAGE_DEFAULT; // ����
	ibd.ByteWidth = sizeof(UINT) * m_iIndex; // ũ��
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER; // �뵵
	ibd.CPUAccessFlags = 0; // CPU���ٱ��� ����
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData; // ���� ������
	iinitData.pSysMem = Index;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&ibd, &iinitData, &m_IndexBuffer);

	if (FAILED(hr))
		return E_FAIL;


	//���̷�Ʈ9 != ���̷�Ʈ11
	//���̷�Ʈ9 = ������Ʈ ���̺��� ���� �� ���� �۷ι� ������ ���� ä������ �ʰ� �Ҵ縸 �Ǿ��־��� = �޸� ����
	//���̷�Ʈ11 = ����� ������� ��� ���۰� ä������ ������ ���̴��� �۵��������� = �޸� ����ȭ

	D3D11_BUFFER_DESC cbd; // ���ٱ��� / �뵵/ ũ��
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;
	cbd.MiscFlags = 0;
	cbd.StructureByteStride = 0;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbd, NULL, &m_ConstantBuffer);
	
	/*D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC; // CPU���� �б⸦ �Ҽ��ִ� ������ �ֽ�
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // ����Ʈ�� ������ �־��� ����?
	CDevice::GetInstance()->m_pDevice->CreateBuffer(&bd, NULL, &m_ConstantBuffer);*/


	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Constant Buffer Error", MB_OK);
		return hr;

	}

	return S_OK;
}

HRESULT CCubeCol::CreateBuffer(D3DXVECTOR3 & vMin, D3DXVECTOR3 & vMax, D3DXCOLOR & dwColor)
{
	CreateRasterizerState();
	//����
	HRESULT hr = E_FAIL;
	m_iVertexStrides = sizeof(VTXCOL);
	m_iVertexOffsets = 0;


	m_pVtxCol[0] = { D3DXVECTOR3(vMin.x, vMax.y, vMin.z), dwColor };
	m_pVtxCol[1] = { D3DXVECTOR3(vMax.x, vMax.y, vMin.z), dwColor };
	m_pVtxCol[2] = { D3DXVECTOR3(vMax.x, vMin.y, vMin.z), dwColor };
	m_pVtxCol[3] = { D3DXVECTOR3(vMin.x, vMin.y, vMin.z), dwColor };
	m_pVtxCol[4] = { D3DXVECTOR3(vMin.x, vMax.y, vMax.z), dwColor };
	m_pVtxCol[5] = { D3DXVECTOR3(vMax.x, vMax.y, vMax.z), dwColor };
	m_pVtxCol[6] = { D3DXVECTOR3(vMax.x, vMin.y, vMax.z), dwColor };
	m_pVtxCol[7] = { D3DXVECTOR3(vMin.x, vMin.y, vMax.z), dwColor };	


	D3D11_BUFFER_DESC vbd; // ���� ��ũ = ������ �뵵�� ũ��� ���ٱ���
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DEFAULT; // ���۴� ������ ���� �ʿ��� ���δ� �ϵ����� ������ ������ ��ü�� 2�������� CPU, GPU
	vbd.ByteWidth = sizeof(VTXCOL) * 8; // ũ��
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // �뵵 = ���۴� 3���� ����. ���ؽ�, �ε���, �ܽ���Ʈ 
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vBufferData; // ���긮�ҽ��� �������δ� = �������� ���� �ǹ���
	ZeroMemory(&vBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vBufferData.pSysMem = m_pVtxCol;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&vbd, &vBufferData, &m_VertexBuffer);

	if (FAILED(hr))
		return E_FAIL;


	UINT Index[] =
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
		0, 2, 3,
	};

	m_iIndex = 36;

	D3D11_BUFFER_DESC ibd; // ����/ũ��/�뵵
	ibd.Usage = D3D11_USAGE_DEFAULT; // ����
	ibd.ByteWidth = sizeof(UINT) * m_iIndex; // ũ��
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER; // �뵵

	ibd.CPUAccessFlags = 0; // CPU���ٱ��� ����
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData; // ���� ������
	iinitData.pSysMem = Index;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&ibd, &iinitData, &m_IndexBuffer);

	if (FAILED(hr))
		return E_FAIL;


	//���̷�Ʈ9 != ���̷�Ʈ11
	//���̷�Ʈ9 = ������Ʈ ���̺��� ���� �� ���� �۷ι� ������ ���� ä������ �ʰ� �Ҵ縸 �Ǿ��־��� = �޸� ����
	//���̷�Ʈ11 = ����� ������� ��� ���۰� ä������ ������ ���̴��� �۵��������� = �޸� ����ȭ

	D3D11_BUFFER_DESC cbd; // ���ٱ��� / �뵵/ ũ��
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;
	cbd.MiscFlags = 0;
	cbd.StructureByteStride = 0;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbd, NULL, &m_ConstantBuffer);


	return S_OK;
}

CCubeCol * CCubeCol::Create(void)
{
	CCubeCol*	pCubeCol = new CCubeCol;

	if (FAILED(pCubeCol->CreateBuffer()))
		Safe_Delete(pCubeCol);

	return pCubeCol;
}

CCubeCol * CCubeCol::Create(D3DXVECTOR3 & vMin, D3DXVECTOR3 & vMax, D3DXCOLOR & dwColor)
{
	CCubeCol*	pCubeCol = new CCubeCol;

	if (FAILED(pCubeCol->CreateBuffer(vMin, vMax, dwColor)))
		Safe_Delete(pCubeCol);

	return pCubeCol;
}

CResources * CCubeCol::CloneResource(void)
{
	CResources* pResource = new CCubeCol(*this);

	pResource->AddRef();

	return pResource;
}

void CCubeCol::CreateRasterizerState()
{
	CDevice* pGrapicDevice = CDevice::GetInstance();

	D3D11_RASTERIZER_DESC tRasterizerDesc;
	ZeroMemory(&tRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	tRasterizerDesc.CullMode = D3D11_CULL_NONE;
	tRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	pGrapicDevice->m_pDevice->CreateRasterizerState(&tRasterizerDesc, &m_pRasterizerState);


}

