#ifndef MouseCol_h__
#define MouseCol_h__

class CMesh;
class CVIBuffer;

class CMouseCol
{
private:
	CMouseCol(void);

public:
	virtual ~CMouseCol(void);

public:
	static POINT	  GetMousePos(void);
	static CMouseCol* Create(void);

public:
	HRESULT		Initialize(void);
	void		Translation_ViewSpace(void);
	void		Translation_Local(const D3DXMATRIX* pWorld);
	
	bool		PickObjMesh(CMesh* pMesh, D3DXMATRIX* pMatWorld);

	bool		PickBoundingBox(CVIBuffer* pBuffer, D3DXMATRIX* pMatWorld);
	
	void		Release(void);

private:
	D3DXVECTOR3			m_vPivotPos;
	D3DXVECTOR3			m_vRayDir;
	D3DXVECTOR3			m_vCamEye;
	float				m_fTime;

};




#endif //MouseCol_h__
