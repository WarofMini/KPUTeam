#include "stdafx.h"
#include "DynamicMesh.h"
#include "AniBuffer.h"
#include "FbxParser.h"
#include "TimeMgr.h"
#include "Device.h"
#include "Shader.h"


CDynamicMesh::CDynamicMesh()
:m_wCurrenAniIdx(-1),
 m_fAniPlayTimer(0.f)
{
}


CDynamicMesh::~CDynamicMesh()
{
	Release();
}

CDynamicMesh* CDynamicMesh::Create(const char* _pPath, vector<string> _vecAniName)
{
	CDynamicMesh* pComponent = new CDynamicMesh;

	if (FAILED(pComponent->Initialize(_pPath, _vecAniName)))
		::Safe_Delete(pComponent);

	return pComponent;
}

HRESULT CDynamicMesh::Initialize(const char* _pPath, vector<string> _vecAniName)
{
	FbxManager*	pFBXManager = FbxManager::Create();
	FbxIOSettings* pIOsettings = FbxIOSettings::Create(pFBXManager, IOSROOT);
	pFBXManager->SetIOSettings(pIOsettings);
	FbxScene* pFBXScene = FbxScene::Create(pFBXManager, "");

	FbxImporter* pImporter = FbxImporter::Create(pFBXManager, "");
	//fbx�� ����Ʈ �ʱ�ȭ.

	FAILED_CHECK_RETURN(Load_Model(_pPath, _vecAniName, pFBXManager, pIOsettings, pFBXScene, pImporter), E_FAIL);

	CMesh::CreateRasterizerState();


	pFBXScene->Destroy();
	pImporter->Destroy();
	pIOsettings->Destroy();
	pFBXManager->Destroy();

	m_bAniEnd = false;
	Yamae = false;

	HRESULT hr;

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

	return S_OK;
}


