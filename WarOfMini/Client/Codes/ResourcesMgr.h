#ifndef ResourcesMgr_h__
#define ResourcesMgr_h__

#include "Include.h"
#include "RcTex.h"
#include "CubeTex.h"
#include "FlagTex.h"
#include "TrailTex.h"
#include "StaticMesh.h"
#include "DynamicMesh.h"
#include "Texture.h"

class CResourcesMgr
{
	DECLARE_SINGLETON(CResourcesMgr)

public:
	enum BUFFERTYPE { BUFFER_RCTEX, BUFFER_CUBE, BUFFER_FLAGTEX, BUFFER_TRAILTEX, BUFFER_END };
	enum MESHTYPE { MESH_STATIC, MESH_DYNAMIC, MESH_END };

private:
	CResourcesMgr(void);
	~CResourcesMgr(void);

public:
	CResource* Clone_ResourceMgr(const _ushort& wContainerIdx, const _tchar* pResourceKey);
	HRESULT Reserve_ContainerSize(const _ushort& wSize);

public:
	HRESULT Ready_Buffer(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext
		, const _ushort& wContainerIdx, BUFFERTYPE eBufferType, const _tchar* pResourceKey
		, const _ushort& wCntX = 0, const _ushort& wCntZ = 0, const _ushort& wItvX = 0, const _ushort& wItvZ = 0);

	HRESULT Ready_Texture(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext
		, const _ushort& wContainerIdx, const _tchar* pResourceKey, CTextures::TEXTURETYPE eTextureType
		, const _tchar* pFilePath, const _ushort& wCnt = 0);

	HRESULT Ready_Mesh(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext
		, const _ushort& wContainerIdx, MESHTYPE eMeshType, const _tchar* pResourceKey, const _tchar* pFilePath);

private:
	void Ready_FbxSdkMgr(void);
	void Load_StaticMesh(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, FbxNode* pNode, CStaticMesh* pParent);
	void Load_DynamicMesh(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, FbxNode* pNode, CDynaicMesh* pParent
		, FbxScene* pFbxScene, FbxArray<FbxString*>& arrAniName, vector<pair<_ushort, _ushort>>& vecFrameCnt);
			CResource* Find_Resource(const _ushort& wContainerIdx, const _tchar* pResourceKey);

private:
	void Load_FrameData(const _tchar* pResourceKey, vector<pair<_ushort, _ushort>>& vecFrameCnt);
	FbxAMatrix GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose = NULL, FbxAMatrix* pParentGlobalPosition = NULL);
	FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex);
	FbxAMatrix GetGeometry(FbxNode* pNode);

public:
	void Reset_Resource(const _ushort& wContainerIdx);
	void Release(void);

private:

	typedef map<const _tchar*, CResource*>  MAPRESOURCE;
	MAPRESOURCE*						m_pmapResource;
	_ushort								m_wReservedSize;

private:
	CResource*		m_pRootMeshStore;
	FbxManager*		m_pFbxSdkMgr;

private:
	list<wstring*> m_ResourceNameList;


public:
	map<string, unsigned int>	mapIndexByName;

	void		SetFbxBoneIndex(map<std::string, unsigned int>* _pIndexByName, FbxNode* _pNode);

};

#endif // ResourceMgr_h__