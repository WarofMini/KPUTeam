#pragma once
#include "Mesh.h"

struct VS_CB_BONE_MATRIX
{
	XMMATRIX m_XMmtxBone[BONE_MATRIX_NUM];// ���� ���. 128���� �Ѿ�� vtf������ ����
};

/*
//�ִϸ��̼� ���� ������ �Դϴ�.
float g_fAnimationPlayTime = 0.0f;				//�ִϸ��̼� ��� �ð�
XMMATRIX **g_pAnimationMatrix = nullptr;		//�ִϸ��̼� Matrix[�ð���][�ε�����]
XMMATRIX * g_pBaseBoneMatrix = nullptr;			//�ִϸ��̼� BaseMatrix[�ε�����]
XMMATRIX ** g_pResultMatrix = nullptr;			//�ִϸ��̼� ���� ��ȯ Matrix[�ð���][�ε�����]
long long g_llAnimationMaxTime = 0;				//�ִϸ��̼� �ִ����
unsigned int g_uiAnimationNodeIndexCount = 0;	//�ִϸ��̼� ����� ����
std::map<std::string, unsigned int> g_mIndexByName;

//�ִϸ��̼ǿ� ���õ� ��� ����
ID3D11Buffer *g_pd3dcbBoneMatrix = nullptr;
D3D11_MAPPED_SUBRESOURCE g_d3dMappedResource;
VS_CB_BONE_MATRIX *g_pcbBoneMatrix = nullptr;
*/



class CAniBuffer;
struct Animation
{
	string			strAniName;// �ִϸ��̼��� �̸�. �� ����� fbx�� Ȯ���ڸ� ������ ���� �̸�.
	CAniBuffer*		pAniBuffer;//�θ� �� �ڽĳ��� ����.

	long long		llAniMaxTime;//�ִϸ��̼��� �ִ�ð�
								 //float			fAniPlayTimer;
	float			fAniPlaySpeed;//�ִϸ��̼��� �ӵ�

	XMMATRIX**		ppAniMatrix;//�ִϸŴϼ��� ���
	XMMATRIX*		pBaseBoneMatrix;//���� ���
	XMMATRIX**		ppResultMatrix;//���� ���

	unsigned int	nAniNodeIdxCnt;//����� �ε���ī��Ʈ
	map<string, unsigned int>	mapIndexByName;


	ID3D11Buffer*				pBoneMatrixBuffer;//���� ������Ʈ����
	D3D11_MAPPED_SUBRESOURCE	tMappedResource;//���μ��긮�ҽ�
	VS_CB_BONE_MATRIX*			pBoneMatrix;//���� ����� ���� �ִ� ����ü

	Animation()
		: pAniBuffer(NULL)
		, llAniMaxTime(0)
		//, fAniPlayTimer(0.f)
		, fAniPlaySpeed(100.f)

		, ppAniMatrix(NULL)
		, pBaseBoneMatrix(NULL)
		, ppResultMatrix(NULL)

		, nAniNodeIdxCnt(0)
		, pBoneMatrixBuffer(NULL)
		, pBoneMatrix(NULL)
	{}
};


class CDynamicMesh :public CMesh
{
public:
	CDynamicMesh();
	virtual ~CDynamicMesh();

private:
	WORD					m_wCurrenAniIdx;//���� �ִ��ε���
	vector<Animation*>		m_vecAni;//�ִϸ��̼� ����ä�� ��� ����
	int						m_iRepeatTime;//�ִϸ��̼��� �ݺ��� Ƚ��

public:
	static CDynamicMesh*		Create(const char* _pPath, vector<string> _vecAniName);
	virtual CResources*			CloneResource();
	float					m_fAniPlayTimer;//���� ����ϰ� �ִ� �ִϸ��̼��� �ð�
	bool					m_bAniEnd;//�ִϸ��̼��� ���������� ���θ� �˷��ִ� Bool��

protected:
	virtual HRESULT		Load_Model(const char*, vector<string> _vecAniName, FbxManager*, FbxIOSettings*, FbxScene*, FbxImporter*);


public:
	virtual HRESULT		Initialize(const char* _pPath, vector<string> _vecAniName);
public:
	virtual int		Update();

private:
	virtual DWORD Release();
	void Release_Animation();


public:
	void		PlayAnimation(int _iIdx);
	void		BWPlayAnim(int _iIdx);


	// CMesh��(��) ���� ��ӵ�
	//virtual CResources * CloneResource(void) override;

};

