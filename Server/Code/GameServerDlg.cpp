#include "../Bin/framework.h"
#include "GameServer.h"
#include "GameServerDlg.h"
#include "afxdialogex.h"

#include "Common/Defines.h"
#include "../../Client/Code/Common/Utility.h"

#include "Core/Core.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CGameServerDlg 대화 상자
CGameServerDlg::CGameServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GAMESERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CGameServerDlg::~CGameServerDlg()
{
	GET_INSTANCE(Core)->ServerQuit();

	std::cout << "프로그램 종료" << std::endl;
}

void CGameServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, mLabel);
	DDX_Control(pDX, IDC_LIST2, mListCtrl);
}

BEGIN_MESSAGE_MAP(CGameServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CGameServerDlg 메시지 처리기

BOOL CGameServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	initializeUI();
	if (GET_INSTANCE(Core)->Initialize() == false)
	{
		return false;
	}

	return true;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CGameServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	
	else if (nID == SC_CLOSE)
	{
		PostMessage(WM_CLOSE);
	}

	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CGameServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CGameServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGameServerDlg::OnDestroy()
{
	KillTimer(1);
	CDialogEx::OnDestroy();
}

void CGameServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		updateUI();
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CGameServerDlg::initializeUI()
{
	mFont.CreatePointFont(120, _T("Gulim"));
	GetDlgItem(IDC_STATIC1)->SetFont(&mFont);

	SetTimer(1, 1000, nullptr);
	mLabel.SetWindowTextW(_T(""));
	mListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 컬럼 추가
	{
		int index = 0;
		mListCtrl.InsertColumn(index++, _T("Id"), LVCFMT_LEFT, 50);
		mListCtrl.InsertColumn(index++, _T("State"), LVCFMT_CENTER, 70);
		mListCtrl.InsertColumn(index++, _T("IP"), LVCFMT_CENTER, 100);
		mListCtrl.InsertColumn(index++, _T("Channel"), LVCFMT_CENTER, 70);
		mListCtrl.InsertColumn(index++, _T("X"), LVCFMT_RIGHT, 50);
		mListCtrl.InsertColumn(index++, _T("Y"), LVCFMT_RIGHT, 50);
	}

	// 항목 추가
	for (int i = 0; i < MAX_USER; ++i)
	{
		mListCtrl.InsertItem(i, std::to_wstring(i).c_str());
		mListCtrl.SetItemText(i, 1, _T("false"));
		mListCtrl.SetItemText(i, 2, _T(""));
		mListCtrl.SetItemText(i, 3, _T(""));
		mListCtrl.SetItemText(i, 4, _T(""));
		mListCtrl.SetItemText(i, 5, _T(""));
	}
}


void CGameServerDlg::updateUI()
{
	int count = GET_INSTANCE(Core)->GetConnectUserCount();
	std::wstring text = L"현재 접속중인 클라이언트 수 : " + std::to_wstring(count);
	mLabel.SetWindowTextW(text.c_str());

	// 항목 추가
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (GET_INSTANCE(Core)->GetUser(i).GetIsConnect() == false)
		{
			mListCtrl.SetItemText(i, 1, L"false");
			mListCtrl.SetItemText(i, 2, _T(""));
			mListCtrl.SetItemText(i, 3, _T(""));
			mListCtrl.SetItemText(i, 4, _T(""));
			mListCtrl.SetItemText(i, 5, _T(""));
			continue;
		}

		std::string ip = GET_INSTANCE(Core)->GetUserIP(i);
		int channel = GET_INSTANCE(Core)->GetUser(i).GetChannel();
		int x = GET_INSTANCE(Core)->GetUser(i).GetX();
		int y = GET_INSTANCE(Core)->GetUser(i).GetY();

		mListCtrl.SetItemText(i, 1, L"true");
		mListCtrl.SetItemText(i, 2, StringToWString(ip).c_str());
		mListCtrl.SetItemText(i, 3, std::to_wstring(channel).c_str());
		mListCtrl.SetItemText(i, 4, std::to_wstring(x).c_str());
		mListCtrl.SetItemText(i, 5, std::to_wstring(y).c_str());
	}
}

