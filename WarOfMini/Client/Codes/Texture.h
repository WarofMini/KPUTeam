#ifndef Texture_h__
#define Texture_h__

#include "Resources.h"

class CTextures :
	public CResource
{
public:
	enum TEXTURETYPE { TYPE_NORMAL, TYPE_TGA, TYPE_DDSCUBE, TYPE_END };

private:
	explicit CTextures(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);
	virtual ~CTextures(void);

private:
	typedef  vector<ID3D11ShaderResourceView*>		VECTEXTURE;
	VECTEXTURE										m_vecTexture;
	size_t											m_sizetContainerSize;

public:
	static CTextures* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext
		, TEXTURETYPE eTextureType, const _tchar* pFilePath, const _ushort& wCnt);

private:
	CTextures* Clone_Resource(void);
	HRESULT Create_Texture(TEXTURETYPE eTextureType, const _tchar* pFilePath, const _ushort& wCnt);

public:
	void Render(const _uint uiSlot, const _ulong& dwIndex);
	void Release(void);

};


#endif
