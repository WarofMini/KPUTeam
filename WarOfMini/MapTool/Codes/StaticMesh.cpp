#include "stdafx.h"
#include "StaticMesh.h"
#include "Device.h"
#include "CubeCol.h"

CStaticMesh::CStaticMesh()
{
}


CStaticMesh::~CStaticMesh()
{
}

CStaticMesh * CStaticMesh::Create(const char * szFilePath, const char * szFileName)
{
	CStaticMesh* pStaticMesh = new CStaticMesh;

	if (FAILED(pStaticMesh->Initalize(szFilePath, szFileName)))
	{
		::Safe_Delete(pStaticMesh);
	}

	return pStaticMesh;
}

CResources * CStaticMesh::CloneResource()
{
	CResources* pResource = this;

	pResource->AddRef();

	return pResource;
}

HRESULT CStaticMesh::Load_StaticMesh(const char* szFilePath,const char* szFileName, FbxManager* _pFBXManager, FbxIOSettings* _pIOsettings, FbxScene* _pFBXScene, FbxImporter* _pImporter)
{
	HRESULT hr = E_FAIL;

	vector<UINT> vecIndeces;

	string	strFullPath;

	strFullPath.clear();
	strFullPath = szFilePath;
	strFullPath += szFileName;//��ο� �����̸� �߰�

	if (!(_pImporter->Initialize(strFullPath.c_str(), -1, _pFBXManager->GetIOSettings())))
		FAILED_CHECK_MSG(E_FAIL, L"Static Mesh Init Failed");
	if (!(_pImporter->Import(_pFBXScene)))
		FAILED_CHECK_MSG(E_FAIL, L"Static Mesh Import Failed");

	FbxGeometryConverter clsConverter(_pFBXManager);
	clsConverter.Triangulate(_pFBXScene, false);
	FbxNode* pRootNode = _pFBXScene->GetRootNode();

	if (!pRootNode)
		return E_FAIL;

	vector<VTXTEX> vecVTXTEX;

	int chilecount = pRootNode->GetChildCount();

	for (int i = 0; i < pRootNode->GetChildCount(); ++i)
	{
		FbxNode* pChildNode = pRootNode->GetChild(i);

		if (pChildNode->GetNodeAttribute() == NULL)
			continue;

		FbxNodeAttribute::EType AttributeType = pChildNode->GetNodeAttribute()->GetAttributeType();

		if (AttributeType != FbxNodeAttribute::eMesh)
			continue;

		FbxMesh* pMesh = (FbxMesh*)pChildNode->GetNodeAttribute();  // ����Ʈ �Ϸ��� �޽��� ������
		

		D3DXVECTOR3 vPos;
		D3DXVECTOR2 vOutUV;
		D3DXVECTOR3 vOutNormal;
		FbxVector4* mControlPoints = pMesh->GetControlPoints();
		int iVTXCounter = 0;

		int polygoncount = pMesh->GetPolygonCount();
		int vertextcount = pMesh->GetControlPointsCount();

		for (int j = 0; j < pMesh->GetPolygonCount(); j++) // �������� �ε���
		{
			int iNumVertices = pMesh->GetPolygonSize(j);
			assert(iNumVertices == 3);
			FbxGeometryElementUV* VtxUV = pMesh->GetElementUV(0);
			FbxGeometryElementNormal* VtxNormal = pMesh->GetElementNormal(0);

			for (int k = 0; k < iNumVertices; k++) // �������� �����ϴ� ���ؽ��� �ε���
			{
				//���� ������ ��°�
				int iControlPointIndex = pMesh->GetPolygonVertex(j, k); // ��Ʈ�� ����Ʈ = �ϳ��� ���ؽ�
				int iTextureUVIndex = pMesh->GetTextureUVIndex(j, k);  // Control = Vertex
				//int iNormalIndex = pMesh->GetPolygonVertexIndex(j, k);
				++iVTXCounter;

				vPos.x = (float)mControlPoints[iControlPointIndex].mData[0];
				vPos.y = (float)mControlPoints[iControlPointIndex].mData[1];
				vPos.z = (float)mControlPoints[iControlPointIndex].mData[2];

				//uv ���
				switch (VtxUV->GetMappingMode()) // UV�� ����
				{
				case FbxGeometryElement::eByControlPoint: // �ϳ��� ��Ʈ�� ����Ʈ�� �ϳ��� ��ֺ��͸� ������

					switch (VtxUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						vOutUV.x = static_cast<float>(VtxUV->GetDirectArray().GetAt(iControlPointIndex).mData[0]);
						vOutUV.y = 1.f - static_cast<float>(VtxUV->GetDirectArray().GetAt(iControlPointIndex).mData[1]);
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int index = VtxUV->GetIndexArray().GetAt(iControlPointIndex);
						vOutUV.x = static_cast<float>(VtxUV->GetDirectArray().GetAt(index).mData[0]);
						vOutUV.y = 1 - static_cast<float>(VtxUV->GetDirectArray().GetAt(index).mData[1]);
					}
					break;

					default:
						throw std::exception("Invalid Reference");
					}


					break;


				case FbxGeometryElement::eByPolygonVertex:  // Sharp Edge ����Ʈ�� �����Ҷ� ��� �츮�� ���������� ���°�
					switch (VtxUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						vOutUV.x = static_cast<float>(VtxUV->GetDirectArray().GetAt(iTextureUVIndex).mData[0]);
						vOutUV.y = 1 - static_cast<float>(VtxUV->GetDirectArray().GetAt(iTextureUVIndex).mData[1]);
					}
					case FbxGeometryElement::eIndexToDirect:
					{

						vOutUV.x = static_cast<float>(VtxUV->GetDirectArray().GetAt(iTextureUVIndex).mData[0]);
						vOutUV.y = 1 - static_cast<float>(VtxUV->GetDirectArray().GetAt(iTextureUVIndex).mData[1]);
					}
					break;
					default:
						throw std::exception("invalid Reference");
					}
					break;
				default:
					throw std::exception("Invalid Reference");
					break;
				}

				//��־��
				switch (VtxNormal->GetMappingMode()) // ��ְ� ����
				{
				case FbxGeometryElement::eByControlPoint: // �ϳ��� ��Ʈ�� ����Ʈ�� �ϳ��� ��ֺ��͸� ������

					switch (VtxNormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						vOutNormal.x = static_cast<float>(VtxNormal->GetDirectArray().GetAt(iControlPointIndex).mData[0]);
						vOutNormal.y = static_cast<float>(VtxNormal->GetDirectArray().GetAt(iControlPointIndex).mData[1]);
						vOutNormal.z = static_cast<float>(VtxNormal->GetDirectArray().GetAt(iControlPointIndex).mData[2]);
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int index = VtxNormal->GetIndexArray().GetAt(iControlPointIndex);
						vOutNormal.x = static_cast<float>(VtxNormal->GetDirectArray().GetAt(index).mData[0]);
						vOutNormal.y = static_cast<float>(VtxNormal->GetDirectArray().GetAt(index).mData[1]);
						vOutNormal.z = static_cast<float>(VtxNormal->GetDirectArray().GetAt(index).mData[2]);
					}
					break;

					default:
						throw std::exception("Invalid Reference");
					}


					break;


				case FbxGeometryElement::eByPolygonVertex:  // Sharp Edge ����Ʈ�� �����Ҷ� ��� �츮�� ���������� ���°�
					switch (VtxNormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						int index = VtxNormal->GetIndexArray().GetAt(iVTXCounter);
						vOutNormal.x = static_cast<float>(VtxNormal->GetDirectArray().GetAt(index).mData[0]);
						vOutNormal.y = static_cast<float>(VtxNormal->GetDirectArray().GetAt(index).mData[1]);
						vOutNormal.z = static_cast<float>(VtxNormal->GetDirectArray().GetAt(index).mData[2]);
					}
					case FbxGeometryElement::eIndexToDirect:
					{
						int index = VtxNormal->GetIndexArray().GetAt(iVTXCounter);
						vOutNormal.x = static_cast<float>(VtxNormal->GetDirectArray().GetAt(index).mData[0]);
						vOutNormal.y = static_cast<float>(VtxNormal->GetDirectArray().GetAt(index).mData[1]);
						vOutNormal.z = static_cast<float>(VtxNormal->GetDirectArray().GetAt(index).mData[2]);
					}
					break;
					default:
						throw std::exception("invalid Reference");
					}
					break;
				default:
					throw std::exception("Invalid Reference");
					break;
				}


				VTXTEX vtxtex;
				vtxtex.vPos = vPos;
				vtxtex.vNormal = vOutNormal;
				vtxtex.vTexUV = vOutUV;
				vecVTXTEX.push_back(vtxtex);


				//int index = VtxUV->GetIndexArray().GetAt(iTextureUVIndex);
				vecIndeces.push_back(VtxUV->GetIndexArray().GetAt(iTextureUVIndex));
			}
		}

	}

	unsigned int n = vecVTXTEX.size();
	pVTXTex = new VTXTEX[n];
	for (unsigned int i = 0; i < vecVTXTEX.size(); ++i)
	{
		pVTXTex[i].vPos = vecVTXTEX[i].vPos;
		pVTXTex[i].vNormal = vecVTXTEX[i].vNormal;
		pVTXTex[i].vTexUV = vecVTXTEX[i].vTexUV;
	}

	m_iVertices = vecVTXTEX.size();
	m_iVertexStrides = sizeof(VTXTEX);
	m_iVertexOffsets = 0; //�޽��� ������ ����

	MakeVertexNormal((BYTE*)pVTXTex, NULL);

	D3D11_BUFFER_DESC tBufferDesc;
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	tBufferDesc.ByteWidth = m_iVertexStrides * m_iVertices;
	tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA tData;
	ZeroMemory(&tData, sizeof(D3D11_SUBRESOURCE_DATA));
	tData.pSysMem = pVTXTex;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&tBufferDesc, &tData, &m_VertexBuffer);


	if (FAILED(hr))
		return E_FAIL;


	D3D11_BUFFER_DESC cbd;
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;
	cbd.MiscFlags = 0;
	cbd.StructureByteStride = 0;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbd, NULL, &m_ConstantBuffer);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Constant Buffer Error", MB_OK);
		return hr;
	}

	//�ٿ���ڽ� ���������� �Լ�
	D3DXComputeBoundingBox(&vecVTXTEX[m_iVertexOffsets].vPos, m_iVertices, m_iVertexStrides, &m_vMin,  &m_vMax);
	
	return S_OK;
}

HRESULT CStaticMesh::Initalize(const char * szFilePath, const char * szFileName)
{
	m_eDrawType = DRAW_VERTEX;
	FbxManager* pFBXManager = FbxManager::Create();
	FbxIOSettings* pIOsettings = FbxIOSettings::Create(pFBXManager, IOSROOT);
	pFBXManager->SetIOSettings(pIOsettings);
	FbxScene* pFBXScene = FbxScene::Create(pFBXManager, "");

	FbxImporter* pImporter = FbxImporter::Create(pFBXManager, "");


	if (FAILED(Load_StaticMesh(szFilePath, szFileName, pFBXManager, pIOsettings, pFBXScene, pImporter)))
		return E_FAIL;


	CMesh::CreateRasterizerState();


	//Init_Shader();
	pFBXScene->Destroy();
	pImporter->Destroy();
	pIOsettings->Destroy();
	pFBXManager->Destroy();


	return S_OK;
}
