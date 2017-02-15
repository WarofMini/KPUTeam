#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CBatch ��ȭ �����Դϴ�.

class CBatch : public CDialog
{
	DECLARE_DYNAMIC(CBatch)

public:
	CBatch(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CBatch();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BATCH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_ctrlTreeObject;
	afx_msg void OnTvnSelchangedTreeObject(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();

public:
	void		 InitTreeObject(void);
	afx_msg void OnBnClickedCreateBtn();
	afx_msg void OnBnClickedDeleteBtn();
	afx_msg void OnBnClickedDeleteAllBtn();
	afx_msg void OnBnClickedSaveBtn();
	afx_msg void OnBnClickedLoadBtn();
	afx_msg void OnBnClickedObjpathbtn();

	wstring ConvertRelativePath(wstring	strFullPath);
};
