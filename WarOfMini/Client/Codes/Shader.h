#pragma once
#include "Component.h"

class CShader : public CComponent
{
private:
	explicit CShader();
	explicit CShader(const CShader& rhs);
public:
	virtual ~CShader();

public:
	ID3D11VertexShader*     m_pVertexShader;
	ID3D11PixelShader*      m_pPixelShader; // �� ���� LPD3DXEFFECT�� ������ �ɰ����� �����̴�
	ID3D11InputLayout*      m_pVertexLayout; // ���̷�Ʈ9�� FVF�� ����Ѵ�

private:
	DWORD m_dwRefCount;

private:
	HRESULT Ready_ShaderFile(wstring wstrFilePath, LPCSTR wstrShaderName, LPCSTR wstrShaderVersion, ShaderType _SType);
	


public:
	static CShader*	Create(wstring wstrFilePath, LPCSTR wstrShaderName, LPCSTR wstrShaderVersion, ShaderType _SType);
	CShader*	CloneShader(void);
	DWORD	Release(void);


};

