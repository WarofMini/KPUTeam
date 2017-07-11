#include "stdafx.h"
#include "ResourcesMgr.h"
#include "Resources.h"
#include "Animation.h"
#include "AnimationInfo.h"

IMPLEMENT_SINGLETON(CResourcesMgr)

CResourcesMgr::CResourcesMgr(void)
: m_pmapResource(NULL)
, m_wReservedSize(0)
, m_pFbxSdkMgr(NULL)
, m_pRootMeshStore(NULL)
{
	Ready_FbxSdkMgr();
}

CResourcesMgr::~CResourcesMgr(void)
{
}

CResource* CResourcesMgr::Clone_ResourceMgr(const _ushort& wContainerIdx, const _tchar* pResourceKey)
{
	CResource* pResource = Find_Resource(wContainerIdx, pResourceKey);

	if (NULL == pResource)
	{
		//MSG_BOX(L"Clone Resource Failed");
		return NULL;
	}

	return pResource->Clone_Resource();
}

HRESULT CResourcesMgr::Reserve_ContainerSize(const _ushort& wSize)
{
	if (0 != m_wReservedSize || NULL != m_pmapResource)
		return E_FAIL;

	m_pmapResource = new MAPRESOURCE[wSize];
	m_wReservedSize = wSize;

	return S_OK;
}

void CResourcesMgr::Ready_FbxSdkMgr(void)
{
	if (m_pFbxSdkMgr == NULL)
	{
		m_pFbxSdkMgr = FbxManager::Create();

		FbxIOSettings* pIOsettings = FbxIOSettings::Create(m_pFbxSdkMgr, IOSROOT);
		m_pFbxSdkMgr->SetIOSettings(pIOsettings);
	}
}

HRESULT CResourcesMgr::Ready_Buffer(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext
	, const _ushort& wContainerIdx, BUFFERTYPE eBufferType, const _tchar* pResourceKey
	, const _ushort& wCntX, const _ushort& wCntZ, const _ushort& wItv)
{
	NULL_CHECK_RETURN_MSG(m_wReservedSize, E_FAIL, L"ReservedSize Not Failed");
	NULL_CHECK_RETURN_MSG(m_pmapResource, E_FAIL, L"pmapResource Not Failed");

	CResource* pResource = Find_Resource(wContainerIdx, pResourceKey);

	if (pResource != NULL)
		return E_FAIL;

	switch (eBufferType)
	{
	case BUFFER_RCTEX:
		pResource = CRcTex::Create(pGraphicDev, pContext);
		break;

	case BUFFER_CUBE:
		pResource = CCubeTex::Create(pGraphicDev, pContext);
		break;

	case BUFFER_FLAGTEX:
		pResource = CFlagTex::Create(pGraphicDev, pContext, wCntX, wCntZ, wItv);
		break;
	}

	NULL_CHECK_RETURN_MSG(pResource, E_FAIL, L"Ready_Buffer Create Failed");
	m_pmapResource[wContainerIdx].insert(MAPRESOURCE::value_type(pResourceKey, pResource));

	return S_OK;
}

HRESULT CResourcesMgr::Ready_Texture(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext
	, const _ushort& wContainerIdx, const _tchar* pResourceKey, CTextures::TEXTURETYPE eTextureType
	, const _tchar* pFilePath, const _ushort& wCnt)
{
	NULL_CHECK_RETURN_MSG(m_wReservedSize, E_FAIL, L"ReservedSize Not Failed");
	NULL_CHECK_RETURN_MSG(m_pmapResource, E_FAIL, L"pmapResource Not Failed");

	CResource* pResource = Find_Resource(wContainerIdx, pResourceKey);

	if (NULL != pResource)
		return E_FAIL;

	wstring* pwstrResourceKey = new wstring(pResourceKey);
	m_ResourceNameList.push_back(pwstrResourceKey);

	pResource = CTextures::Create(pGraphicDev, pContext, eTextureType, pFilePath, wCnt);
	NULL_CHECK_RETURN_MSG(pResource, E_FAIL, L"Ready_Texture Create Failed");

	m_pmapResource[wContainerIdx].insert(MAPRESOURCE::value_type(pwstrResourceKey->c_str(), pResource));

	return S_OK;
}

