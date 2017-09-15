#ifndef Font_h__
#define Font_h__

#include "Include.h"
#include "FW1FontWrapper.h"

class CFont
{
private:
	explicit CFont(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);
	~CFont(void);

public:
	HRESULT Ready_Font(const _tchar* pFontName);
	void	Render_Font(const _tchar* pString, _float fSize, _float fPosX, _float fPosY, _uint uiColor);


public:
	static CFont* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, const _tchar* pFontName);

public:
	void Release(void);


private:
	ID3D11Device*			m_pGraphicDev;
	ID3D11DeviceContext*	m_pContext;

	IFW1Factory*			m_pFW1Factory;
	IFW1FontWrapper*		m_pFontWrapper;
};



#endif