HRESULT CDynamicMesh::Load_Model(const char* _pPath, vector<string> _vecAniName, FbxManager* _pFBXManager, FbxIOSettings* _pIOsettings,
									FbxScene* _pFBXScene, FbxImporter* _pImporter)
{
	int		iFileFormat = EOF;
	string	strFullPath;
	string	strExtenstion = ".FBX";

	FbxAxisSystem CurrAxisSystem;//���� fbx���������� ��ǥ���� ������ ����
	FbxAxisSystem DestAxisSystem = FbxAxisSystem::eMayaYUp;//Ŭ�󳻿��� ��ǥ���� ����.
	FbxGeometryConverter lGeomConverter(_pFBXManager);

	FbxNode*		pFbxRootNode;
	Animation*		pAni;


	for (unsigned int i = 0; i < _vecAniName.size(); ++i)
	{
		// http://www.slideshare.net/quxn6/c4316-alignment-error
		pAni = new Animation();
		pAni->strAniName = _vecAniName[i];//fbx�� �̸� ����.

		strFullPath.clear();
		strFullPath = _pPath;
		strFullPath += _vecAniName[i];//��ο� �����̸� �߰�
		strFullPath += strExtenstion;//�����̸� �߰��Ѱſ� Ȯ���ڸ� �߰�.

		_pFBXManager->GetIOPluginRegistry()->DetectReaderFileFormat(strFullPath.c_str(), iFileFormat);//������ �ִ��� Ȯ��.

		//FALSE_CHECK_MSG(_pImporter->Initialize(strFullPath.c_str(), iFileFormat), L"No Find Fbx Path");
		if (_pImporter->Initialize(strFullPath.c_str(), iFileFormat) == false)
			MessageBox(NULL, L"No Find Fbx Path", L"System Error", MB_OK);


		_pImporter->Import(_pFBXScene);
		//������ ������ fbx���� �߰��� �Ѵ�.

		CurrAxisSystem = _pFBXScene->GetGlobalSettings().GetAxisSystem();
		//fbx�� ������ ��ǥ���� �����´�.
		if (CurrAxisSystem != DestAxisSystem) DestAxisSystem.ConvertScene(_pFBXScene);
		//������ ��ǥ��� fbx���� ��ǥ��� ���Ͽ� �ٸ��� ������ ��ǥ������ �ٲ۴�.

		lGeomConverter.Triangulate(_pFBXScene, true);
		//�������� �ﰢ��ȭ �Ҽ� �ִ� ��� ��带 �ﰢ��ȭ��Ŵ.

		pFbxRootNode = _pFBXScene->GetRootNode();
		NULL_CHECK_RETURN(pFbxRootNode, E_FAIL);
		//��Ʈ��带 �������� Ȯ���۾�.

		//pAni->pBaseBoneMatrix = new XMMATRIX[BONE_MATRIX_NUM];
		pAni->pBaseBoneMatrix = (XMMATRIX*)_aligned_malloc(	sizeof(XMMATRIX) * BONE_MATRIX_NUM, 16);// ���� ����� ���*������� �����ŭ �ؼ� �ʱ�ȭ.
		//new (pAni->pBaseBoneMatrix)XMMATRIX();

		for (int i = 0; i < BONE_MATRIX_NUM; ++i)
			pAni->pBaseBoneMatrix[i] = XMMatrixIdentity();
		//�������ȭ ��Ŵ.

		pAni->pAniBuffer = new CAniBuffer;
		pAni->pAniBuffer->SetFbxBoneIndex(&(pAni->mapIndexByName), pFbxRootNode);
		//������ �����Ѵ�. �θ�-�ڽĳ�尣�� ����.

		CFbxParser::ParsingVertex(pFbxRootNode, pAni);
		//���ؽ��� uv , �븻�� �Ľ��Ѵ�.

		//pAni->pAniBuffer->CreateBuffer2();

		// Animation -----------------------
		auto AnimStack = _pFBXScene->GetSrcObject<FbxAnimStack>();
		NULL_CHECK_RETURN(AnimStack, E_FAIL);

		pAni->llAniMaxTime = AnimStack->GetLocalTimeSpan().GetDuration().GetMilliSeconds();
		//fbx���� �ִϸ��̼��� �ִ�ð��� �����´�.
		pAni->nAniNodeIdxCnt = pAni->mapIndexByName.size();

		pAni->ppAniMatrix = new XMMATRIX*[unsigned int(pAni->llAniMaxTime / 10)];
		pAni->ppResultMatrix = new XMMATRIX*[unsigned int(pAni->llAniMaxTime / 10)];

		for (long long i = 0; i < pAni->llAniMaxTime / 10; ++i)
		{
			//pAni->ppAniMatrix[i] = new XMMATRIX[pAni->nAniNodeIdxCnt];
			//pAni->ppResultMatrix[i] = new XMMATRIX[pAni->nAniNodeIdxCnt];

			pAni->ppAniMatrix[i] = (XMMATRIX*)_aligned_malloc(
				sizeof(XMMATRIX) * pAni->nAniNodeIdxCnt, 16);
			pAni->ppResultMatrix[i] = (XMMATRIX*)_aligned_malloc(
				sizeof(XMMATRIX) * pAni->nAniNodeIdxCnt, 16);
		}

		//Animation ��� ���� ����
		{
			//���� ������۸� �����Ѵ�.
			D3D11_BUFFER_DESC BD;
			::ZeroMemory(&BD, sizeof(D3D11_BUFFER_DESC));
			BD.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
			BD.ByteWidth = sizeof(VS_CB_BONE_MATRIX);
			BD.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			BD.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;

			m_pGrapicDevice->m_pDevice->CreateBuffer(&BD, NULL, &pAni->pBoneMatrixBuffer);

			m_pGrapicDevice->m_pDeviceContext->Map(pAni->pBoneMatrixBuffer,	NULL, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, NULL, &pAni->tMappedResource);//���� �Ͽ� ���۸� ����.

			pAni->pBoneMatrix = (VS_CB_BONE_MATRIX *)pAni->tMappedResource.pData;

			for (int i = 0; i < BONE_MATRIX_NUM; i++)
				pAni->pBoneMatrix->m_XMmtxBone[i] = XMMatrixIdentity();

			m_pGrapicDevice->m_pDeviceContext->Unmap(pAni->pBoneMatrixBuffer, NULL);//������ ������ ����� �Ͽ� �ݴ´�.
		}

		CFbxParser::ParsingAnimation(pFbxRootNode, pAni, AnimStack);


		//XMMATRIX ResultMtx;
		for (long long i = 0; i < pAni->llAniMaxTime / 10; ++i)
		{
			for (unsigned int j = 0; j < pAni->nAniNodeIdxCnt; ++j)
			{
				XMMATRIX ResultMtx(pAni->pBaseBoneMatrix[j] * (XMMATRIX)(pAni->ppAniMatrix[i][j]));
				pAni->ppResultMatrix[i][j] = ResultMtx;
			}
		}

		m_vecAni.push_back(pAni);
	}

	return S_OK;
}

int CDynamicMesh::Update()
{
	return 0;
}

DWORD CDynamicMesh::Release()
{
	return 0;
}

void CDynamicMesh::Release_Animation()
{
}


