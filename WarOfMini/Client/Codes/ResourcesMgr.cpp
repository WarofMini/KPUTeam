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

	case BUFFER_TERRAIN:
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
	if (!bSuccess) return E_FAIL;

	//파일이 있으면 fbx씬에 추가를 한다.
	bSuccess = pImporter->Import(pFbxScene);
	if (!bSuccess) return E_FAIL;

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
	
	if (!bSuccess) return E_FAIL;

	pImporter->Destroy();

	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();

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

	VTXTEX* pVtxTex = nullptr;
	_uint* pIndex = nullptr;

	_vec3 vMin = _vec3(0.f, 0.f, 0.f);
	_vec3 vMax = _vec3(0.f, 0.f, 0.f);

	_tchar wstrTextureName[MAX_PATH];
	ZeroMemory(wstrTextureName, sizeof(_tchar) * MAX_PATH);

	if (pFbxNodeAttribute != nullptr && pFbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		FbxMesh* pMesh = (FbxMesh*)pNode->GetNodeAttribute();

		// Get Transform
		_matrix matWorld;
		FbxMatrix matFbxWorld = pNode->EvaluateGlobalTransform();

		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				matWorld.m[i][j] = (_float)matFbxWorld.Get(i, j);

		// Compute Bounding Box
		pMesh->ComputeBBox();

		FbxDouble3 min = pMesh->BBoxMin.Get();
		vMin = _vec3((_float)min.mData[0], (_float)min.mData[1], (_float)min.mData[2]);
		FbxDouble3 max = pMesh->BBoxMax.Get();
		vMax = _vec3((_float)max.mData[0], (_float)max.mData[1], (_float)max.mData[2]);

		D3DXVec3TransformCoord(&vMin, &vMin, &matWorld);

		D3DXVec3TransformCoord(&vMax, &vMax, &matWorld);


		if (vMin.x > vMax.x) 
			swap(vMin.x, vMax.x);
		if (vMin.y > vMax.y) 
			swap(vMin.y, vMax.y);
		if (vMin.z > vMax.z) 
			swap(vMin.z, vMax.z);

		// Get Vertex, UV, Normal, Index, Texture
		FbxVector4* pVertices = pMesh->GetControlPoints();

		uiVtxCnt = pMesh->GetControlPointsCount();
		pVtxTex = new VTXTEX[uiVtxCnt];
		ZeroMemory(pVtxTex, sizeof(VTXTEX) * uiVtxCnt);

		FbxVector2 vFbxUV(0, 0);

		// Vertex
		for (_uint i = 0; i < uiVtxCnt; ++i)
		{
			pVtxTex[i].vPos = _vec3((_float)pVertices[i].mData[0], (_float)pVertices[i].mData[1], (_float)pVertices[i].mData[2]);

			D3DXVec3TransformCoord(&pVtxTex[i].vPos, &pVtxTex[i].vPos, &matWorld);
		}

		// UV
		FbxStringList UVNames;
		pMesh->GetUVSetNames(UVNames);

		_uint uiPolygonCount = pMesh->GetPolygonCount();

		for (_uint uiPolygonIndex = 0; uiPolygonIndex < uiPolygonCount; ++uiPolygonIndex)
		{
			_uint uiVerticeCount = pMesh->GetPolygonSize(uiPolygonIndex);

			for (_uint uiVerticeIndex = 0; uiVerticeIndex < uiVerticeCount; ++uiVerticeIndex)
			{
				_uint uiIndex = pMesh->GetPolygonVertex(uiPolygonIndex, uiVerticeIndex);

				FbxVector4 vNormal;

				pMesh->GetPolygonVertexNormal(uiPolygonIndex, uiVerticeIndex, vNormal);

				pVtxTex[uiIndex].vNormal = _vec3((_float)vNormal.Buffer()[0],
												(_float)vNormal.Buffer()[1],
												(_float)vNormal.Buffer()[2]);

				FbxVector2 vUV;
				_bool bUnmappedUV;

				if (UVNames.GetCount())
				{
					const char* pUVName = NULL;
					pUVName = UVNames[0];

					pMesh->GetPolygonVertexUV(uiPolygonIndex, uiVerticeIndex, pUVName, vUV, bUnmappedUV);
					pVtxTex[uiIndex].vTexUV = _vec2((_float)vUV.Buffer()[0], -(_float)vUV.Buffer()[1]);
				}
			}
		}

		// Index
		_uint index = 0;

		for (_uint uiPolygonIndex = 0; uiPolygonIndex < uiPolygonCount; ++uiPolygonIndex)
		{
			_uint uiVerticeCount = pMesh->GetPolygonSize(uiPolygonIndex);
			for (_uint uiVerticeIndex = 0; uiVerticeIndex < uiVerticeCount; ++uiVerticeIndex)
				++index;
		}

		uiIdxCnt = index;
		pIndex = new _uint[uiIdxCnt];
		ZeroMemory(pIndex, sizeof(_uint) * uiIdxCnt);

		for (_uint uiPolygonIndex = 0, uiCuridx = 0; uiPolygonIndex < uiPolygonCount; ++uiPolygonIndex)
		{
			_uint uiVerticeCount = pMesh->GetPolygonSize(uiPolygonIndex);

			for (_uint uiVerticeIndex = 0; uiVerticeIndex < uiVerticeCount; ++uiVerticeIndex)
			{
				_uint uiIndex = pMesh->GetPolygonVertex(uiPolygonIndex, uiVerticeIndex);
				pIndex[uiCuridx] = uiIndex;
				++uiCuridx;
			}
		}

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
					_uint uiFileLength = 0;
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

	for (_uint uiCnt = 0; uiCnt < uiNodeChild; uiCnt++)
	{
		FbxNode *pChildNode = pNode->GetChild(uiCnt);
		Load_StaticMesh(pGraphicDev, pContext, pChildNode, pStaticMesh);
	}
}

