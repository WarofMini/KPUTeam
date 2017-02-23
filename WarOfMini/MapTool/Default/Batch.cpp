// Batch.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "Batch.h"
#include "afxdialogex.h"
#include "Obj.h"
#include "ObjMgr.h"
#include "StaticObject.h"
#include "Info.h"
#include "ObjCol.h"
#include "Obj.h"
#include "StaticObject.h"
// CBatch 대화 상자입니다.

IMPLEMENT_DYNAMIC(CBatch, CDialog)

CBatch::CBatch(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_BATCH, pParent)
{
	bStartReSize = TRUE;
	nYOrgHeight = 0;
}

CBatch::~CBatch()
{
}

void CBatch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_OBJECT, m_ctrlTreeObject);
	DDX_Control(pDX, IDC_SLIDER_OBJ_X, m_ctrObjXSlider);
	DDX_Control(pDX, IDC_SLIDER_OBJ_Y, m_ctrObjYSlider);
	DDX_Control(pDX, IDC_SLIDER_OBJ_Z, m_ctrObjZSlider);
	DDX_Control(pDX, IDC_SLIDER_OBJ_RotX, m_ctrObjRotXSlider);
	DDX_Control(pDX, IDC_SLIDER_OBJ_RotZ, m_ctrObjRotZSlider);
	DDX_Control(pDX, IDC_SLIDER_OBJ_RotY, m_ctrObjRotYSlider);
}


BEGIN_MESSAGE_MAP(CBatch, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_OBJECT, &CBatch::OnTvnSelchangedTreeObject)
	ON_BN_CLICKED(IDC_CREATE_BTN, &CBatch::OnBnClickedCreateBtn)
	ON_BN_CLICKED(IDC_DELETE_BTN, &CBatch::OnBnClickedDeleteBtn)
	ON_BN_CLICKED(IDC_DELETE_ALL_BTN, &CBatch::OnBnClickedDeleteAllBtn)
	ON_BN_CLICKED(IDC_SAVE_BTN, &CBatch::OnBnClickedSaveBtn)
	ON_BN_CLICKED(IDC_LOAD_BTN, &CBatch::OnBnClickedLoadBtn)
	ON_BN_CLICKED(IDC_OBJPATHBTN, &CBatch::OnBnClickedObjpathbtn)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_OBJ_X, &CBatch::OnEnChangeEditObjX)
	ON_EN_CHANGE(IDC_EDIT_OBJ_Y, &CBatch::OnEnChangeEditObjY)
	ON_EN_CHANGE(IDC_EDIT_OBJ_Z, &CBatch::OnEnChangeEditObjZ)
END_MESSAGE_MAP()


// CBatch 메시지 처리기입니다.


void CBatch::OnTvnSelchangedTreeObject(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


BOOL CBatch::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	InitTreeObject();

	//슬라이더 초기정보 셋팅
	InitSlider();

	return TRUE;
}

void CBatch::InitSlider(void)
{
	//X 슬라이더
	//범위
	m_ctrObjXSlider.SetRange(-5000, 5000);
	m_ctrObjXSlider.SetRangeMin(-5000);
	m_ctrObjXSlider.SetRangeMax(5000);
	m_ctrObjXSlider.SetPos(0);

	//눈금 간격
	m_ctrObjXSlider.SetTicFreq(1);
	m_ctrObjXSlider.SetLineSize(1);

	//Y 슬라이더
	//범위
	m_ctrObjYSlider.SetRange(-5000, 5000);
	m_ctrObjYSlider.SetRangeMin(-5000);
	m_ctrObjYSlider.SetRangeMax(5000);
	m_ctrObjYSlider.SetPos(0);

	//눈금 간격
	m_ctrObjYSlider.SetTicFreq(1);
	m_ctrObjYSlider.SetLineSize(1);


	//Z 슬라이더
	//범위
	m_ctrObjZSlider.SetRange(-5000, 5000);
	m_ctrObjZSlider.SetRangeMin(-5000);
	m_ctrObjZSlider.SetRangeMax(5000);
	m_ctrObjZSlider.SetPos(0);

	//눈금 간격
	m_ctrObjZSlider.SetTicFreq(1);
	//키보드 커서키로 슬라이더 움직일떄 증가 크기 설정
	m_ctrObjZSlider.SetLineSize(1);


	//RotX
	m_ctrObjRotXSlider.SetRange(0, 360);
	m_ctrObjRotXSlider.SetRangeMin(0);
	m_ctrObjRotXSlider.SetRangeMax(360);
	m_ctrObjRotXSlider.SetPos(0);

	//눈금 간격
	m_ctrObjRotXSlider.SetTicFreq(1);
	//키보드 커서키로 슬라이더 움직일떄 증가 크기 설정
	m_ctrObjRotXSlider.SetLineSize(2);



	//RotY
	m_ctrObjRotYSlider.SetRange(0, 360);
	m_ctrObjRotYSlider.SetRangeMin(0);
	m_ctrObjRotYSlider.SetRangeMax(360);
	m_ctrObjRotYSlider.SetPos(0);

	//눈금 간격
	m_ctrObjRotYSlider.SetTicFreq(1);
	//키보드 커서키로 슬라이더 움직일떄 증가 크기 설정
	m_ctrObjRotYSlider.SetLineSize(2);

	//RotZ
	m_ctrObjRotZSlider.SetRange(0, 360);
	m_ctrObjRotZSlider.SetRangeMin(0);
	m_ctrObjRotZSlider.SetRangeMax(360);
	m_ctrObjRotZSlider.SetPos(0);

	//눈금 간격
	m_ctrObjRotZSlider.SetTicFreq(1);
	//키보드 커서키로 슬라이더 움직일떄 증가 크기 설정
	m_ctrObjRotZSlider.SetLineSize(2);



	float fZero = 0.0f;
	CString str;

	str.Format(L"%.1f", fZero);

	GetDlgItem(IDC_EDIT_OBJ_X)->SetWindowText(str);
	GetDlgItem(IDC_EDIT_OBJ_Y)->SetWindowText(str);
	GetDlgItem(IDC_EDIT_OBJ_Z)->SetWindowText(str);

	//Rot
	GetDlgItem(IDC_EDIT_OBJ_RotX)->SetWindowText(str);
	GetDlgItem(IDC_EDIT_OBJ_RotY)->SetWindowText(str);
	GetDlgItem(IDC_EDIT_OBJ_RotZ)->SetWindowText(str);
}

