#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CNaviDIg ��ȭ �����Դϴ�.

class CNaviDIg : public CDialog
{
	DECLARE_DYNAMIC(CNaviDIg)

public:
	CNaviDIg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CNaviDIg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NAVIDIG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