void CResourcesMgr::Load_DynamicMesh(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, FbxNode* pNode, CDynaicMesh* pParent
	, FbxScene* pFbxScene, FbxArray<FbxString*>& arrAniName, vector<pair<_ushort, _ushort>>& vecFrameCnt)
{
	FbxNodeAttribute *pFbxNodeAttribute = pNode->GetNodeAttribute();

	_uint uiVtxCnt = 0;
	_uint uiIdxCnt = 0;

	VTXBONE* pVtxBone = nullptr;
	_uint* pIndex = nullptr;

	CAnimation* pAnimation = nullptr;

	_vec3 vMin = _vec3(0.f, 0.f, 0.f);
	_vec3 vMax = _vec3(0.f, 0.f, 0.f);

	_tchar wstrTextureName[MAX_PATH];
	ZeroMemory(wstrTextureName, sizeof(_tchar) * MAX_PATH);

	if (pFbxNodeAttribute != nullptr && pFbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		FbxMesh* pMesh = (FbxMesh*)pNode->GetNodeAttribute();

		// Get Transform
		_matrix matWorld;
		{
			FbxMatrix matFbxWorld = pNode->EvaluateGlobalTransform();

			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					matWorld.m[i][j] = (_float)matFbxWorld.Get(i, j);
		}

		// Compute Bounding Box
		{
			pMesh->ComputeBBox();

			FbxDouble3 min = pMesh->BBoxMin.Get();
			vMin = _vec3((_float)min.mData[0], (_float)min.mData[1], (_float)min.mData[2]);
			FbxDouble3 max = pMesh->BBoxMax.Get();
			vMax = _vec3((_float)max.mData[0], (_float)max.mData[1], (_float)max.mData[2]);


			D3DXVec3TransformCoord(&vMin, &vMin, &matWorld);
			D3DXVec3TransformCoord(&vMax, &vMax, &matWorld);


			if (vMin.x > vMax.x) 
				swap(vMin.x, vMax.x);

			if (vMin.y > vMax.y) 
				swap(vMin.y, vMax.y);

			if (vMin.z > vMax.z) 
				swap(vMin.z, vMax.z);
		}

		// Get Vertex, UV, Normal, Index, Texture
		uiVtxCnt = pMesh->GetControlPointsCount();
		pVtxBone = new VTXBONE[uiVtxCnt];
		ZeroMemory(pVtxBone, sizeof(VTXBONE) * uiVtxCnt);

		FbxVector2 vFbxUV(0, 0);

		// Vertex
		{
			FbxVector4* pVertices = pMesh->GetControlPoints();

			for (_uint i = 0; i < uiVtxCnt; ++i)
			{
				pVtxBone[i].vPos = _vec3((_float)pVertices[i].mData[0], (_float)pVertices[i].mData[1], (_float)pVertices[i].mData[2]);

				D3DXVec3TransformCoord(&pVtxBone[i].vPos, &pVtxBone[i].vPos, &matWorld);


				for (_int boneindex = 0; boneindex < 4; ++boneindex)
				{
					pVtxBone[i].uiBones[boneindex] = MAX_BONE_MATRICES - 1;
					pVtxBone[i].fWeights[boneindex] = 0.f;
				}
			}
		}

		// Index, UV
		{
			_uint uiPolygonCount = pMesh->GetPolygonCount();

			uiIdxCnt = uiPolygonCount * 3;
			pIndex = new _uint[uiIdxCnt];
			ZeroMemory(pIndex, sizeof(_uint) * uiIdxCnt);
			_uint uiIdxArrIndex = 0;

			FbxStringList UVNames;
			pMesh->GetUVSetNames(UVNames);

			for (_uint uiPolygonIndex = 0; uiPolygonIndex < uiPolygonCount; ++uiPolygonIndex)
			{
				_uint uiVerticeCount = pMesh->GetPolygonSize(uiPolygonIndex);

				for (_uint uiVerticeIndex = 0; uiVerticeIndex < uiVerticeCount; ++uiVerticeIndex)
				{
					_uint uiIndex = pMesh->GetPolygonVertex(uiPolygonIndex, uiVerticeIndex);

					pIndex[uiIdxArrIndex] = uiIndex;
					++uiIdxArrIndex;

					FbxVector4 vNormal;

					pMesh->GetPolygonVertexNormal(uiPolygonIndex, uiVerticeIndex, vNormal);

					pVtxBone[uiIndex].vNormal = _vec3((_float)vNormal.Buffer()[0],
						(_float)vNormal.Buffer()[1],
						(_float)vNormal.Buffer()[2]);

					FbxVector2 vUV;
					_bool bUnmappedUV;

					if (UVNames.GetCount())
					{
						const char* pUVName = NULL;
						pUVName = UVNames[0];

						pMesh->GetPolygonVertexUV(uiPolygonIndex, uiVerticeIndex, pUVName, vUV, bUnmappedUV);
						pVtxBone[uiIndex].vTexUV = _vec2((_float)vUV.Buffer()[0], -(_float)vUV.Buffer()[1]);
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

						_uint uiFileLength = 0;
						for (_int i = strFileName.Size(); i > 0; --i)
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
		FbxSkin* pSkinDeformer = (FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin);
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

				for (_uint i = 0; i < uiVtxCnt; ++i)
				{
					float fullWeights = pVtxBone[i].fWeights[0] + pVtxBone[i].fWeights[1] + pVtxBone[i].fWeights[2] + pVtxBone[i].fWeights[3];
					
					if (fullWeights < 1.0f)
						pVtxBone[i].fWeights[3] = 1.0f - fullWeights;
				}

				Safe_Delete_Array(bonecount);
			}

			// Animation
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

				FbxAMatrix matGlobalPosition;
				matGlobalPosition.SetIdentity();

				for (_uint uiDataIdx = 0; uiDataIdx < vecFrameCnt.size(); ++uiDataIdx)
				{
					CAnimation::ANIMDATA tAnimData;

					for (WORD wFrame = vecFrameCnt[uiDataIdx].first; wFrame <= vecFrameCnt[uiDataIdx].second; ++wFrame)
					{
						CAnimation::FRAME vecAniMatrix;

						// Matrix
						if (pFbxScene->GetPoseCount())
						{
							FbxPose* pPose = pFbxScene->GetPose(pFbxScene->GetPoseCount() + 1);

							for (_uint uiClusterIndex = 0; uiClusterIndex < uiClusterCount; ++uiClusterIndex)
							{
								FbxAMatrix matVertexTransform, matGlobalPosition;
								matVertexTransform.SetIdentity();
								matGlobalPosition.SetIdentity();

								FbxCluster* pCluster = pSkinDeformer->GetCluster(uiClusterIndex);
								FbxTime pTime = unit * wFrame;

								if (pCluster->GetLink())
								{
									FbxAMatrix lRefGlobalInitPos, lRefGlobalCurrentPos;
									FbxAMatrix lClusterGlobalInitPos, lClusterGlobalCurPos;
									FbxAMatrix lRefGeometry;
									FbxAMatrix lClusterRelativeInitPos, lClusterRelativeCurPosInv;


									pCluster->GetTransformMatrix(lRefGlobalInitPos);

									//링크의 매트릭스
									pCluster->GetTransformLinkMatrix(lClusterGlobalInitPos);

									//단위행렬 값을 넣어준다.
									lRefGlobalCurrentPos = matGlobalPosition;

									lRefGeometry = GetGeometry(pMesh->GetNode());


									//해당 노드의 EvaluateGlobalTransform 값 반환
									lClusterGlobalCurPos = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);
									//=====================================================================

									lClusterRelativeInitPos = lClusterGlobalInitPos.Inverse() * lRefGeometry;

									lClusterRelativeCurPosInv = lClusterGlobalCurPos;


									matVertexTransform = lRefGlobalInitPos * lClusterRelativeCurPosInv * lClusterRelativeInitPos;
								}

								FbxQuaternion vQ = matVertexTransform.GetQ();
								FbxVector4 vT = matVertexTransform.GetT();
								FbxVector4 vS = matVertexTransform.GetS();

								CLUSDATA tClusData;

								tClusData.R = _vec4((_float)vQ.mData[0], (_float)vQ.mData[1], (_float)vQ.mData[2], (_float)vQ.mData[3]);
								tClusData.T = _vec3((_float)vT.mData[0], (_float)vT.mData[1], (_float)vT.mData[2]);
								tClusData.S = _vec3((_float)vS.mData[0], (_float)vS.mData[1], (_float)vS.mData[2]);
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

	CDynaicMesh* pDynamicMesh = CDynaicMesh::Create(pGraphicDev, pContext, pVtxBone, uiVtxCnt, pIndex, uiIdxCnt
		, vMin, vMax, wstrTextureName, pAnimation);

	Safe_Delete_Array(pVtxBone);
	Safe_Delete_Array(pIndex);

	if (pParent == nullptr)
		m_pRootMeshStore = pDynamicMesh;

	else
		pParent->Add_Child(pDynamicMesh);

	_uint uiNodeChild = pNode->GetChildCount();
	pDynamicMesh->Reserve_ChildSize(uiNodeChild);

	for (_uint uiCnt = 0; uiCnt < uiNodeChild; uiCnt++)
	{
		FbxNode *pChildNode = pNode->GetChild(uiCnt);
		Load_DynamicMesh(pGraphicDev, pContext, pChildNode, pDynamicMesh, pFbxScene, arrAniName, vecFrameCnt);
	}
}

CResource* CResourcesMgr::Find_Resource(const _ushort& wContainerIdx, const _tchar* pResourceKey)
{
	MAPRESOURCE::iterator iter = find_if(m_pmapResource[wContainerIdx].begin()
		, m_pmapResource[wContainerIdx].end(), CTag_Finder(pResourceKey));

	MAPRESOURCE map = m_pmapResource[2];

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

//애니메이션 정보 Load
void CResourcesMgr::Load_FrameData(const _tchar* pResourceKey, vector<pair<_ushort, _ushort>>& vecFrameCnt)
{
	pair<_ushort, _ushort> pairFrame;						// -> StartFrame & EndFrame
	CAnimationInfo::VECFRAMEINFO vecFrameInfo;		// -> KeyName & KeyInex & FrameCnt & FrameSpeed
	CAnimationInfo::FRAMEINFO tFrameInfo;

	vecFrameCnt.reserve(10);

	wstring wstrPath = L"../Bin/Data/";
	wstrPath += pResourceKey;
	wstrPath += L".txt";

	wifstream inFile;
	inFile.open(wstrPath.c_str(), ios::in);

	_tchar pStartFrame[4];
	_tchar pEndFrame[4];
	_tchar pKeyName[MAX_NAME];
	_tchar pFrameSpeed[4];

	while (!inFile.eof())
	{
		inFile.getline(pStartFrame, 4, '-');
		inFile.getline(pEndFrame, 4, '-');
		inFile.getline(pKeyName, MAX_NAME, '-');
		inFile.getline(pFrameSpeed, 4);

		pairFrame.first = _wtoi(pStartFrame);
		pairFrame.second = _wtoi(pEndFrame);

		tFrameInfo.wFrameCnt = (pairFrame.second - pairFrame.first) + 1;
		tFrameInfo.wFrameSpeed = _wtoi(pFrameSpeed);

		vecFrameCnt.push_back(pairFrame);
		vecFrameInfo.push_back(tFrameInfo);
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
FbxAMatrix CResourcesMgr::GetGeometry(FbxNode* pNode)
{
	const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}