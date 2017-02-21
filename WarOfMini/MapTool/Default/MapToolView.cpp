
// MapToolView.cpp : CMapToolView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
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
	// 표준 인쇄 명령입니다.
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

// CMapToolView 생성/소멸

CMapToolView::CMapToolView()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_pToolApp = NULL;
	m_pBatch = NULL;
}

CMapToolView::~CMapToolView()
{
	::Safe_Release(m_pToolApp);
}

BOOL CMapToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CMapToolView 그리기
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

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CMapToolView 인쇄


void CMapToolView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMapToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CMapToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CMapToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
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


// CMapToolView 진단

#ifdef _DEBUG
void CMapToolView::AssertValid() const
{
	CView::AssertValid();
}

void CMapToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMapToolDoc* CMapToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMapToolDoc)));
	return (CMapToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CMapToolView 메시지 처리기


void CMapToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	g_hWnd = m_hWnd;
	g_hInst = AfxGetInstanceHandle();

	m_pToolApp = CToolApp::Create();


	CMainFrame*	pFrame = (CMainFrame*)AfxGetMainWnd();

	RECT	rcWindow;
	pFrame->GetWindowRect(&rcWindow);	// 전체 프레임의 사이즈를 얻어오는 함수

	SetRect(&rcWindow, 0, 0,
		rcWindow.right - rcWindow.left,
		rcWindow.bottom - rcWindow.top);

	RECT	rcMainView;

	// 뷰 창의 크기를 얻어오는 함수
	GetClientRect(&rcMainView);

	float fRowFrm = float(rcWindow.right - rcMainView.right);// 가로 프레임의 길이
	float fColFrm = float(rcWindow.bottom - rcMainView.bottom);// 세로 프레임의 길이

	pFrame->SetWindowPos(NULL, 0, 0,
		int(WINCX + fRowFrm),
		int(WINCY + fColFrm),
		SWP_NOZORDER);


	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}

//Mouse Move
void CMapToolView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnLButtonDown(nFlags, point);
}


void CMapToolView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnLButtonUp(nFlags, point);
}


void CMapToolView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	g_bRBtnCheck = true;

	CView::OnRButtonDown(nFlags, point);
}