HRESULT CResourcesMgr::Ready_Mesh(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext
	, const _ushort& wContainerIdx, MESHTYPE eMeshType, const _tchar* pResourceKey, const _tchar* pFilePath)
{
	NULL_CHECK_RETURN_MSG(m_wReservedSize, E_FAIL, L"ReservedSize Not Failed");
	NULL_CHECK_RETURN_MSG(m_pmapResource, E_FAIL, L"pmapResource Not Failed");

	CResource* pResource = Find_Resource(wContainerIdx, pResourceKey);

	if (pResource != NULL)
		return E_FAIL;

	wstring* pwstrResourceKey = new wstring(pResourceKey);
	m_ResourceNameList.push_back(pwstrResourceKey);

	// Init
	char szFilePath[MAX_NAME];
	WideCharToMultiByte(CP_ACP, 0, pFilePath, -1, szFilePath, MAX_NAME, NULL, NULL);

	_int iFileFormat = EOF;
	_bool bSuccess = false;

	FbxImporter* pImporter = FbxImporter::Create(m_pFbxSdkMgr, "");
	FbxScene* pFbxScene = FbxScene::Create(m_pFbxSdkMgr, "");

	FbxAxisSystem CurrAxisSystem;//현재 fbx씬내에서의 좌표축을 가져올 변수
	FbxAxisSystem DestAxisSystem = FbxAxisSystem::eMayaYUp;//클라내에서 좌표축을 설정.

														   //파일이 있는지 확인.
	m_pFbxSdkMgr->GetIOPluginRegistry()->DetectReaderFileFormat(szFilePath, iFileFormat);

	bSuccess = pImporter->Initialize(szFilePath, iFileFormat, m_pFbxSdkMgr->GetIOSettings());

	if (!bSuccess)
		return E_FAIL;

	//파일이 있으면 fbx씬에 추가를 한다.
	bSuccess = pImporter->Import(pFbxScene);

	if (!bSuccess)
		return E_FAIL;

	CurrAxisSystem = pFbxScene->GetGlobalSettings().GetAxisSystem();

	//fbx의 씬에서 좌표축을 가져온다.
	if (CurrAxisSystem != DestAxisSystem)
		DestAxisSystem.ConvertScene(pFbxScene);

	//지정한 좌표축과 fbx씬의 좌표축과 비교하여 다르면 지정한 좌표축으로 바꾼다.


	/*FbxAxisSystem CurrAxisSystem = pFbxScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem DestAxisSystem = FbxAxisSystem::eDirectX;

	if (CurrAxisSystem != DestAxisSystem) DestAxisSystem.ConvertScene(pFbxScene);*/

	/* 모델이 삼각형 으로 쪼개져 있지 않으면 쪼개준다. (Fbx File 설정) */

	FbxGeometryConverter lGeomConverter(m_pFbxSdkMgr);
	bSuccess = lGeomConverter.Triangulate(pFbxScene, /* Replace */ true);

	if (!bSuccess)
		return E_FAIL;

	pImporter->Destroy();

	FbxNode* pFbxRootNode = pFbxScene->GetRootNode(); //Scene의 루트노드

	switch (eMeshType)
	{
	case MESH_STATIC:
		Load_StaticMesh(pGraphicDev, pContext, pFbxRootNode, nullptr);
		break;

	case MESH_DYNAMIC:
		// Load Animation Name Data
		FbxArray<FbxString*> AnimStackNameArray;
		pFbxScene->FillAnimStackNameArray(AnimStackNameArray);

		// Load Frame Data
		vector<pair<_ushort, _ushort>> vecFrameCnt;
		Load_FrameData(pResourceKey, vecFrameCnt);

		SetFbxBoneIndex(&mapIndexByName, pFbxRootNode);


		// Load Mesh Data
		Load_DynamicMesh(pGraphicDev, pContext, pFbxRootNode, nullptr, pFbxScene, AnimStackNameArray, vecFrameCnt);

		// Release
		size_t iAnimStackNameSize = AnimStackNameArray.Size();

		for (size_t i = 0; i < iAnimStackNameSize; ++i)
			Safe_Delete(AnimStackNameArray[i]);

		vecFrameCnt.clear();

		break;
	}

	dynamic_cast<CMesh*>(m_pRootMeshStore)->Set_RootMinMax();
	dynamic_cast<CMesh*>(m_pRootMeshStore)->Clear_NullChild();

	pResource = m_pRootMeshStore;
	NULL_CHECK_RETURN_MSG(pResource, E_FAIL, L"Ready_Mesh Create Failed");

	m_pmapResource[wContainerIdx].insert(MAPRESOURCE::value_type(pwstrResourceKey->c_str(), pResource));

	return S_OK;
}

