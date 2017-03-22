#include "stdafx.h"
#include "Loading.h"
#include "Include.h"
#include <process.h>
#include "ResourcesMgr.h"
#include "Scene.h"
#include "MeshMgr.h"
#include "GraphicDev.h"

CLoading::CLoading(LOADINGID eLoadID)
: m_eLoadID(eLoadID)
, m_hThread(NULL)
, m_bComplete(false)
, m_bServerConnected(false)
, pGraphicDev(NULL)
, pContext(NULL)
{
	ZeroMemory(m_szLoadMessage, sizeof(TCHAR) * 128);
	ZeroMemory(&m_CSKey, sizeof(CRITICAL_SECTION));

	// Graphic Device
	pGraphicDev = CGraphicDev::GetInstance()->GetGraphicDevice();
	pContext	= CGraphicDev::GetInstance()->GetContext();

}

CLoading::~CLoading(void)
{

}

bool CLoading::GetComplete(void)
{
	return m_bComplete;
}

const TCHAR* CLoading::GetLoadMessage(void)
{
	return m_szLoadMessage;
}

HRESULT CLoading::InitLoading(void)
{
	InitializeCriticalSection(&m_CSKey);

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, LoadingFunction, this, 0, NULL);

	if (m_hThread == NULL)
		return E_FAIL;

	return S_OK;
}

void CLoading::LogoLoading(void)
{
	//Texture Loading ,tga, dds 모두 사용가능
	cout << "Texture Loading~~" << endl;
	Ready_TextureFromFile(pGraphicDev, pContext);

	//DynamicMesh Loading
	cout << "DynamicMesh Loading~~" << endl;
	Ready_DynamicMeshFromFile(pGraphicDev, pContext);

	//StaticMesh Loading
	cout << "StaticMesh Loading~~" << endl;
	Ready_StaticMeshFromFile(pGraphicDev, pContext);


	cout << "Thread Logo Loading End" << endl;
	m_bComplete = true;
	m_bLogoLoading = TRUE;
}

void CLoading::StageLoading(void)
{


	cout << "Thread Stage Loading End" << endl;
	m_bComplete = true;
}


CLoading* CLoading::Create(LOADINGID eLoadID)
{
	CLoading*	pLoading = new CLoading(eLoadID);
	
	if (FAILED(pLoading->InitLoading()))
		Safe_Release(pLoading);
	
	return pLoading;
}

UINT WINAPI CLoading::LoadingFunction(void* pArg)
{
	CLoading*		pLoading = (CLoading*)pArg;

	switch (pLoading->m_eLoadID)
	{
	case CLoading::LOADING_LOGO:
		pLoading->LogoLoading();
		break;

	case CLoading::LOADING_STAGE:
		pLoading->StageLoading();
		break;
	}
	return 0;
}

void CLoading::Release(void)
{
	DeleteCriticalSection(&m_CSKey);
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);

	delete this;
}

void CLoading::Ready_TextureFromFile(ID3D11Device * pGraphicDev, ID3D11DeviceContext * pContext)
{
	//텍스쳐이름-확장자-개수(0 단독 텍스쳐, n 여러장 텍스쳐 한번에 로드 시)
	wstring wstrPath = L"../Bin/Data/TextureInfomation.txt";

	wifstream inFile;
	inFile.open(wstrPath.c_str(), ios::in);

	_tchar pFileName[MAX_NAME];
	_tchar pFileType[4];
	_tchar pFileNum[4];

	while (!inFile.eof())
	{
		inFile.getline(pFileName, MAX_NAME, '-');
		inFile.getline(pFileType, 4, '-');
		inFile.getline(pFileNum, 4);

		_ushort wTextureCnt = _wtoi(pFileNum);

		wstring wstrTextureKey = L"Texture_";
		wstrTextureKey += pFileName;

		wstring wstrTexturePath = L"../Bin/Resources/Texture/";
		wstrTexturePath += pFileName;
		if (wTextureCnt) wstrTexturePath += L"%d.";
		else wstrTexturePath += L".";
		wstrTexturePath += pFileType;

		CTextures::TEXTURETYPE eTextureType = CTextures::TYPE_NORMAL;

		if (!lstrcmp(pFileType, L"tga"))
			eTextureType = CTextures::TYPE_TGA;

		else if (!lstrcmp(pFileType, L"dds"))
			eTextureType = CTextures::TYPE_DDSCUBE;

		CResourcesMgr::GetInstance()->Ready_Texture(pGraphicDev, pContext, RESOURCE_STAGE, wstrTextureKey.c_str(), eTextureType, wstrTexturePath.c_str(), wTextureCnt);
	}

	inFile.close();
}

