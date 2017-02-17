#ifndef ToolVIBuffer_h__
#define ToolVIBuffer_h__

#include "Resources.h"

class CVIBuffer :
	public CResources
{
public:
	CVIBuffer();
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer();

public:
	UINT				m_nInstanceBufferStride;
	UINT				m_nInstanceBufferOffset;

	ID3D11Buffer*		m_ConstantBuffer;
	ID3D11Buffer*		m_VertexBuffer;
	//ID3D11InputLayout*  m_VertexLayout;

	int					m_iVertices;
	int					m_iBuffers;
	UINT				m_iSlot;
	UINT				m_iStartVertex;
	UINT				m_iVertexStrides;
	UINT				m_iVertexOffsets;

	ID3D11Buffer*		m_IndexBuffer;

	UINT				m_iIndex;
	UINT				m_iStartIndex;
	int					m_iBaseVertex;
	DXGI_FORMAT			m_dxgiIndexFormat;
	UINT				m_iIndexOffset;

	DrawType			m_eDrawType;
	//������ ������ ������Ʈ
	ID3D11RasterizerState*		m_pRasterizerState;


	VTXTEX*				pVTXTex;

public:
	virtual void	Render(void);
	virtual DWORD	Release(void);
	virtual void	CreateRasterizerState();
	

	
	//ID3D11VertexShader*	m_pVertexShader;
	//ID3D11InputLayout*	m_pVertexLayout;


};

#endif