// MyForm.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MapTool.h"
#include "MyForm.h"


// CMyForm

IMPLEMENT_DYNCREATE(CMyForm, CFormView)

CMyForm::CMyForm()
	: CFormView(IDD_MYFORM)
{
	m_iCurrentViewDIg = 0;
	m_eToolMode = BATCH_TOOL;
}

CMyForm::~CMyForm()
{
}

void CMyForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TABSHEET, m_TapControl);
}

BEGIN_MESSAGE_MAP(CMyForm, CFormView)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABSHEET, &CMyForm::OnTcnSelchangeTabsheet)
END_MESSAGE_MAP()


// CMyForm �����Դϴ�.

#ifdef _DEBUG
void CMyForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMyForm �޽��� ó�����Դϴ�.


void CMyForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	m_TapControl.DeleteAllItems();


	m_TapControl.InsertItem(0, L"Batch");
	m_TapControl.InsertItem(1, L"Navi");

	RECT	rcToolWindow;
	m_TapControl.GetClientRect(&rcToolWindow);
	
	m_pBatchDIg.Create(IDD_BATCH);
	m_pBatchDIg.SetWindowPos(NULL, 10, 95, rcToolWindow.right - 10,
							rcToolWindow.bottom - 35, SWP_SHOWWINDOW);

	m_pNaviDIg.Create(IDD_NAVIDIG);
	m_pNaviDIg.SetWindowPos(NULL, 10, 95, rcToolWindow.right - 10,
							rcToolWindow.bottom - 35, SWP_SHOWWINDOW);


	m_pBatchDIg.ShowWindow(SW_SHOW);
	m_pNaviDIg.ShowWindow(SW_HIDE);
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
}


void CMyForm::OnTcnSelchangeTabsheet(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int iIndex = m_TapControl.GetCurSel();

	m_iCurrentViewDIg = iIndex;

	switch (m_iCurrentViewDIg)
	{
	case 0:
		m_pBatchDIg.ShowWindow(SW_SHOW);
		m_pNaviDIg.ShowWindow(SW_HIDE);

		m_eToolMode = BATCH_TOOL;
		break;

	case 1:
		m_pBatchDIg.ShowWindow(SW_HIDE);
		m_pNaviDIg.ShowWindow(SW_SHOW);

		m_eToolMode = NAVI_TOOL;
		break;
	}

}
