#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CNaviDIg 대화 상자입니다.

class CNaviDIg : public CDialog
{
	DECLARE_DYNAMIC(CNaviDIg)

public:
	CNaviDIg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CNaviDIg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NAVIDIG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
