#ifndef ShaderMgr_h__
#define ShaderMgr_h__

#include "Include.h"

class CShader;
class CShaderMgr
{
	DECLARE_SINGLETON(CShaderMgr)

public:
	enum LAYOUT_FLAG { LAYOUT_FLAG_BONE = 0x01 };

private:
	explicit CShaderMgr(void);
	~CShaderMgr(void);

public:
	ID3D11VertexShader* Get_VertexShader(const _tchar* pShaderTag);
	ID3D11InputLayout* Get_InputLayout(const _tchar* pShaderTag);
	ID3D11PixelShader* Get_PixelShader(const _tchar* pShaderTag);

public:
	HRESULT Ready_ShaderFromFiles(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext
		, const _tchar* pShaderTag, _tchar* szFileNamebyLayoutFlag, _ubyte byLayoutFlag);

private:
	CShader* Find_Shader(const _tchar* pShaderTag);

public:
	void Release(void);

private:
	using MAPSHADERS = map<const _tchar*, CShader*>;
	MAPSHADERS	m_mapShaders;
};

#endif // ShaderMgr_h__
