// NaviDIg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "NaviDIg.h"
#include "afxdialogex.h"


// CNaviDIg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CNaviDIg, CDialog)

CNaviDIg::CNaviDIg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_NAVIDIG, pParent)
{

}

CNaviDIg::~CNaviDIg()
{
}

void CNaviDIg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNaviDIg, CDialog)
END_MESSAGE_MAP()


// CNaviDIg 메시지 처리기입니다.