void CResourcesMgr::Load_StaticMesh(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, FbxNode* pNode, CStaticMesh* pParent)
{
	FbxNodeAttribute *pFbxNodeAttribute = pNode->GetNodeAttribute();


	_uint uiVtxCnt = 0;
	_uint uiIdxCnt = 0;

	VTXTEX* pVtxTex = NULL;
	_uint* pIndex = NULL;

	XMFLOAT3 vMin = XMFLOAT3(0.f, 0.f, 0.f);
	XMFLOAT3 vMax = XMFLOAT3(0.f, 0.f, 0.f);

	_tchar wstrTextureName[MAX_PATH];
	ZeroMemory(wstrTextureName, sizeof(_tchar) * MAX_PATH);



	if (pFbxNodeAttribute != nullptr && pFbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		FbxMesh* pMesh = (FbxMesh*)pNode->GetNodeAttribute();

		
		// Compute Bounding Box
		pMesh->ComputeBBox();

		FbxDouble3 min = pMesh->BBoxMin.Get();
		vMin = XMFLOAT3((FLOAT)min.mData[0], (FLOAT)min.mData[1], (FLOAT)min.mData[2]);
		FbxDouble3 max = pMesh->BBoxMax.Get();
		vMax = XMFLOAT3((FLOAT)max.mData[0], (FLOAT)max.mData[1], (FLOAT)max.mData[2]);


		if (vMin.x > vMax.x)
			swap(vMin.x, vMax.x);
		if (vMin.y > vMax.y)
			swap(vMin.y, vMax.y);
		if (vMin.z > vMax.z)
			swap(vMin.z, vMax.z);

		// Get Vertex, UV, Normal, Index, Texture
		FbxVector4* pVertices = pMesh->GetControlPoints();

		uiVtxCnt = pMesh->GetPolygonCount() * 3;

		pVtxTex = new VTXTEX[uiVtxCnt];
		ZeroMemory(pVtxTex, sizeof(VTXTEX) * uiVtxCnt);


		//Index
		uiIdxCnt = uiVtxCnt;
		pIndex = new _uint[uiIdxCnt];
		ZeroMemory(pIndex, sizeof(_uint) * uiIdxCnt);

		// Vertex
		//for (UINT i = 0; i < uiVtxCnt; ++i)
		//{
		//	pVtxTex[i].vPos = XMFLOAT3((FLOAT)pVertices[i].mData[0], (FLOAT)pVertices[i].mData[1], (FLOAT)pVertices[i].mData[2]);
		//	XMStoreFloat3(&pVtxTex[i].vPos, XMVector3TransformCoord(XMLoadFloat3(&pVtxTex[i].vPos), XMLoadFloat4x4(&matWorld)));
		//}

		int iVTXCounter = 0;
		UINT uiPolygonCount2 = pMesh->GetPolygonCount();

		for (UINT uiPolygonIndex = 0; uiPolygonIndex < uiPolygonCount2; ++uiPolygonIndex)
		{
			int iNumVertices = pMesh->GetPolygonSize(uiPolygonIndex);
			assert(iNumVertices == 3);
			FbxGeometryElementUV* VtxUV = pMesh->GetElementUV(0);
			FbxGeometryElementNormal* VtxNormal = pMesh->GetElementNormal(0);


			for (int k = 0; k < iNumVertices; k++) // 폴리곤을 구성하는 버텍스의 인덱스
			{

				//정점 데이터 얻는곳
				int iControlPointIndex = pMesh->GetPolygonVertex(uiPolygonIndex, k); // 컨트롤 포인트 = 하나의 버텍스
				int iTextureUVIndex = pMesh->GetTextureUVIndex(uiPolygonIndex, k);  // Control = Vertex

				//인덱스
				pIndex[iVTXCounter] = iVTXCounter;

				pVtxTex[iVTXCounter].vPos = XMFLOAT3((_float)pVertices[iControlPointIndex].mData[0],
					(_float)pVertices[iControlPointIndex].mData[1],
					(_float)pVertices[iControlPointIndex].mData[2]);

				switch (VtxUV->GetMappingMode()) //UV값 추출
				{
				case FbxGeometryElement::eByControlPoint: // 하나의 컨트롤 포인트가 하나의 노멀벡터를 가질때
					switch (VtxUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						pVtxTex[iVTXCounter].vTexUV.x = static_cast<float>(VtxUV->GetDirectArray().GetAt(iControlPointIndex).mData[0]);
						pVtxTex[iVTXCounter].vTexUV.y = 1.f - static_cast<float>(VtxUV->GetDirectArray().GetAt(iControlPointIndex).mData[1]);
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int index = VtxUV->GetIndexArray().GetAt(iControlPointIndex);
						pVtxTex[iVTXCounter].vTexUV.x = static_cast<float>(VtxUV->GetDirectArray().GetAt(index).mData[0]);
						pVtxTex[iVTXCounter].vTexUV.y = 1 - static_cast<float>(VtxUV->GetDirectArray().GetAt(index).mData[1]);
					}
					break;
					default:
						throw std::exception("Invalid Reference");
					}
					break;


				case FbxGeometryElement::eByPolygonVertex:  // Sharp Edge 포인트가 존재할때 고로 우리가 실질적으로 쓰는곳
					switch (VtxUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						pVtxTex[iVTXCounter].vTexUV.x = static_cast<float>(VtxUV->GetDirectArray().GetAt(iTextureUVIndex).mData[0]);
						pVtxTex[iVTXCounter].vTexUV.y = 1 - static_cast<float>(VtxUV->GetDirectArray().GetAt(iTextureUVIndex).mData[1]);
					}
					case FbxGeometryElement::eIndexToDirect:
					{

						pVtxTex[iVTXCounter].vTexUV.x = static_cast<float>(VtxUV->GetDirectArray().GetAt(iTextureUVIndex).mData[0]);
						pVtxTex[iVTXCounter].vTexUV.y = 1 - static_cast<float>(VtxUV->GetDirectArray().GetAt(iTextureUVIndex).mData[1]);
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

				//노멀얻기
				switch (VtxNormal->GetMappingMode()) // 노멀값 추출
				{
				case FbxGeometryElement::eByControlPoint: // 하나의 컨트롤 포인트가 하나의 노멀벡터를 가질때
					switch (VtxNormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						pVtxTex[iVTXCounter].vNormal.x = static_cast<float>(VtxNormal->GetDirectArray().GetAt(iControlPointIndex).mData[0]);
						pVtxTex[iVTXCounter].vNormal.y = static_cast<float>(VtxNormal->GetDirectArray().GetAt(iControlPointIndex).mData[1]);
						pVtxTex[iVTXCounter].vNormal.z = static_cast<float>(VtxNormal->GetDirectArray().GetAt(iControlPointIndex).mData[2]);
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int index = VtxNormal->GetIndexArray().GetAt(iControlPointIndex);
						pVtxTex[iVTXCounter].vNormal.x = static_cast<float>(VtxNormal->GetDirectArray().GetAt(index).mData[0]);
						pVtxTex[iVTXCounter].vNormal.y = static_cast<float>(VtxNormal->GetDirectArray().GetAt(index).mData[1]);
						pVtxTex[iVTXCounter].vNormal.z = static_cast<float>(VtxNormal->GetDirectArray().GetAt(index).mData[2]);
					}
					break;

					default:
						throw std::exception("Invalid Reference");
					}


					break;

				case FbxGeometryElement::eByPolygonVertex:  // Sharp Edge 포인트가 존재할때 고로 우리가 실질적으로 쓰는곳
					switch (VtxNormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						int index = VtxNormal->GetIndexArray().GetAt(iVTXCounter);
						pVtxTex[iVTXCounter].vNormal.x = static_cast<float>(VtxNormal->GetDirectArray().GetAt(index).mData[0]);
						pVtxTex[iVTXCounter].vNormal.y = static_cast<float>(VtxNormal->GetDirectArray().GetAt(index).mData[1]);
						pVtxTex[iVTXCounter].vNormal.z = static_cast<float>(VtxNormal->GetDirectArray().GetAt(index).mData[2]);
					}
					case FbxGeometryElement::eIndexToDirect:
					{
						int index = VtxNormal->GetIndexArray().GetAt(iVTXCounter);
						pVtxTex[iVTXCounter].vNormal.x = static_cast<float>(VtxNormal->GetDirectArray().GetAt(index).mData[0]);
						pVtxTex[iVTXCounter].vNormal.y = static_cast<float>(VtxNormal->GetDirectArray().GetAt(index).mData[1]);
						pVtxTex[iVTXCounter].vNormal.z = static_cast<float>(VtxNormal->GetDirectArray().GetAt(index).mData[2]);
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

				iVTXCounter += 1;
			}
		}


		//// UV
		/*
		FbxStringList UVNames;
		pMesh->GetUVSetNames(UVNames);

		UINT uiPolygonCount = pMesh->GetPolygonCount();

		for (UINT uiPolygonIndex = 0; uiPolygonIndex < uiPolygonCount; ++uiPolygonIndex)
		{
		UINT uiVerticeCount = pMesh->GetPolygonSize(uiPolygonIndex);

		D3DXVECTOR2 outUV;
		FbxGeometryElementUV* vertexUV = pMesh->GetElementUV(0);
		FbxGeometryElementNormal* vertexNormal = pMesh->GetElementNormal(0);


		for (UINT uiVerticeIndex = 0; uiVerticeIndex < uiVerticeCount; ++uiVerticeIndex)
		{
		UINT uiIndex = pMesh->GetPolygonVertex(uiPolygonIndex, uiVerticeIndex);

		UINT TextureUVIndex = pMesh->GetTextureUVIndex(uiPolygonIndex, uiVerticeIndex);


		FbxVector4 vNormal;

		pMesh->GetPolygonVertexNormal(uiPolygonIndex, uiVerticeIndex, vNormal);

		pVtxTex[uiIndex].vNormal = XMFLOAT3((FLOAT)vNormal.Buffer()[0],
		(FLOAT)vNormal.Buffer()[1],
		(FLOAT)vNormal.Buffer()[2]);


		FbxVector2 vUV= FbxVector2(0.0, 0.0);
		bool bUnmappedUV;


		if (UVNames.GetCount())
		{
		const char* pUVName = NULL;
		pUVName = UVNames[0];

		pMesh->GetPolygonVertexUV(uiPolygonIndex, uiVerticeIndex, pUVName, vUV, bUnmappedUV);
		pVtxTex[uiIndex].vTexUV = XMFLOAT2((FLOAT)vUV.mData[0],  1.f - (FLOAT)vUV.mData[1]);
		}

		}
		}
		*/

		// Index
		/*
		UINT index = 0;

		for (UINT uiPolygonIndex = 0; uiPolygonIndex < uiPolygonCount; ++uiPolygonIndex)
		{
		UINT uiVerticeCount = pMesh->GetPolygonSize(uiPolygonIndex);
		for (UINT uiVerticeIndex = 0; uiVerticeIndex < uiVerticeCount; ++uiVerticeIndex)
		++index;
		}

		uiIdxCnt = index;
		pIndex = new UINT[uiIdxCnt];
		ZeroMemory(pIndex, sizeof(UINT) * uiIdxCnt);

		for (UINT uiPolygonIndex = 0, uiCuridx = 0; uiPolygonIndex < uiPolygonCount; ++uiPolygonIndex)
		{
		UINT uiVerticeCount = pMesh->GetPolygonSize(uiPolygonIndex);
		for (UINT uiVerticeIndex = 0; uiVerticeIndex < uiVerticeCount; ++uiVerticeIndex)
		{
		UINT uiIndex = pMesh->GetPolygonVertex(uiPolygonIndex, uiVerticeIndex);
		pIndex[uiCuridx] = uiIndex;
		++uiCuridx;
		}
		}
		*/

		// Texture
		FbxSurfaceMaterial * pMaterial = pNode->GetMaterial(0);

		if (pMaterial)
		{
			FbxProperty pProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

			if (pProperty.IsValid())
			{
				FbxFileTexture* pTexture = pProperty.GetSrcObject<FbxFileTexture>();

				if (pTexture)
				{
					const FbxString strFileName = pTexture->GetFileName();
					UINT uiFileLength = 0;
					for (int i = strFileName.Size(); i > 0; --i)
					{
						if (strFileName.Buffer()[i] == '/' || strFileName.Buffer()[i] == 92)
						{
							uiFileLength = i;
							wsprintf(wstrTextureName, L"%S", strFileName.Right(strFileName.Size() - uiFileLength - 1).Left(strFileName.Size() - uiFileLength - 5));
							break;
						}
					}

					if (uiFileLength == 0)
						wsprintf(wstrTextureName, L"%S", strFileName.Left(strFileName.Size() - 4));
				}
			}
		}
	}



	CStaticMesh* pStaticMesh = CStaticMesh::Create(pGraphicDev, pContext, pVtxTex, uiVtxCnt, pIndex, uiIdxCnt, vMin, vMax, wstrTextureName);

	Safe_Delete_Array(pVtxTex);
	Safe_Delete_Array(pIndex);

	if (pParent == nullptr)
		m_pRootMeshStore = pStaticMesh;

	else
		pParent->Add_Child(pStaticMesh);

	_uint uiNodeChild = pNode->GetChildCount();
	pStaticMesh->Reserve_ChildSize(uiNodeChild);

	for (UINT uiCnt = 0; uiCnt < uiNodeChild; uiCnt++)
	{
		FbxNode *pChildNode = pNode->GetChild(uiCnt);
		Load_StaticMesh(pGraphicDev, pContext, pChildNode, pStaticMesh);
	}
}

void CResourcesMgr::Load_DynamicMesh(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, FbxNode* pNode, CDynaicMesh* pParent
	, FbxScene* pFbxScene, FbxArray<FbxString*>& arrAniName, vector<pair<_ushort, _ushort>>& vecFrameCnt)
{
	//Nodes는 Scene내부 요소들의 position, rotation, scale을 명시하는데 사용
	//FbxScene은 노드의 부모-자식 계층 구조를 표현(트리)
	//이 트리의 루트 노드는 GetRootNode() 메서드를 통해 접근


	//FbxNodeAttribute는 FbxNode와 1쌍으로 존재
	//FbxNode에 만약 아무런 NodeAttribute가 설정 안되어 있으면 GetNodeAttribute() 호출 시 NULL을 반환
	FbxNodeAttribute *pFbxNodeAttribute = pNode->GetNodeAttribute();

	UINT uiVtxCnt = 0;
	UINT uiIdxCnt = 0;

	VTXBONE* pVtxBone = nullptr;
	UINT* pIndex = nullptr;

	CAnimation* pAnimation = nullptr;

	XMFLOAT3 vMin = XMFLOAT3(0.f, 0.f, 0.f);
	XMFLOAT3 vMax = XMFLOAT3(0.f, 0.f, 0.f);

	TCHAR wstrTextureName[MAX_PATH];
	ZeroMemory(wstrTextureName, sizeof(TCHAR) * MAX_PATH);

	//Attribute 타입
	if (pFbxNodeAttribute != nullptr && pFbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		//우리가 익스포트 하려는 메쉬
		FbxMesh* pMesh = (FbxMesh*)pNode->GetNodeAttribute();

		// Get Transform
		XMFLOAT4X4 matWorld;
		{
			//  EvaluateLocalTransform 같은건가? -> 결과가 똑같이 나옴
			FbxMatrix matFbxWorld = pNode->EvaluateGlobalTransform();

			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					matWorld.m[i][j] = (FLOAT)matFbxWorld.Get(i, j);
		}

		// Compute Bounding Box=============================================================================
		{
			pMesh->ComputeBBox();

			FbxDouble3 min = pMesh->BBoxMin.Get();
			vMin = XMFLOAT3((FLOAT)min.mData[0], (FLOAT)min.mData[1], (FLOAT)min.mData[2]);
			FbxDouble3 max = pMesh->BBoxMax.Get();
			vMax = XMFLOAT3((FLOAT)max.mData[0], (FLOAT)max.mData[1], (FLOAT)max.mData[2]);

			XMStoreFloat3(&vMin, XMVector3TransformCoord(XMLoadFloat3(&vMin), XMLoadFloat4x4(&matWorld)));
			XMStoreFloat3(&vMax, XMVector3TransformCoord(XMLoadFloat3(&vMax), XMLoadFloat4x4(&matWorld)));

			if (vMin.x > vMax.x) swap(vMin.x, vMax.x);
			if (vMin.y > vMax.y) swap(vMin.y, vMax.y);
			if (vMin.z > vMax.z) swap(vMin.z, vMax.z);
		}
		//===================================================================================================

		// Get Vertex, UV, Normal, Index, Texture

		//ControlPointsCount() = 물리적인 vertex    (걍 vertex)

		
		uiVtxCnt = pMesh->GetControlPointsCount();//Vertex의 갯수
		pVtxBone = new VTXBONE[uiVtxCnt];
		ZeroMemory(pVtxBone, sizeof(VTXBONE) * uiVtxCnt);

		// Vertex
		{
			FbxVector4* pVertices = pMesh->GetControlPoints();//물리적인 Vertex

			for (_uint i = 0; i < uiVtxCnt; ++i)
			{
				//Vertex의 Position을 대입
				pVtxBone[i].vPos = XMFLOAT3((_float)pVertices[i].mData[0], (_float)pVertices[i].mData[1], (_float)pVertices[i].mData[2]);

				//도대체 이건 왜 필요한것인가
				XMStoreFloat3(&pVtxBone[i].vPos, XMVector3TransformCoord(XMLoadFloat3(&pVtxBone[i].vPos), XMLoadFloat4x4(&matWorld)));

				for (_int boneindex = 0; boneindex < 4; ++boneindex)
				{
					pVtxBone[i].uiBones[boneindex] = MAX_BONE_MATRICES - 1;
					pVtxBone[i].fWeights[boneindex] = 0.f;
				}
			}
		}		
		

		// Index, UV
		{
			_uint uiPolygonCount = pMesh->GetPolygonCount(); //폴리곤 개수

			uiIdxCnt = uiPolygonCount * 3;
			pIndex = new _uint[uiIdxCnt];
			ZeroMemory(pIndex, sizeof(_uint) * uiIdxCnt);
			_uint uiIdxArrIndex = 0;

			FbxStringList UVNames;
			pMesh->GetUVSetNames(UVNames);

			for (_uint uiPolygonIndex = 0; uiPolygonIndex < uiPolygonCount; uiPolygonIndex++)
			{
				_uint uiVerticeCount = pMesh->GetPolygonSize(uiPolygonIndex);

				assert(uiVerticeCount == 3);

				for (_uint uiVerticeIndex = 0; uiVerticeIndex < uiVerticeCount; uiVerticeIndex++)
				{
		
					_uint uiIndex = pMesh->GetPolygonVertex(uiPolygonIndex, uiVerticeIndex);

					
					pIndex[uiIdxArrIndex] = uiIndex;
					uiIdxArrIndex++;

					FbxVector4 vNormal;

					pMesh->GetPolygonVertexNormal(uiPolygonIndex, uiVerticeIndex, vNormal);

					pVtxBone[uiIndex].vNormal = XMFLOAT3((_float)vNormal.Buffer()[0],
													 	 (_float)vNormal.Buffer()[1],
													 	 (_float)vNormal.Buffer()[2]);

					FbxVector2 vUV;
					_bool bUnmappedUV;

					if (UVNames.GetCount())
					{
						const char* pUVName = NULL;
						pUVName = UVNames[0];

						pMesh->GetPolygonVertexUV(uiPolygonIndex, uiVerticeIndex, pUVName, vUV, bUnmappedUV);
						pVtxBone[uiIndex].vTexUV = XMFLOAT2((_float)vUV.Buffer()[0], 1.0f - (_float)vUV.Buffer()[1]);
					}
				}
			}
		}

		// Texture
		{
			FbxSurfaceMaterial * pMaterial = pNode->GetMaterial(0);

			if (pMaterial)
			{
				FbxProperty pProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

				if (pProperty.IsValid())
				{
					FbxFileTexture* pTexture = pProperty.GetSrcObject<FbxFileTexture>();
					if (pTexture)
					{
						const FbxString strFileName = pTexture->GetFileName();

						UINT uiFileLength = 0;
						for (int i = strFileName.Size(); i > 0; --i)
						{
							if (strFileName.Buffer()[i] == '/' || strFileName.Buffer()[i] == 92)
							{
								uiFileLength = i;
								wsprintf(wstrTextureName, L"%S", strFileName.Right(strFileName.Size() - uiFileLength - 1).Left(strFileName.Size() - uiFileLength - 5));
								break;
							}
						}

						if (uiFileLength == 0)
							wsprintf(wstrTextureName, L"%S", strFileName.Left(strFileName.Size() - 4));
					}
				}
			}
		}




		// Bones & Weights & Animation
		//Deformer안에 Cluster라는 것들을 갖고있다. (보통 메시 1개당 Deformer 1개이다.)
		//Cluster안에 Link라는게 있는데 이게 진짜 joint이다.
		FbxSkin* pSkinDeformer = (FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin);

		if (pSkinDeformer != nullptr)
		{
			_uint uiClusterCount = pSkinDeformer->GetClusterCount();

			if (pMesh->GetDeformerCount(FbxDeformer::eSkin) && uiClusterCount)
			{
				// Bones & Weights
				{
					_uint* bonecount = new _uint[uiVtxCnt];
					ZeroMemory(bonecount, sizeof(_uint) * uiVtxCnt);

					for (_uint i = 0; i < uiClusterCount; ++i)
					{
						FbxCluster* pCluster = pSkinDeformer->GetCluster(i);

						if (!pCluster->GetLink())
							continue;

						//본의 이름
						
						//string BoneName = pCluster->GetLink()->GetName();
						//int INDEX = mapIndexByName[BoneName];


						_uint uiVertexIndexCount = pCluster->GetControlPointIndicesCount();

						for (_uint j = 0; j < uiVertexIndexCount; j++)
						{
							_uint uiVectexIndex = pCluster->GetControlPointIndices()[j];
						
							

							if (pVtxBone[uiVectexIndex].fWeights[bonecount[uiVectexIndex]] == 0.f)
							{
									pVtxBone[uiVectexIndex].uiBones[bonecount[uiVectexIndex]] = i;
									pVtxBone[uiVectexIndex].fWeights[bonecount[uiVectexIndex]] = (_float)pCluster->GetControlPointWeights()[j];
							}

							bonecount[uiVectexIndex] += 1;
						}
					}

					/*
					for (_uint i = 0; i < uiVtxCnt; ++i)
					{
						float fullWeights = pVtxBone[i].fWeights[0] + pVtxBone[i].fWeights[1] + pVtxBone[i].fWeights[2] + pVtxBone[i].fWeights[3];

						if (fullWeights < 1.0f)
							pVtxBone[i].fWeights[3] = 1.0f - fullWeights;
					}
					*/

					Safe_Delete_Array(bonecount);
				}




				//Animation
				{
					pAnimation = CAnimation::Create(pContext);

					// Time
					FbxAnimStack* pAnimStack = pFbxScene->FindMember<FbxAnimStack>(arrAniName[0]->Buffer());
					pFbxScene->SetCurrentAnimationStack(pAnimStack);

					FbxTakeInfo* pTakeInfo = pFbxScene->GetTakeInfo(*(arrAniName[0]));

					FbxTime stopTime = pTakeInfo->mLocalTimeSpan.GetStop();
					FbxLongLong frameCnt = stopTime.GetFrameCount();

					if (frameCnt == 0)
						frameCnt = 1;

					FbxLongLong stop = stopTime.Get();

					FbxLongLong unit = stop / frameCnt;

					for (_uint uiDataIdx = 0; uiDataIdx < vecFrameCnt.size(); ++uiDataIdx)
					{
						CAnimation::ANIMDATA tAnimData;

						for (WORD wFrame = vecFrameCnt[uiDataIdx].first; wFrame <= vecFrameCnt[uiDataIdx].second; ++wFrame)
						{
							CAnimation::FRAME vecAniMatrix;

							// Matrix
							if (pFbxScene->GetPoseCount())
							{
								//FbxPose* pPose = pFbxScene->GetPose(pFbxScene->GetPoseCount() + 1);

								for (_uint uiClusterIndex = 0; uiClusterIndex < uiClusterCount; ++uiClusterIndex)
								{
									FbxAMatrix matVertexTransform;

									matVertexTransform.SetIdentity();


									FbxCluster* pCluster = pSkinDeformer->GetCluster(uiClusterIndex);
									FbxTime pTime = unit * wFrame;

									if (pCluster->GetLink())
									{
										FbxAMatrix TransBoneMtx;
										FbxAMatrix LinkBoneMtx, lClusterGlobalCurPos;
										FbxAMatrix lRefGeometry;


										lRefGeometry = GetGeometry(pMesh->GetNode()); //대부분 단위행렬

										//============================================================
										//GetTransformMatrix : 전체 메시의 Global Transform이고,
										//모든 cluster는 정확하게 같은 TransformMatrix를 가진다.
										pCluster->GetTransformMatrix(TransBoneMtx);
										TransBoneMtx *= lRefGeometry;
										//============================================================

										//============================================================
										//Cluster(Joint)의 변환이다. 마야에서 Joint Space -> World Space로 변하는 변환
										pCluster->GetTransformLinkMatrix(LinkBoneMtx);
										//============================================================

										

										//시간에 따른 EvaluateGlobalTransform
										//lClusterGlobalCurPos = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);

										lClusterGlobalCurPos = pCluster->GetLink()->EvaluateGlobalTransform(pTime);

										//옛날버전
										matVertexTransform = TransBoneMtx * lClusterGlobalCurPos * LinkBoneMtx.Inverse();

										//matVertexTransform = lClusterGlobalCurPos * LinkBoneMtx.Inverse() * TransBoneMtx;


									}
	

									FbxQuaternion vQ = matVertexTransform.GetQ();
									FbxVector4 vT = matVertexTransform.GetT();
									FbxVector4 vS = matVertexTransform.GetS();

									CLUSDATA tClusData;
									tClusData.R = XMFLOAT4((_float)vQ.mData[0], (_float)vQ.mData[1], (_float)vQ.mData[2], (_float)vQ.mData[3]);
									tClusData.T = XMFLOAT3((_float)vT.mData[0], (_float)vT.mData[1], (_float)vT.mData[2]);
									tClusData.S = XMFLOAT3((_float)vS.mData[0], (_float)vS.mData[1], (_float)vS.mData[2]);
									vecAniMatrix.push_back(tClusData);
								}
							}

							tAnimData.push_back(vecAniMatrix);
						}

						pAnimation->Push_AnimData(tAnimData);
					}
				}
			}
		}
	}

	CDynaicMesh* pDynamicMesh = CDynaicMesh::Create(pGraphicDev, pContext, pVtxBone, uiVtxCnt, pIndex, uiIdxCnt
		, vMin, vMax, wstrTextureName, pAnimation);

	Safe_Delete_Array(pVtxBone);
	Safe_Delete_Array(pIndex);

	if (pParent == nullptr)
		m_pRootMeshStore = pDynamicMesh;

	else
		pParent->Add_Child(pDynamicMesh);

	UINT uiNodeChild = pNode->GetChildCount();
	pDynamicMesh->Reserve_ChildSize(uiNodeChild);

	for (UINT uiCnt = 0; uiCnt < uiNodeChild; uiCnt++)
	{
		FbxNode *pChildNode = pNode->GetChild(uiCnt);
		Load_DynamicMesh(pGraphicDev, pContext, pChildNode, pDynamicMesh, pFbxScene, arrAniName, vecFrameCnt);
	}
}

CResource* CResourcesMgr::Find_Resource(const _ushort& wContainerIdx, const _tchar* pResourceKey)
{

	MAPRESOURCE::iterator iter = find_if(m_pmapResource[wContainerIdx].begin()
		, m_pmapResource[wContainerIdx].end(), CTag_Finder(pResourceKey));


	if (iter == m_pmapResource[wContainerIdx].end())
		return NULL;

	return iter->second;
}

void CResourcesMgr::Reset_Resource(const _ushort& wContainerIdx)
{
	for_each(m_pmapResource[wContainerIdx].begin(), m_pmapResource[wContainerIdx].end(), CRelease_Pair());
	m_pmapResource[wContainerIdx].clear();
}

void CResourcesMgr::Release(void)
{
	if (0 == m_wReservedSize)
		return;

	for (size_t i = 0; i < m_wReservedSize; ++i)
	{
		for_each(m_pmapResource[i].begin(), m_pmapResource[i].end(), CRelease_Pair());
		m_pmapResource[i].clear();
	}

	Safe_Delete_Array(m_pmapResource);

	list<wstring*>::iterator iter_end = m_ResourceNameList.end();
	for (list<wstring*>::iterator iter = m_ResourceNameList.begin(); iter != iter_end; ++iter)
		Safe_Delete(*iter);

	m_ResourceNameList.clear();

	m_pFbxSdkMgr->Destroy();

	delete this;
}

void CResourcesMgr::SetFbxBoneIndex(map<std::string, unsigned int>* _pIndexByName, FbxNode * _pNode)
{
	(*_pIndexByName)[_pNode->GetName()] = _pIndexByName->size();

	for (int i = 0; i < _pNode->GetChildCount(); ++i)
		this->SetFbxBoneIndex(_pIndexByName, _pNode->GetChild(i));
}

//애니메이션 정보 Load
void CResourcesMgr::Load_FrameData(const _tchar* pResourceKey, vector<pair<_ushort, _ushort>>& vecFrameCnt)
{
	pair<_ushort, _ushort> pairFrame;						// -> StartFrame & EndFrame
	CAnimationInfo::VECFRAMEINFO vecFrameInfo;		// -> KeyName & KeyInex & FrameCnt & FrameSpeed
	FRAMEINFO tFrameInfo;

	vecFrameCnt.reserve(50);

	wstring wstrPath = L"../Bin/Data/";
	wstrPath += pResourceKey;
	wstrPath += L".txt";

	wifstream inFile;
	inFile.open(wstrPath.c_str(), ios::in);

	_tchar pStartFrame[MAXFRAME];
	_tchar pEndFrame[MAXFRAME];
	_tchar pKeyName[MAX_NAME];
	_tchar pFrameSpeed[MAXFRAME];

	while (!inFile.eof())
	{
		inFile.getline(pStartFrame, MAXFRAME, '-');
		inFile.getline(pEndFrame, MAXFRAME, '-');
		inFile.getline(pKeyName, MAX_NAME, '-');
		inFile.getline(pFrameSpeed, MAXFRAME);

		pairFrame.first = _wtoi(pStartFrame);
		pairFrame.second = _wtoi(pEndFrame);

		tFrameInfo.wFrameCnt = (pairFrame.second - pairFrame.first) + 1;
		tFrameInfo.wFrameSpeed = _wtoi(pFrameSpeed);

		vecFrameCnt.push_back(pairFrame);	//여기에 시작 프레임, 끝 프레임
		vecFrameInfo.push_back(tFrameInfo);	//여기는 프레임 수랑 속도
	}

	inFile.close();

	CAnimationInfo::Push_FrameInfo(vecFrameInfo);
}

FbxAMatrix CResourcesMgr::GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition)
{
	FbxAMatrix lGlobalPosition;
	_bool        lPositionFound = false;

	if (pPose)
	{
		_int lNodeIndex = pPose->Find(pNode);

		if (lNodeIndex > -1)
		{
			if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
				lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);

			else
			{
				FbxAMatrix lParentGlobalPosition;

				if (pParentGlobalPosition)
					lParentGlobalPosition = *pParentGlobalPosition;

				else
				{
					if (pNode->GetParent())
						lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pTime, pPose);
				}

				FbxAMatrix lLocalPosition = GetPoseMatrix(pPose, lNodeIndex);
				lGlobalPosition = lParentGlobalPosition * lLocalPosition;
			}

			lPositionFound = true;
		}
	}

	if (!lPositionFound)
		lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);

	return lGlobalPosition;
}
FbxAMatrix CResourcesMgr::GetPoseMatrix(FbxPose* pPose, _int pNodeIndex)
{
	FbxAMatrix lPoseMatrix;
	FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);

	memcpy((_double*)lPoseMatrix, (_double*)lMatrix, sizeof(lMatrix.mData));

	return lPoseMatrix;
}

//대부분 그냥 단위행렬이다.
FbxAMatrix CResourcesMgr::GetGeometry(FbxNode* pNode)
{
	const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}