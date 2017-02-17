// Batch.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MapTool.h"
#include "Batch.h"
#include "afxdialogex.h"
#include "Obj.h"
#include "ObjMgr.h"
#include "StaticObject.h"
#include "Info.h"
// CBatch ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CBatch, CDialog)

CBatch::CBatch(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_BATCH, pParent)
{

}

CBatch::~CBatch()
{
}

void CBatch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_OBJECT, m_ctrlTreeObject);
}


BEGIN_MESSAGE_MAP(CBatch, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_OBJECT, &CBatch::OnTvnSelchangedTreeObject)
	ON_BN_CLICKED(IDC_CREATE_BTN, &CBatch::OnBnClickedCreateBtn)
	ON_BN_CLICKED(IDC_DELETE_BTN, &CBatch::OnBnClickedDeleteBtn)
	ON_BN_CLICKED(IDC_DELETE_ALL_BTN, &CBatch::OnBnClickedDeleteAllBtn)
	ON_BN_CLICKED(IDC_SAVE_BTN, &CBatch::OnBnClickedSaveBtn)
	ON_BN_CLICKED(IDC_LOAD_BTN, &CBatch::OnBnClickedLoadBtn)
	ON_BN_CLICKED(IDC_OBJPATHBTN, &CBatch::OnBnClickedObjpathbtn)
END_MESSAGE_MAP()


// CBatch �޽��� ó�����Դϴ�.


void CBatch::OnTvnSelchangedTreeObject(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}


BOOL CBatch::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	InitTreeObject();



	return TRUE;
}

void CBatch::InitTreeObject(void)
{
	HTREEITEM hRoot;

	hRoot = m_ctrlTreeObject.InsertItem(L"Object", 0/* nImage */, 1/* nSelectedImage */, TVI_ROOT, TVI_LAST);

	//Book
	HTREEITEM  hBook;

	hBook = m_ctrlTreeObject.InsertItem(L"Book", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	HTREEITEM  hBook1;
	hBook1 = m_ctrlTreeObject.InsertItem(L"Mesh_Book1", 1/* nImage */, 1/* nSelectedImage */, hBook, TVI_LAST);

	HTREEITEM  hBook2;
	hBook2 = m_ctrlTreeObject.InsertItem(L"Mesh_Book2", 1/* nImage */, 1/* nSelectedImage */, hBook, TVI_LAST);

	HTREEITEM  hBook3;
	hBook3 = m_ctrlTreeObject.InsertItem(L"Mesh_Book3", 1/* nImage */, 1/* nSelectedImage */, hBook, TVI_LAST);

	//Book
	HTREEITEM  hBookBox;
	hBookBox = m_ctrlTreeObject.InsertItem(L"BookBox", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	HTREEITEM  hBookBox1;
	hBookBox1 = m_ctrlTreeObject.InsertItem(L"BookBox1", 1/* nImage */, 1/* nSelectedImage */, hBookBox, TVI_LAST);
	
	HTREEITEM  hBookBox2;
	hBookBox2 = m_ctrlTreeObject.InsertItem(L"BookBox2", 1/* nImage */, 1/* nSelectedImage */, hBookBox, TVI_LAST);

	//BoxFile
	HTREEITEM  hBoxFile;

	hBoxFile = m_ctrlTreeObject.InsertItem(L"BoxFile", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	HTREEITEM  hBoxFile1;
	hBoxFile1 = m_ctrlTreeObject.InsertItem(L"BoxFile1", 1/* nImage */, 1/* nSelectedImage */, hBoxFile, TVI_LAST);

	HTREEITEM  hBoxFile2;
	hBoxFile2 = m_ctrlTreeObject.InsertItem(L"BoxFile2", 1/* nImage */, 1/* nSelectedImage */, hBoxFile, TVI_LAST);

	HTREEITEM  hBoxFile3;
	hBoxFile3 = m_ctrlTreeObject.InsertItem(L"BoxFile3", 1/* nImage */, 1/* nSelectedImage */, hBoxFile, TVI_LAST);

	HTREEITEM  hBoxFile4;
	hBoxFile4 = m_ctrlTreeObject.InsertItem(L"BoxFile4", 1/* nImage */, 1/* nSelectedImage */, hBoxFile, TVI_LAST);

	HTREEITEM  hBoxFile5;
	hBoxFile5 = m_ctrlTreeObject.InsertItem(L"BoxFile5", 1/* nImage */, 1/* nSelectedImage */, hBoxFile, TVI_LAST);

	HTREEITEM  hBoxFile6;
	hBoxFile6 = m_ctrlTreeObject.InsertItem(L"BoxFile6", 1/* nImage */, 1/* nSelectedImage */, hBoxFile, TVI_LAST);

	HTREEITEM  hBoxFile7;
	hBoxFile7 = m_ctrlTreeObject.InsertItem(L"BoxFile7", 1/* nImage */, 1/* nSelectedImage */, hBoxFile, TVI_LAST);

	HTREEITEM  hBoxFile8;
	hBoxFile8 = m_ctrlTreeObject.InsertItem(L"BoxFile8", 1/* nImage */, 1/* nSelectedImage */, hBoxFile, TVI_LAST);

}

//����
void CBatch::OnBnClickedCreateBtn()
{
	UpdateData(TRUE);

	HTREEITEM	Object;

	Object = m_ctrlTreeObject.GetSelectedItem();

	CString strName = m_ctrlTreeObject.GetItemText(Object);
	wstring wstrName = (LPCTSTR)strName;

	CObj* pObj = NULL;

	pObj = CToolStaticObject::Create(wstrName);

	CObjMgr::GetInstance()->AddObject(L"StaticObject", pObj);

	UpdateData(FALSE);
}

//����
void CBatch::OnBnClickedDeleteBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

//��� ����
void CBatch::OnBnClickedDeleteAllBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

//Save
void CBatch::OnBnClickedSaveBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

//Load
void CBatch::OnBnClickedLoadBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

//Path ���
void CBatch::OnBnClickedObjpathbtn()
{
	CFileDialog	FileDlg(FALSE, L"dat", L"MapObj.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
						L"*.dat", this);

	TCHAR	szCurDir[MAX_PATH] = L"";

	GetCurrentDirectory(MAX_PATH - 1, szCurDir);

	PathRemoveFileSpec(szCurDir);
	PathRemoveFileSpec(szCurDir);

	FileDlg.m_ofn.lpstrInitialDir = szCurDir;

	CString strPathName;

	if (FileDlg.DoModal() == IDOK)
	{
		strPathName = FileDlg.GetPathName();
	}

	CString strRelativePath = ConvertRelativePath((LPCWSTR)strPathName).c_str();

	SetDlgItemText(IDC_OBJPATHSTATIC, strRelativePath);

	UpdateData(TRUE);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

wstring CBatch::ConvertRelativePath(wstring strFullPath)
{
	TCHAR	szRelativePath[MAX_PATH] = L"";
	TCHAR	szDirectoryPath[MAX_PATH] = L"";

	GetCurrentDirectory(sizeof(szDirectoryPath), szDirectoryPath);

	//�� ��ο��� �ٸ� ��η� ���� ��� ��θ� �����ϴ� ����...
	PathRelativePathTo((LPWSTR)szRelativePath, szDirectoryPath,
		FILE_ATTRIBUTE_DIRECTORY, strFullPath.c_str(),
		FILE_ATTRIBUTE_NORMAL);

	return szRelativePath;
}
