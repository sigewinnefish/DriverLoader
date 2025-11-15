
// DriverLoaderDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "DriverLoader.h"
#include "DriverLoaderDlg.h"
#include "afxdialogex.h"
#include "loader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CDriverLoaderDlg 对话框



CDriverLoaderDlg::CDriverLoaderDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DRIVERLOADER_DIALOG, pParent)
	, driverpath(_T(""))
	, status(_T("拖放或填入驱动文件路径"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDriverLoaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_driverpath, driverpath);
	DDX_Text(pDX, IDC_status, status);
}

BEGIN_MESSAGE_MAP(CDriverLoaderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_install, &CDriverLoaderDlg::OnBnClickedinstall)
	ON_BN_CLICKED(IDC_start, &CDriverLoaderDlg::OnBnClickedstart)
	ON_BN_CLICKED(IDC_stop, &CDriverLoaderDlg::OnBnClickedstop)
	ON_BN_CLICKED(IDC_uninstall, &CDriverLoaderDlg::OnBnClickeduninstall)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CDriverLoaderDlg 消息处理程序

BOOL CDriverLoaderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	this->DragAcceptFiles(TRUE);
	initDriverLoader();
	SetWindowText(_T("驱动加载器"));
	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDriverLoaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDriverLoaderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDriverLoaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void Cstring2Char(char* path ,CString &cs)
{
	ZeroMemory(path, MAX_PATH);
	WCHAR* pwchar = cs.GetBuffer();
	char* pchar = (char*)pwchar;
	int j = 0;
	for (int i = 0; i < wcslen(pwchar); i++)
	{
		path[i] = pchar[j];
		j += 2;
	}
	cs.ReleaseBuffer();
}

void CDriverLoaderDlg::OnBnClickedinstall()
{
	UpdateData(TRUE);
	Cstring2Char(path, driverpath);
	DriverInstall();

	if (::s == DriverInstallFailed)
	{
		status = _T("驱动安装失败");
	}
	else
	{
		status = _T("驱动安装成功");
	}
	UpdateData(FALSE);
}

void CDriverLoaderDlg::OnBnClickedstart()
{
	UpdateData(TRUE);
	Cstring2Char(path, driverpath);
	DriverStart();
	if (::s == DriverStartFailed)
	{
		status = _T("驱动启动失败");
	}
	else
	{
		status = _T("驱动启动成功");
	}
	UpdateData(FALSE);
}

void CDriverLoaderDlg::OnBnClickedstop()
{
	UpdateData(TRUE);
	Cstring2Char(path, driverpath);
	DriverStop();
	if (::s == DriverStopFailed)
	{
		status = _T("驱动停止失败");
	}
	else
	{
		status = _T("驱动停止成功");
	}
	UpdateData(FALSE);
}

void CDriverLoaderDlg::OnBnClickeduninstall()
{
	UpdateData(TRUE);
	Cstring2Char(path, driverpath);
	DriverUninstall();
	if (::s == DriverUninstallFailed)
	{
		status = _T("驱动卸载失败");
	}
	else
	{
		status = _T("驱动卸载成功");
	}
	UpdateData(FALSE);
}

void CDriverLoaderDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	::DragQueryFileA(hDropInfo, 0, path, MAX_PATH);
	driverpath = path;
	UpdateData(FALSE);
	CDialogEx::OnDropFiles(hDropInfo);
}
