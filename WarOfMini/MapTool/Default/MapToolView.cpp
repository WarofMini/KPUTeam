
// MapToolView.cpp : CMapToolView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "MapTool.h"
#endif

#include "MapToolDoc.h"
#include "MapToolView.h"
#include "ToolApp.h"
#include "MyForm.h"
#include "MainFrm.h"
#include "Batch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMapToolView

IMPLEMENT_DYNCREATE(CMapToolView, CView)

BEGIN_MESSAGE_MAP(CMapToolView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMapToolView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

// CMapToolView ����/�Ҹ�

CMapToolView::CMapToolView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	m_pToolApp = NULL;
	m_pBatch = NULL;
}

CMapToolView::~CMapToolView()
{
	::Safe_Release(m_pToolApp);
}

BOOL CMapToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CMapToolView �׸���
void CMapToolView::OnDraw(CDC* /*pDC*/)
{
	CMapToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	m_pToolApp->Update();
	m_pToolApp->Render();



	if( ((CMainFrame*)AfxGetMainWnd())->m_pMyForm->m_eToolMode == CMyForm::BATCH_TOOL)
	{
		if (m_pBatch == NULL)
		{
			m_pBatch = &((CMainFrame*)AfxGetMainWnd())->m_pMyForm->m_pBatchDIg;
		}

		m_pBatch->Update();

	}

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CMapToolView �μ�


void CMapToolView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMapToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CMapToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CMapToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}

void CMapToolView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	g_bRBtnCheck = false;

	//ClientToScreen(&point);
	//OnContextMenu(this, point);
}

void CMapToolView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMapToolView ����

#ifdef _DEBUG
void CMapToolView::AssertValid() const
{
	CView::AssertValid();
}

void CMapToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMapToolDoc* CMapToolView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMapToolDoc)));
	return (CMapToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CMapToolView �޽��� ó����


void CMapToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	g_hWnd = m_hWnd;
	g_hInst = AfxGetInstanceHandle();

	m_pToolApp = CToolApp::Create();


	CMainFrame*	pFrame = (CMainFrame*)AfxGetMainWnd();

	RECT	rcWindow;
	pFrame->GetWindowRect(&rcWindow);	// ��ü �������� ����� ������ �Լ�

	SetRect(&rcWindow, 0, 0,
		rcWindow.right - rcWindow.left,
		rcWindow.bottom - rcWindow.top);

	RECT	rcMainView;

	// �� â�� ũ�⸦ ������ �Լ�
	GetClientRect(&rcMainView);

	float fRowFrm = float(rcWindow.right - rcMainView.right);// ���� �������� ����
	float fColFrm = float(rcWindow.bottom - rcMainView.bottom);// ���� �������� ����

	pFrame->SetWindowPos(NULL, 0, 0,
		int(WINCX + fRowFrm),
		int(WINCY + fColFrm),
		SWP_NOZORDER);


	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
}

//Mouse Move
void CMapToolView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CView::OnMouseMove(nFlags, point);

	g_bRBtnCheck = false;

	g_vMouse = D3DXVECTOR3( (float)point.x, (float)point.y, 0.f);


	//if( ((CMainFrame*)AfxGetMainWnd())->m_pMyForm->m_eToolMode == CMyForm::BATCH_TOOL)
	//{
	//}
		CBatch* 	pDIg = &((CMainFrame*)AfxGetMainWnd())->m_pMyForm->m_pBatchDIg;

		pDIg->SetDlgItemInt(IDC_EDIT_MOUSEX, int(g_vViewMouse.x));
		pDIg->SetDlgItemInt(IDC_EDIT_MOUSEY, int(g_vViewMouse.y));
		pDIg->SetDlgItemInt(IDC_EDIT_MOUSEZ, int(g_vViewMouse.z));

}

//L Btn Down
void CMapToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CView::OnLButtonDown(nFlags, point);
}


void CMapToolView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CView::OnLButtonUp(nFlags, point);
}


void CMapToolView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	g_bRBtnCheck = true;

	CView::OnRButtonDown(nFlags, point);
}