void CDynamicMesh::PlayAnimation(int _iIdx)
{

	if (_iIdx < 0 || (unsigned)_iIdx > m_vecAni.size())
		return;

	m_pGrapicDevice->m_pDeviceContext->RSSetState(m_pRasterizerState);

	//m_vecAni[_iIdx]->fAniPlayTimer
	m_fAniPlayTimer	+= m_vecAni[_iIdx]->fAniPlaySpeed * CTimeMgr::GetInstance()->GetTime();

	//if (m_vecAni[_iIdx]->fAniPlayTimer > m_vecAni[_iIdx]->llAniMaxTime / 10)
		//m_vecAni[_iIdx]->fAniPlayTimer = 0;

	//�ִϸ��̼� ������Ʈ üũ�ؼ� ��𼭺��� �ݺ����� �־��ش�

	/*switch (_iIdx)
	{
	case CPlayingInfo::ANI_STATE_BOOSTER:
	case CPlayingInfo::ANI_STATE_DEAD:
	case CPlayingInfo::ANI_STATE_BREAK:
		if (m_fAniPlayTimer > m_vecAni[_iIdx]->llAniMaxTime / 10) {
			m_fAniPlayTimer = (m_vecAni[_iIdx]->llAniMaxTime / 10) - 1.f;
			m_bAniEnd = true;
		}
		break;
	default:
		if (m_fAniPlayTimer > m_vecAni[_iIdx]->llAniMaxTime / 10) {
			m_fAniPlayTimer = 0;
			m_bAniEnd = true;
		}
		break;
	}*/

	if (m_fAniPlayTimer > m_vecAni[_iIdx]->llAniMaxTime/10)
	{
		m_fAniPlayTimer = 0;
		m_bAniEnd = true;
	}

	//�������

	for (unsigned int i = 0; i < m_vecAni[_iIdx]->nAniNodeIdxCnt; ++i)
	{
		m_vecAni[_iIdx]->pBoneMatrix->m_XMmtxBone[i] = m_vecAni[_iIdx]->ppResultMatrix[int(m_fAniPlayTimer)][i];
			//= m_vecAni[_iIdx]->ppResultMatrix[int(m_vecAni[_iIdx]->fAniPlayTimer)][i];
	}


	if (m_vecAni[_iIdx]->pBoneMatrixBuffer != NULL)
	{
		m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_vecAni[_iIdx]->pBoneMatrixBuffer);
	}

	m_vecAni[_iIdx]->pAniBuffer->Render();
	m_wCurrenAniIdx = _iIdx;
}


void CDynamicMesh::BWPlayAnim(int _iIdx)
{
	//�ִϸ��̼� ������Ʈ üũ�ؼ� ��𼭺��� �ݺ����� �־��ش�
	//switch (_iIdx)
	//{
	//case CPlayingInfo::ANI_STATE_BOOSTER:
	//	m_iRepeatTime = 120;
	//	break;
	//case CPlayingInfo::ANI_STATE_DEAD:
	//	m_iRepeatTime = 100;
	//	break;
	//case CPlayingInfo::ANI_STATE_BREAK:
	//	m_iRepeatTime = 50;
	//	break;
	//default:
	//	m_iRepeatTime = 0;
	//	break;
	//}

	if (_iIdx < 0 || (unsigned)_iIdx > m_vecAni.size())
		return;


	//m_pShader->Render();
	m_pGrapicDevice->m_pDeviceContext->RSSetState(m_pRasterizerState);




	//�ݴ�� �����پƤ�
	m_fAniPlayTimer
		-= m_vecAni[_iIdx]->fAniPlaySpeed * CTimeMgr::GetInstance()->GetTime();

	//AllocConsole();
	//_cprintf("%lf\n", m_fAniPlayTimer);

	if (m_fAniPlayTimer <= m_iRepeatTime) {
		m_bAniEnd = false;
		//return true;
	}

	//-----------------------------------------------------

	for (unsigned int i = 0; i < m_vecAni[_iIdx]->nAniNodeIdxCnt; ++i)
	{
		m_vecAni[_iIdx]->pBoneMatrix->m_XMmtxBone[i]
			= m_vecAni[_iIdx]->ppResultMatrix[int(m_fAniPlayTimer)][i];
	}


	if (m_vecAni[_iIdx]->pBoneMatrixBuffer != NULL)
	{
		m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_vecAni[_iIdx]->pBoneMatrixBuffer);
	}

	m_vecAni[_iIdx]->pAniBuffer->Render();
	m_wCurrenAniIdx = _iIdx;
	//return false;
}

CResources * CDynamicMesh::CloneResource()
{
	CResources* pResource = this;

	pResource->AddRef();

	return pResource;
}