void CLoading::Ready_DynamicMeshFromFile(ID3D11Device * pGraphicDev, ID3D11DeviceContext * pContext)
{
	wstring wstrPath = L"../Bin/Data/DynamicMeshInformation.txt";

	wifstream inFile;
	inFile.open(wstrPath.c_str(), ios::in);

	_tchar pFileName[MAX_NAME];
	_tchar pFileAlpha[2];
	_tchar pFileBillboard[2];

	_ushort wAlpha, wBillboard;
	MESHDATA* pMeshData = NULL;

	while (!inFile.eof())
	{
		inFile.getline(pFileName, MAX_NAME, '-');
		inFile.getline(pFileAlpha, 2, '-');
		inFile.getline(pFileBillboard, 2);

		//문자열을 int형으로 바꿔준다.
		wAlpha = _wtoi(pFileAlpha);
		wBillboard = _wtoi(pFileBillboard);


		wstring wstrMeshKey = L"Mesh_";
		wstrMeshKey += pFileName;

		wstring wstrMeshPath = L"../Bin/Resources/DynamicMesh/";
		wstrMeshPath += pFileName;
		wstrMeshPath += L".FBX";

		//메시 준비
		CResourcesMgr::GetInstance()->Ready_Mesh(pGraphicDev, pContext, RESOURCE_STAGE, CResourcesMgr::MESH_DYNAMIC, wstrMeshKey.c_str(), wstrMeshPath.c_str());

		pMeshData = new MESHDATA;

		pMeshData->pMesh = (CMesh*)CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, wstrMeshKey.c_str());
		pMeshData->bAlpha = (_bool)wAlpha;
		pMeshData->bBillboard = (_bool)wBillboard;


		//MeshMgr에 pMeshData의 정보를 넣어논다.
		CMeshMgr::GetInstance()->Ready_MeshMgr(pMeshData);
	}

	inFile.close();
}

void CLoading::Ready_StaticMeshFromFile(ID3D11Device * pGraphicDev, ID3D11DeviceContext * pContext)
{
	// Static Mesh
	//메시이름 - 알파값유무 - 빌보드(0 없음, 1있음)
	wstring wstrPath = L"../Bin/Data/StaticMeshInformation.txt";

	wifstream inFile;
	inFile.open(wstrPath.c_str(), ios::in);

	_tchar pFileName[MAX_NAME];
	_tchar pFileAlpha[2];
	_tchar pFileBillboard[2];

	WORD wAlpha, wBillboard;
	MESHDATA* pMeshData = NULL;

	while (!inFile.eof())
	{
		inFile.getline(pFileName, MAX_NAME, '-');
		inFile.getline(pFileAlpha, 2, '-');
		inFile.getline(pFileBillboard, 2);

		wAlpha = _wtoi(pFileAlpha);
		wBillboard = _wtoi(pFileBillboard);

		wstring wstrMeshKey = L"Mesh_";
		wstrMeshKey += pFileName;

		wstring wstrMeshPath = L"../Bin/Resources/StaticMesh/Mesh/";
		wstrMeshPath += pFileName;
		wstrMeshPath += L".FBX";

		CResourcesMgr::GetInstance()->Ready_Mesh(pGraphicDev, pContext, RESOURCE_STAGE, CResourcesMgr::MESH_STATIC, wstrMeshKey.c_str(), wstrMeshPath.c_str());

		pMeshData = new MESHDATA;

		pMeshData->pMesh = (CMesh*)CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, wstrMeshKey.c_str());
		pMeshData->bAlpha = (_bool)wAlpha;
		pMeshData->bBillboard = (_bool)wBillboard;

		CMeshMgr::GetInstance()->Ready_MeshMgr(pMeshData);
	}

	inFile.close();
}