void CBatch::Update(void)
{
	CToolStaticObject* pCreateMesh = CObjCol::GetInstance()->m_pCreateMesh;
	CToolStaticObject* pSelectMesh = CObjCol::GetInstance()->m_pSelectMesh;

	if (pCreateMesh != NULL)//생성상태
	{
		pSelectMesh = NULL;

		D3DXVECTOR3 vPos = pCreateMesh->GetInfo()->m_vPos;
		float vRotX = (float)D3DXToDegree(pCreateMesh->GetInfo()->m_fAngle[ANGLE_X]);
		float vRotY = (float)D3DXToDegree(pCreateMesh->GetInfo()->m_fAngle[ANGLE_Y]);
		float vRotZ = (float)D3DXToDegree(pCreateMesh->GetInfo()->m_fAngle[ANGLE_Z]);

		CString str;

		str.Format(L"%.1f", vPos.x);
		GetDlgItem(IDC_EDIT_OBJ_X)->SetWindowText(str);
		m_ctrObjXSlider.SetPos((int)vPos.x);
		str.Format(L"%.1f", vPos.y);
		GetDlgItem(IDC_EDIT_OBJ_Y)->SetWindowText(str);
		m_ctrObjYSlider.SetPos((int)vPos.y);
		str.Format(L"%.1f", vPos.z);
		GetDlgItem(IDC_EDIT_OBJ_Z)->SetWindowText(str);
		m_ctrObjZSlider.SetPos((int)vPos.z);

		str.Format(L"%.1f", vRotX);
		GetDlgItem(IDC_EDIT_OBJ_RotX)->SetWindowText(str);
		m_ctrObjRotXSlider.SetPos((int)vRotX);

		str.Format(L"%.1f", vRotY);
		GetDlgItem(IDC_EDIT_OBJ_RotY)->SetWindowText(str);
		m_ctrObjRotYSlider.SetPos((int)vRotY);

		str.Format(L"%.1f", vRotZ);
		GetDlgItem(IDC_EDIT_OBJ_RotZ)->SetWindowText(str);
		m_ctrObjRotZSlider.SetPos((int)vRotZ);
	}

	if (pSelectMesh != NULL)//생성상태
	{
		pCreateMesh = NULL;

		D3DXVECTOR3 vPos = pSelectMesh->GetInfo()->m_vPos;
		float vRotX = (float)D3DXToDegree(pSelectMesh->GetInfo()->m_fAngle[ANGLE_X]);
		float vRotY = (float)D3DXToDegree(pSelectMesh->GetInfo()->m_fAngle[ANGLE_Y]);
		float vRotZ = (float)D3DXToDegree(pSelectMesh->GetInfo()->m_fAngle[ANGLE_Z]);

		CString str;

		str.Format(L"%.1f", vPos.x);
		GetDlgItem(IDC_EDIT_OBJ_X)->SetWindowText(str);
		m_ctrObjXSlider.SetPos((int)vPos.x);
		str.Format(L"%.1f", vPos.y);
		GetDlgItem(IDC_EDIT_OBJ_Y)->SetWindowText(str);
		m_ctrObjYSlider.SetPos((int)vPos.y);
		str.Format(L"%.1f", vPos.z);
		GetDlgItem(IDC_EDIT_OBJ_Z)->SetWindowText(str);
		m_ctrObjZSlider.SetPos((int)vPos.z);

		str.Format(L"%.1f", vRotX);
		GetDlgItem(IDC_EDIT_OBJ_RotX)->SetWindowText(str);
		m_ctrObjRotXSlider.SetPos((int)vRotX);

		str.Format(L"%.1f", vRotY);
		GetDlgItem(IDC_EDIT_OBJ_RotY)->SetWindowText(str);
		m_ctrObjRotYSlider.SetPos((int)vRotY);

		str.Format(L"%.1f", vRotZ);
		GetDlgItem(IDC_EDIT_OBJ_RotZ)->SetWindowText(str);
		m_ctrObjRotZSlider.SetPos((int)vRotZ);
	}

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



	//Painting
	HTREEITEM  hPainting;

	hPainting = m_ctrlTreeObject.InsertItem(L"Painting", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	HTREEITEM  hPainting1;
	hPainting1 = m_ctrlTreeObject.InsertItem(L"Mesh_Painting1", 1/* nImage */, 1/* nSelectedImage */, hPainting, TVI_LAST);

	HTREEITEM  hPainting2;
	hPainting2 = m_ctrlTreeObject.InsertItem(L"Mesh_Painting2", 1/* nImage */, 1/* nSelectedImage */, hPainting, TVI_LAST);




	//Wall
	HTREEITEM  hWall;

	hWall = m_ctrlTreeObject.InsertItem(L"Wall", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	HTREEITEM  hWall1;
	hWall1 = m_ctrlTreeObject.InsertItem(L"Mesh_Wall1", 1/* nImage */, 1/* nSelectedImage */, hWall, TVI_LAST);

	HTREEITEM  hWall2;
	hWall2 = m_ctrlTreeObject.InsertItem(L"Mesh_Wall2", 1/* nImage */, 1/* nSelectedImage */, hWall, TVI_LAST);

	HTREEITEM  hWall3;
	hWall3 = m_ctrlTreeObject.InsertItem(L"Mesh_Wall3", 1/* nImage */, 1/* nSelectedImage */, hWall, TVI_LAST);

	HTREEITEM  hWall4;
	hWall4 = m_ctrlTreeObject.InsertItem(L"Mesh_Wall4", 1/* nImage */, 1/* nSelectedImage */, hWall, TVI_LAST);

	HTREEITEM  hWall5;
	hWall5 = m_ctrlTreeObject.InsertItem(L"Mesh_Wall5", 1/* nImage */, 1/* nSelectedImage */, hWall, TVI_LAST);

	HTREEITEM  hWall6;
	hWall6 = m_ctrlTreeObject.InsertItem(L"Mesh_Wall6", 1/* nImage */, 1/* nSelectedImage */, hWall, TVI_LAST);

	HTREEITEM  hWall7;
	hWall7 = m_ctrlTreeObject.InsertItem(L"Mesh_Wall7", 1/* nImage */, 1/* nSelectedImage */, hWall, TVI_LAST);

	HTREEITEM  hWall8;
	hWall8 = m_ctrlTreeObject.InsertItem(L"Mesh_Wall8", 1/* nImage */, 1/* nSelectedImage */, hWall, TVI_LAST);

	HTREEITEM  hWall9;
	hWall9 = m_ctrlTreeObject.InsertItem(L"Mesh_Wall9", 1/* nImage */, 1/* nSelectedImage */, hWall, TVI_LAST);

	HTREEITEM  hWall10;
	hWall10 = m_ctrlTreeObject.InsertItem(L"Mesh_Wall10", 1/* nImage */, 1/* nSelectedImage */, hWall, TVI_LAST);


	//BookBox
	HTREEITEM  hBookBox;
	hBookBox = m_ctrlTreeObject.InsertItem(L"BookBox", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	HTREEITEM  hBookBox1;
	hBookBox1 = m_ctrlTreeObject.InsertItem(L"Mesh_BookBox1", 1/* nImage */, 1/* nSelectedImage */, hBookBox, TVI_LAST);
	
	HTREEITEM  hBookBox2;
	hBookBox2 = m_ctrlTreeObject.InsertItem(L"Mesh_BookBox2", 1/* nImage */, 1/* nSelectedImage */, hBookBox, TVI_LAST);

	//BoxFile
	HTREEITEM  hBoxFile;

	hBoxFile = m_ctrlTreeObject.InsertItem(L"BoxFile", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	HTREEITEM  hBoxFile1;
	hBoxFile1 = m_ctrlTreeObject.InsertItem(L"Mesh_BoxFile1", 1/* nImage */, 1/* nSelectedImage */, hBoxFile, TVI_LAST);

	HTREEITEM  hBoxFile2;
	hBoxFile2 = m_ctrlTreeObject.InsertItem(L"Mesh_BoxFile2", 1/* nImage */, 1/* nSelectedImage */, hBoxFile, TVI_LAST);

	HTREEITEM  hBoxFile3;
	hBoxFile3 = m_ctrlTreeObject.InsertItem(L"Mesh_BoxFile3", 1/* nImage */, 1/* nSelectedImage */, hBoxFile, TVI_LAST);

	HTREEITEM  hBoxFile4;
	hBoxFile4 = m_ctrlTreeObject.InsertItem(L"Mesh_BoxFile4", 1/* nImage */, 1/* nSelectedImage */, hBoxFile, TVI_LAST);

	HTREEITEM  hBoxFile5;
	hBoxFile5 = m_ctrlTreeObject.InsertItem(L"Mesh_BoxFile5", 1/* nImage */, 1/* nSelectedImage */, hBoxFile, TVI_LAST);

	HTREEITEM  hBoxFile6;
	hBoxFile6 = m_ctrlTreeObject.InsertItem(L"Mesh_BoxFile6", 1/* nImage */, 1/* nSelectedImage */, hBoxFile, TVI_LAST);

	HTREEITEM  hBoxFile7;
	hBoxFile7 = m_ctrlTreeObject.InsertItem(L"Mesh_BoxFile7", 1/* nImage */, 1/* nSelectedImage */, hBoxFile, TVI_LAST);

	HTREEITEM  hBoxFile8;
	hBoxFile8 = m_ctrlTreeObject.InsertItem(L"Mesh_BoxFile8", 1/* nImage */, 1/* nSelectedImage */, hBoxFile, TVI_LAST);



	//WaterCooler
	HTREEITEM  hWaterCooler;

	hWaterCooler = m_ctrlTreeObject.InsertItem(L"WaterCooler", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	//WaterCooler1
	HTREEITEM  hWaterCooler1;
	hWaterCooler1 = m_ctrlTreeObject.InsertItem(L"Mesh_WaterCooler", 1/* nImage */, 1/* nSelectedImage */, hWaterCooler, TVI_LAST);


	//Couch
	HTREEITEM  hCouch;

	hCouch = m_ctrlTreeObject.InsertItem(L"Couch", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	//Couch1
	HTREEITEM  hCouch1;
	hCouch1 = m_ctrlTreeObject.InsertItem(L"Mesh_Couch1", 1/* nImage */, 1/* nSelectedImage */, hCouch, TVI_LAST);

	//Couch2
	HTREEITEM  hCouch2;
	hCouch2 = m_ctrlTreeObject.InsertItem(L"Mesh_Couch2", 1/* nImage */, 1/* nSelectedImage */, hCouch, TVI_LAST);

	//Couch3
	HTREEITEM  hCouch3;
	hCouch3 = m_ctrlTreeObject.InsertItem(L"Mesh_Couch3", 1/* nImage */, 1/* nSelectedImage */, hCouch, TVI_LAST);

	//Couch4
	HTREEITEM  hCouch4;
	hCouch4 = m_ctrlTreeObject.InsertItem(L"Mesh_Couch4", 1/* nImage */, 1/* nSelectedImage */, hCouch, TVI_LAST);

	//RubbishBin
	HTREEITEM  hRubbishBin;

	hRubbishBin = m_ctrlTreeObject.InsertItem(L"RubbishBin", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	//RubbishBin1
	HTREEITEM  hRubbishBin1;
	hRubbishBin1 = m_ctrlTreeObject.InsertItem(L"Mesh_RubbishBin1", 1/* nImage */, 1/* nSelectedImage */, hRubbishBin, TVI_LAST);

	//RubbishBin2
	HTREEITEM  hRubbishBin2;
	hRubbishBin2 = m_ctrlTreeObject.InsertItem(L"Mesh_RubbishBin2", 1/* nImage */, 1/* nSelectedImage */, hRubbishBin, TVI_LAST);


	//Coffeetable
	HTREEITEM  hCoffeetable;

	hCoffeetable = m_ctrlTreeObject.InsertItem(L"Coffeetable", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	//Coffeetable1
	HTREEITEM  hCoffeetable1;
	hCoffeetable1 = m_ctrlTreeObject.InsertItem(L"Mesh_Coffeetable1", 1/* nImage */, 1/* nSelectedImage */, hCoffeetable, TVI_LAST);

	//Coffeetable2
	HTREEITEM  hCoffeetable2;
	hCoffeetable2 = m_ctrlTreeObject.InsertItem(L"Mesh_Coffeetable2", 1/* nImage */, 1/* nSelectedImage */, hCoffeetable, TVI_LAST);



	//Clock
	HTREEITEM  hClock;

	hClock = m_ctrlTreeObject.InsertItem(L"Clock", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	//Clock
	HTREEITEM  hClock1;
	hClock1 = m_ctrlTreeObject.InsertItem(L"Mesh_Clock", 1/* nImage */, 1/* nSelectedImage */, hClock, TVI_LAST);


	//Door
	HTREEITEM  hDoor;

	hDoor = m_ctrlTreeObject.InsertItem(L"Door", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	//Door1
	HTREEITEM  hDoor1;
	hDoor1 = m_ctrlTreeObject.InsertItem(L"Mesh_Door", 1/* nImage */, 1/* nSelectedImage */, hDoor, TVI_LAST);



	//Pinboard
	HTREEITEM  hPinboard;

	hPinboard = m_ctrlTreeObject.InsertItem(L"Pinboard", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);
	
	//Pinboard1
	HTREEITEM  hPinboard1;
	hPinboard1 = m_ctrlTreeObject.InsertItem(L"Mesh_Pinboard", 1/* nImage */, 1/* nSelectedImage */, hPinboard, TVI_LAST);



	//PowerOutlet
	HTREEITEM  hPowerOutlet;

	hPowerOutlet = m_ctrlTreeObject.InsertItem(L"PowerOutlet", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	//PowerOutlet1
	HTREEITEM  hPowerOutlet1;
	hPowerOutlet1 = m_ctrlTreeObject.InsertItem(L"Mesh_PowerOutlet", 1/* nImage */, 1/* nSelectedImage */, hPowerOutlet, TVI_LAST);


	//Floor
	HTREEITEM  hFloor;

	hFloor = m_ctrlTreeObject.InsertItem(L"Floor", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	//Floor1
	HTREEITEM  hFloor1;
	hFloor1 = m_ctrlTreeObject.InsertItem(L"Mesh_Floor1", 1/* nImage */, 1/* nSelectedImage */, hFloor, TVI_LAST);

	//Floor2
	HTREEITEM  hFloor2;
	hFloor2 = m_ctrlTreeObject.InsertItem(L"Mesh_Floor2", 1/* nImage */, 1/* nSelectedImage */, hFloor, TVI_LAST);

	//Floor3
	HTREEITEM  hFloor3;
	hFloor3 = m_ctrlTreeObject.InsertItem(L"Mesh_Floor3", 1/* nImage */, 1/* nSelectedImage */, hFloor, TVI_LAST);




	//Plant
	HTREEITEM  hPlant;

	hPlant = m_ctrlTreeObject.InsertItem(L"Plant", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	//Plant1
	HTREEITEM  hPlant1;
	hPlant1 = m_ctrlTreeObject.InsertItem(L"Mesh_Plant1", 1/* nImage */, 1/* nSelectedImage */, hPlant, TVI_LAST);

	//Plant2
	HTREEITEM  hPlant2;
	hPlant2 = m_ctrlTreeObject.InsertItem(L"Mesh_Plant2", 1/* nImage */, 1/* nSelectedImage */, hPlant, TVI_LAST);

	//Plant3
	HTREEITEM  hPlant3;
	hPlant3 = m_ctrlTreeObject.InsertItem(L"Mesh_Plant3", 1/* nImage */, 1/* nSelectedImage */, hPlant, TVI_LAST);

	//Plant4
	HTREEITEM  hPlant4;
	hPlant4 = m_ctrlTreeObject.InsertItem(L"Mesh_Plant4", 1/* nImage */, 1/* nSelectedImage */, hPlant, TVI_LAST);


	//Plant5
	HTREEITEM  hPlant5;
	hPlant5 = m_ctrlTreeObject.InsertItem(L"Mesh_Plant5", 1/* nImage */, 1/* nSelectedImage */, hPlant, TVI_LAST);


	//Chair
	HTREEITEM  hChair;

	hChair = m_ctrlTreeObject.InsertItem(L"Chair", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	//Chair1
	HTREEITEM  hChair1;
	hChair1 = m_ctrlTreeObject.InsertItem(L"Mesh_Chair1", 1/* nImage */, 1/* nSelectedImage */, hChair, TVI_LAST);

	//Chair2
	HTREEITEM  hChair2;
	hChair2 = m_ctrlTreeObject.InsertItem(L"Mesh_Chair2", 1/* nImage */, 1/* nSelectedImage */, hChair, TVI_LAST);

	//Chair3
	HTREEITEM  hChair3;
	hChair3 = m_ctrlTreeObject.InsertItem(L"Mesh_Chair3", 1/* nImage */, 1/* nSelectedImage */, hChair, TVI_LAST);

	//Chair4
	HTREEITEM  hChair4;
	hChair4 = m_ctrlTreeObject.InsertItem(L"Mesh_Chair4", 1/* nImage */, 1/* nSelectedImage */, hChair, TVI_LAST);


	//Cabinet
	HTREEITEM  hCabinet;
	hCabinet = m_ctrlTreeObject.InsertItem(L"Cabinet", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	//Cabinet1
	HTREEITEM  hCabinet1;
	hCabinet1 = m_ctrlTreeObject.InsertItem(L"Mesh_Cabinet1", 1/* nImage */, 1/* nSelectedImage */, hCabinet, TVI_LAST);

	//Cabinet2
	HTREEITEM  hCabinet2;
	hCabinet2 = m_ctrlTreeObject.InsertItem(L"Mesh_Cabinet2", 1/* nImage */, 1/* nSelectedImage */, hCabinet, TVI_LAST);

	//Shelf
	HTREEITEM  hShelf;
	hShelf = m_ctrlTreeObject.InsertItem(L"Shelf", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	//Shelf1
	HTREEITEM  hShelf1;
	hShelf1 = m_ctrlTreeObject.InsertItem(L"Mesh_Shelf1", 1/* nImage */, 1/* nSelectedImage */, hShelf, TVI_LAST);

	//Shelf2
	HTREEITEM  hShelf2;
	hShelf2 = m_ctrlTreeObject.InsertItem(L"Mesh_Shelf2", 1/* nImage */, 1/* nSelectedImage */, hShelf, TVI_LAST);

	//Shelf3
	HTREEITEM  hShelf3;
	hShelf3 = m_ctrlTreeObject.InsertItem(L"Mesh_Shelf3", 1/* nImage */, 1/* nSelectedImage */, hShelf, TVI_LAST);


	//Desk
	HTREEITEM  hDesk;
	hDesk = m_ctrlTreeObject.InsertItem(L"Desk", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	//Desk1
	HTREEITEM  hDesk1;
	hDesk1 = m_ctrlTreeObject.InsertItem(L"Mesh_Desk1", 1/* nImage */, 1/* nSelectedImage */, hDesk, TVI_LAST);

	//Desk2
	HTREEITEM  hDesk2;
	hDesk2 = m_ctrlTreeObject.InsertItem(L"Mesh_Desk2", 1/* nImage */, 1/* nSelectedImage */, hDesk, TVI_LAST);

	//Desk3
	HTREEITEM  hDesk3;
	hDesk3 = m_ctrlTreeObject.InsertItem(L"Mesh_Desk3", 1/* nImage */, 1/* nSelectedImage */, hDesk, TVI_LAST);

	//Desk4
	HTREEITEM  hDesk4;
	hDesk4 = m_ctrlTreeObject.InsertItem(L"Mesh_Desk4", 1/* nImage */, 1/* nSelectedImage */, hDesk, TVI_LAST);

	//Desk5
	HTREEITEM  hDesk5;
	hDesk5 = m_ctrlTreeObject.InsertItem(L"Mesh_Desk5", 1/* nImage */, 1/* nSelectedImage */, hDesk, TVI_LAST);

	//Desk6
	HTREEITEM  hDesk6;
	hDesk6 = m_ctrlTreeObject.InsertItem(L"Mesh_Desk6", 1/* nImage */, 1/* nSelectedImage */, hDesk, TVI_LAST);

	//Desk7
	HTREEITEM  hDesk7;
	hDesk7 = m_ctrlTreeObject.InsertItem(L"Mesh_Desk7", 1/* nImage */, 1/* nSelectedImage */, hDesk, TVI_LAST);

	//Desk8
	HTREEITEM  hDesk8;
	hDesk8 = m_ctrlTreeObject.InsertItem(L"Mesh_Desk8", 1/* nImage */, 1/* nSelectedImage */, hDesk, TVI_LAST);

	//Desk9
	HTREEITEM  hDesk9;
	hDesk9 = m_ctrlTreeObject.InsertItem(L"Mesh_Desk9", 1/* nImage */, 1/* nSelectedImage */, hDesk, TVI_LAST);

	//Desk10
	HTREEITEM  hDesk10;
	hDesk10 = m_ctrlTreeObject.InsertItem(L"Mesh_Desk10", 1/* nImage */, 1/* nSelectedImage */, hDesk, TVI_LAST);

	//Desk11
	HTREEITEM  hDesk11;
	hDesk11 = m_ctrlTreeObject.InsertItem(L"Mesh_Desk11", 1/* nImage */, 1/* nSelectedImage */, hDesk, TVI_LAST);

	//Desk12
	HTREEITEM  hDesk12;
	hDesk12 = m_ctrlTreeObject.InsertItem(L"Mesh_Desk12", 1/* nImage */, 1/* nSelectedImage */, hDesk, TVI_LAST);


	//Photocopier
	HTREEITEM  hPhotocopier;
	hPhotocopier = m_ctrlTreeObject.InsertItem(L"Photocopier", 1/* nImage */, 1/* nSelectedImage */, hRoot, TVI_LAST);

	//Photocopier1
	HTREEITEM  hPhotocopier1;
	hPhotocopier1 = m_ctrlTreeObject.InsertItem(L"Mesh_Photocopier", 1/* nImage */, 1/* nSelectedImage */, hPhotocopier, TVI_LAST);


}

//생성
void CBatch::OnBnClickedCreateBtn()
{
	UpdateData(TRUE);

	HTREEITEM	Object;

	Object = m_ctrlTreeObject.GetSelectedItem();

	CString strName = m_ctrlTreeObject.GetItemText(Object);
	wstring wstrName = (LPCTSTR)strName;

	if (strName == L"Object" || strName == L"Book" || strName == L"BookBox" || 
		strName == L"BoxFile" || strName == L"Floor")
	{
		AfxMessageBox(L"메시 선택 잘못됐어!");
		return;
	}

	CObj* pObj = NULL;

	pObj = CToolStaticObject::Create(wstrName);


	CObjMgr::GetInstance()->AddObject(L"StaticObject", pObj);

	UpdateData(FALSE);
}

//삭제
void CBatch::OnBnClickedDeleteBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"StaticObject")->begin();
	list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"StaticObject")->end();

	CToolStaticObject* pCreateMesh = CObjCol::GetInstance()->m_pCreateMesh;
	CToolStaticObject* pSelectMesh = CObjCol::GetInstance()->m_pSelectMesh;

	if (pCreateMesh && pSelectMesh)
		return;

	for (iter; iter != iter_end; ++iter)
	{
		if ( ((*iter) == pCreateMesh)   || ((*iter) == pSelectMesh) )
		{
			if (  (*iter) == pCreateMesh)
			{
				CObjCol::GetInstance()->m_pCreateMesh = NULL;
			}
			if (((*iter) == pSelectMesh))
			{
				CObjCol::GetInstance()->m_pSelectMesh = NULL;
			}

			(*iter)->SetObjState(STATE_DESTROY);
		}
	}
}

//모두 삭제
void CBatch::OnBnClickedDeleteAllBtn()
{
	list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"StaticObject")->begin();
	list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"StaticObject")->end();

	
	CObjCol::GetInstance()->m_pCreateMesh = NULL;
	CObjCol::GetInstance()->m_pSelectMesh = NULL;

	for (iter; iter != iter_end; ++iter)
	{
		if( ((CToolStaticObject*)(*iter))->GetMode() != MODE_FIX)
			(*iter)->SetObjState(STATE_DESTROY);
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

//Save
void CBatch::OnBnClickedSaveBtn()
{
	CString strPath;
	GetDlgItemText(IDC_OBJPATHSTATIC, strPath);

	if (strPath == L"")
	{
		AfxMessageBox(L"경로 지정을 먼저 해야 됩니다.!");
		return;
	}

	DWORD	dwByte = 0;
	HANDLE	hFile = CreateFile(strPath, GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == NULL)
	{
		AfxMessageBox(L"파일 개방 실패");
		return;
	}

	list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"StaticObject")->begin();
	list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"StaticObject")->end();

	for (iter; iter != iter_end; ++iter)
	{
		if (((CToolStaticObject*)(*iter))->GetMode() == MODE_FIX)
			continue;

		((CToolStaticObject*)(*iter))->ObjInfoSetting();
		WriteFile(hFile, ((CToolStaticObject*)(*iter))->GetObjInfo(), sizeof(OBJ_INFO), &dwByte, NULL);
	}

	CloseHandle(hFile);
	AfxMessageBox(L"저장 성공!");

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

//Load
void CBatch::OnBnClickedLoadBtn()
{
	CFileDialog	FileDlg(TRUE, NULL, NULL,
						OFN_EXPLORER, NULL, NULL);

	TCHAR	szCurDir[MAX_PATH] = L"";
	GetCurrentDirectory(MAX_PATH - 1, szCurDir);
	PathRemoveFileSpec(szCurDir);
	PathRemoveFileSpec(szCurDir);
	lstrcat(szCurDir, L"\\dat");
	FileDlg.m_ofn.lpstrInitialDir = szCurDir;

	CString strPathName;

	if (FileDlg.DoModal() == IDOK)
	{
		DWORD	dwByte = 0;

		strPathName = FileDlg.GetPathName();

		HANDLE hFile = CreateFile(strPathName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		CObjMgr::GetInstance()->AllDestroy();

		CObjCol::GetInstance()->m_pCreateMesh = NULL;
		CObjCol::GetInstance()->m_pSelectMesh = NULL;

		while (true)
		{
			OBJ_INFO   tInfo;

			ReadFile(hFile, &tInfo, sizeof(OBJ_INFO), &dwByte, NULL);

			if (dwByte == 0)
			{
				CloseHandle(hFile);
				return;
			}

			CObj* pObject = NULL;

			wstring strName = tInfo.m_szName;

			pObject = CToolStaticObject::Create(strName);
			((CToolStaticObject*)(pObject))->SetObjInfo(&tInfo);
			((CToolStaticObject*)(pObject))->InfoSetting();
			((CToolStaticObject*)(pObject))->SetMode(MODE_STOP);
			CObjMgr::GetInstance()->AddObject(L"StaticObject", pObject);
		}
		AfxMessageBox(L"로드 성공!!");
	}
	else
	{
		AfxMessageBox(L"로드 실패 !!");
	}
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

//Path 경로
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

wstring CBatch::ConvertRelativePath(wstring strFullPath)
{
	TCHAR	szRelativePath[MAX_PATH] = L"";
	TCHAR	szDirectoryPath[MAX_PATH] = L"";

	GetCurrentDirectory(sizeof(szDirectoryPath), szDirectoryPath);

	//한 경로에서 다른 경로로 가는 상대 경로를 추출하는 것임...
	PathRelativePathTo((LPWSTR)szRelativePath, szDirectoryPath,
		FILE_ATTRIBUTE_DIRECTORY, strFullPath.c_str(),
		FILE_ATTRIBUTE_NORMAL);

	return szRelativePath;
}


void CBatch::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	SCROLLINFO	si;
	int			nYCur = 0;

	if (bStartReSize == TRUE)
	{
		nYOrgHeight = cy;
		bStartReSize = FALSE;
	}

	nYCur = GetScrollPos(SB_VERT);
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
	si.nMin = 0;
	si.nMax = nYOrgHeight;
	si.nPage = cy;
	si.nPos = nYCur;

	SetScrollInfo(SB_VERT, &si, TRUE);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CBatch::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	int yInc = 0, yMove = 0;
	int nYCur = GetScrollPos(SB_VERT);
	int nMin = 0, nMax = 0;
	CRect crt;

	switch (nSBCode)
	{
	case SB_LINEUP:
		yInc = -1;
		break;
	case SB_LINEDOWN:
		yInc = 1;
		break;
	case SB_PAGEUP:
		yInc = -20;
		break;
	case SB_PAGEDOWN:
		yInc = 20;
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		yInc = nPos - nYCur;
		break;
	default:
		break;
	}

	GetClientRect(&crt);
	GetScrollRange(SB_VERT, &nMin, &nMax);
	nMax -= crt.bottom;

	yMove = nYCur + yInc;
	if (nYCur + yInc < nMin)
	{
		yInc = -nYCur;
	}
	else if (nYCur + yInc > nMax)
	{
		yInc = nMax - nYCur;
	}
	nYCur = nYCur + yInc;
	ScrollWindow(0, -yInc);
	SetScrollPos(SB_VERT, nYCur);

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CBatch::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	
	if ((pScrollBar != NULL) && (pScrollBar->m_hWnd == m_ctrObjXSlider.m_hWnd))
	{
		CToolStaticObject* pCreateMesh = CObjCol::GetInstance()->m_pCreateMesh;
		CToolStaticObject* pSelectMesh = CObjCol::GetInstance()->m_pSelectMesh;

		if (pCreateMesh != NULL)//생성상태
		{
			pSelectMesh = NULL;

			pCreateMesh->GetInfo()->m_vPos.x = (float)m_ctrObjXSlider.GetPos();
			CString str;

			str.Format(L"%.1f", (float)m_ctrObjXSlider.GetPos());
			GetDlgItem(IDC_EDIT_OBJ_X)->SetWindowText(str);
		}

		if (pSelectMesh != NULL)//
		{
			pCreateMesh = NULL;

			pSelectMesh->GetInfo()->m_vPos.x = (float)m_ctrObjXSlider.GetPos();
			CString str;

			str.Format(L"%.1f", (float)m_ctrObjXSlider.GetPos());
			GetDlgItem(IDC_EDIT_OBJ_X)->SetWindowText(str);
		}
	}


	//Y슬라이더가 움직일경우
	if ((pScrollBar != NULL) && (pScrollBar->m_hWnd == m_ctrObjYSlider.m_hWnd))
	{
		CToolStaticObject* pCreateMesh = CObjCol::GetInstance()->m_pCreateMesh;
		CToolStaticObject* pSelectMesh = CObjCol::GetInstance()->m_pSelectMesh;

		if (pCreateMesh != NULL)//생성상태
		{
			pSelectMesh = NULL;

			pCreateMesh->GetInfo()->m_vPos.y = (float)m_ctrObjYSlider.GetPos();
			CString str;

			str.Format(L"%.1f", (float)m_ctrObjYSlider.GetPos());
			GetDlgItem(IDC_EDIT_OBJ_Y)->SetWindowText(str);
		}

		if (pSelectMesh != NULL)//
		{
			pCreateMesh = NULL;

			pSelectMesh->GetInfo()->m_vPos.y = (float)m_ctrObjYSlider.GetPos();
			CString str;

			str.Format(L"%.1f", (float)m_ctrObjYSlider.GetPos());
			GetDlgItem(IDC_EDIT_OBJ_Y)->SetWindowText(str);
		}
	}

	//Z슬라이더가 움직일경우
	if ((pScrollBar != NULL) && (pScrollBar->m_hWnd == m_ctrObjZSlider.m_hWnd))
	{
		CToolStaticObject* pCreateMesh = CObjCol::GetInstance()->m_pCreateMesh;
		CToolStaticObject* pSelectMesh = CObjCol::GetInstance()->m_pSelectMesh;

		if (pCreateMesh != NULL)//생성상태
		{
			pSelectMesh = NULL;

			pCreateMesh->GetInfo()->m_vPos.z = (float)m_ctrObjZSlider.GetPos();
			CString str;

			str.Format(L"%.1f", (float)m_ctrObjZSlider.GetPos());
			GetDlgItem(IDC_EDIT_OBJ_Z)->SetWindowText(str);
		}

		if (pSelectMesh != NULL)//
		{
			pCreateMesh = NULL;

			pSelectMesh->GetInfo()->m_vPos.z = (float)m_ctrObjZSlider.GetPos();
			CString str;

			str.Format(L"%.1f", (float)m_ctrObjZSlider.GetPos());
			GetDlgItem(IDC_EDIT_OBJ_Z)->SetWindowText(str);
		}

	}

	//RotX 슬라이더가 움직이는 경우
	if ((pScrollBar != NULL) && (pScrollBar->m_hWnd == m_ctrObjRotXSlider.m_hWnd))
	{
		CToolStaticObject* pCreateMesh = CObjCol::GetInstance()->m_pCreateMesh;
		CToolStaticObject* pSelectMesh = CObjCol::GetInstance()->m_pSelectMesh;

		if (pCreateMesh != NULL)//생성상태
		{
			pSelectMesh = NULL;

			pCreateMesh->GetInfo()->m_fAngle[ANGLE_X] = (float)D3DXToRadian(m_ctrObjRotXSlider.GetPos());
			CString str;

			str.Format(L"%.1f", (float)m_ctrObjRotXSlider.GetPos());

			GetDlgItem(IDC_EDIT_OBJ_RotX)->SetWindowText(str);
		}

		if (pSelectMesh != NULL)
		{
			pCreateMesh = NULL;

			pSelectMesh->GetInfo()->m_fAngle[ANGLE_X] = (float)D3DXToRadian(m_ctrObjRotXSlider.GetPos());
			CString str;

			str.Format(L"%.1f", (float)m_ctrObjRotXSlider.GetPos());
			GetDlgItem(IDC_EDIT_OBJ_RotX)->SetWindowText(str);
		}
	}

	//RotY 슬라이더가 움직이는 경우
	if ((pScrollBar != NULL) && (pScrollBar->m_hWnd == m_ctrObjRotYSlider.m_hWnd))
	{
		CToolStaticObject* pCreateMesh = CObjCol::GetInstance()->m_pCreateMesh;
		CToolStaticObject* pSelectMesh = CObjCol::GetInstance()->m_pSelectMesh;

		if (pCreateMesh != NULL)//생성상태
		{
			pSelectMesh = NULL;

			pCreateMesh->GetInfo()->m_fAngle[ANGLE_Y] = (float)D3DXToRadian(m_ctrObjRotYSlider.GetPos());
			CString str;

			str.Format(L"%.1f", (float)m_ctrObjRotYSlider.GetPos());

			GetDlgItem(IDC_EDIT_OBJ_RotY)->SetWindowText(str);
		}

		if (pSelectMesh != NULL)
		{
			pCreateMesh = NULL;

			pSelectMesh->GetInfo()->m_fAngle[ANGLE_Y] = (float)D3DXToRadian(m_ctrObjRotYSlider.GetPos());
			CString str;

			str.Format(L"%.1f", (float)m_ctrObjRotYSlider.GetPos());
			GetDlgItem(IDC_EDIT_OBJ_RotY)->SetWindowText(str);
		}
	}

	//RotZ 슬라이더가 움직이는 경우
	if ((pScrollBar != NULL) && (pScrollBar->m_hWnd == m_ctrObjRotZSlider.m_hWnd))
	{
		CToolStaticObject* pCreateMesh = CObjCol::GetInstance()->m_pCreateMesh;
		CToolStaticObject* pSelectMesh = CObjCol::GetInstance()->m_pSelectMesh;

		if (pCreateMesh != NULL)//생성상태
		{
			pSelectMesh = NULL;

			pCreateMesh->GetInfo()->m_fAngle[ANGLE_Z] = (float)D3DXToRadian(m_ctrObjRotZSlider.GetPos());
			CString str;

			str.Format(L"%.1f", (float)m_ctrObjRotZSlider.GetPos());
			GetDlgItem(IDC_EDIT_OBJ_RotZ)->SetWindowText(str);
		}

		if (pSelectMesh != NULL)
		{
			pCreateMesh = NULL;

			pSelectMesh->GetInfo()->m_fAngle[ANGLE_Z] = (float)D3DXToRadian(m_ctrObjRotZSlider.GetPos());
			CString str;

			str.Format(L"%.1f", (float)m_ctrObjRotZSlider.GetPos());
			GetDlgItem(IDC_EDIT_OBJ_RotZ)->SetWindowText(str);
		}
	}



	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

//X축 변환
void CBatch::OnEnChangeEditObjX()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

//Y축 변환
void CBatch::OnEnChangeEditObjY()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

//Z축 변환
void CBatch::OnEnChangeEditObjZ()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
