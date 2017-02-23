#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CBatch 대화 상자입니다.

class CBatch : public CDialog
{
	DECLARE_DYNAMIC(CBatch)

public:
	CBatch(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CBatch();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BATCH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_ctrlTreeObject;
	afx_msg void OnTvnSelchangedTreeObject(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();

public:
	BOOL						bStartReSize;
	int							nYOrgHeight;


public:
	void			InitSlider(void);
	void			Update(void);

public:
	void		 InitTreeObject(void);
	afx_msg void OnBnClickedCreateBtn();
	afx_msg void OnBnClickedDeleteBtn();
	afx_msg void OnBnClickedDeleteAllBtn();
	afx_msg void OnBnClickedSaveBtn();
	afx_msg void OnBnClickedLoadBtn();
	afx_msg void OnBnClickedObjpathbtn();

	wstring ConvertRelativePath(wstring	strFullPath);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CSliderCtrl m_ctrObjXSlider;
	CSliderCtrl m_ctrObjYSlider;
	CSliderCtrl m_ctrObjZSlider;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	afx_msg void OnEnChangeEditObjX();
	afx_msg void OnEnChangeEditObjY();
	afx_msg void OnEnChangeEditObjZ();
	CSliderCtrl m_ctrObjRotXSlider;
	CSliderCtrl m_ctrObjRotZSlider;
	CSliderCtrl m_ctrObjRotYSlider;
};
