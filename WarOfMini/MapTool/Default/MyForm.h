#pragma once

#include "afxcmn.h"
#include "Batch.h"
#include "NaviDIg.h"

// CMyForm �� ���Դϴ�.

class CMyForm : public CFormView
{
	DECLARE_DYNCREATE(CMyForm)

protected:
	CMyForm();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CMyForm();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYFORM };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	enum  TOOLMODE {BATCH_TOOL, NAVI_TOOL, TOOL_END};

public:
	CBatch			m_pBatchDIg;
	CNaviDIg		m_pNaviDIg;
	CTabCtrl		m_TapControl;

	TOOLMODE		m_eToolMode;

	int				m_iCurrentViewDIg;

	virtual void OnInitialUpdate();
	afx_msg void OnTcnSelchangeTabsheet(NMHDR *pNMHDR, LRESULT *pResult);
};


