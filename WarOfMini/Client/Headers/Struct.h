#ifndef Struct_h__
#define Struct_h__

#include "Include.h"

////콘스턴트 버퍼
struct ConstantBuffer
{
	_matrix matWorld;
	_matrix matView;
	_matrix matProjection;
};


typedef struct tagBaseShader
{

	_matrix matWorld;
	_matrix matView;
	_matrix matProj;

}BASESHADERCB;

typedef struct tagInstancingShader
{
	_matrix matWorld[32];

}INSTANCINGSHADERCB;


typedef struct tagDynamicShader
{
	_matrix matBoneWorld[64];

}DYNAMICSHADERCB;


typedef struct tagIndex16
{
	_ushort			_1, _2, _3;
}INDEX16;

typedef struct tagIndex32
{
	_uint			_1, _2, _3;
}INDEX32;

typedef struct tagVertexTexture
{
	_vec3 vPos;
	_vec2 vTexUV;
	_vec3 vNormal;

}VTXTEX;

typedef struct tagVertexBone
{
	_vec3		vPos;
	_vec2		vTexUV;
	_vec3		vNormal;
	_uint		uiBones[4];
	_float		fWeights[4];

}VTXBONE;

typedef struct tagObjectInfo
{
	TCHAR			 m_szName[MAX_PATH]; //이름
	_vec3			 m_vAngle;          //회전값
	_vec3			 m_vScale;          //크기
	_vec3			 m_vPos;            //위치

}OBJ_INFO;


class CMesh;

typedef struct tagMeshData
{
	CMesh* pMesh;
	_bool  bAlpha;
	_bool  bBillboard;

}MESHDATA;


typedef struct tagClusData
{
	_vec4 R;
	_vec3 T;
	_vec3 S;

}CLUSDATA;


typedef struct Tag
{
	enum CLASSA{TESTA, TESTEND};

	enum CLASSB{ TESTB, TESTENDTWO };

}TAG;


#endif