
// mfcDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "mfc.h"
#include "mfcDlg.h"
#include "afxdialogex.h"
#include "..\Parking\ParkingImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CBCGPDialog
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CBCGPDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CBCGPDialog)
END_MESSAGE_MAP()


// CmfcDlg dialog



CmfcDlg::CmfcDlg(CWnd *pParent /*=nullptr*/)
	: CBCGPDialog(IDD_MFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	EnableLayout();
}

void CmfcDlg::DoDataExchange(CDataExchange *pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE, m_svgCtrl);
	DDX_Control(pDX, IDC_SIDE_LINES_COMBO, m_sideLinesCombo);
	DDX_Control(pDX, IDC_FRONT_LINES_COMBO, m_frontLinesCombo);
	DDX_Control(pDX, IDC_REAR_LINES_COMBO, m_rearLinesCombo);
}

BEGIN_MESSAGE_MAP(CmfcDlg, CBCGPDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DRIVE_LHS_RADIO, &CmfcDlg::OnChangeParameter)
	ON_BN_CLICKED(IDC_DRIVE_RHS_RADIO, &CmfcDlg::OnChangeParameter)
	ON_BN_CLICKED(IDC_ROWS_SINGLE_RADIO, &CmfcDlg::OnChangeParameter)
	ON_BN_CLICKED(IDC_ROWS_DOUBLE_RADIO, &CmfcDlg::OnChangeParameter)
	ON_CBN_SELCHANGE(IDC_SIDE_LINES_COMBO, &CmfcDlg::OnChangeParameter)
	ON_CBN_SELCHANGE(IDC_FRONT_LINES_COMBO, &CmfcDlg::OnChangeParameter)
	ON_CBN_SELCHANGE(IDC_REAR_LINES_COMBO, &CmfcDlg::OnChangeParameter)
	ON_EN_CHANGE(IDC_LENGTH_EDIT, &CmfcDlg::OnChangeParameter)
	ON_EN_CHANGE(IDC_WIDTH_EDIT, &CmfcDlg::OnChangeParameter)
	ON_EN_CHANGE(IDC_ANGLE_EDIT, &CmfcDlg::OnChangeParameter)
	ON_EN_CHANGE(IDC_SHORT_PAINT_EDIT, &CmfcDlg::OnChangeParameter)
	ON_EN_CHANGE(IDC_CAP_WIDTH_EDIT, &CmfcDlg::OnChangeParameter)
	ON_EN_CHANGE(IDC_CAP_EXT_EDIT, &CmfcDlg::OnChangeParameter)
	ON_BN_CLICKED(IDC_SHOWBAYSCHK, &CmfcDlg::OnChangeParameter)
	ON_BN_CLICKED(IDC_SHOWCAPSCHK, &CmfcDlg::OnChangeParameter)
	ON_BN_CLICKED(IDC_SHOWARROWSCHK, &CmfcDlg::OnChangeParameter)
	ON_BN_CLICKED(IDC_SHOWPATHSCHK, &CmfcDlg::OnChangeParameter)
END_MESSAGE_MAP()


// CmfcDlg message handlers

BOOL CmfcDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu *pSysMenu = GetSystemMenu(FALSE);
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	EnableVisualManagerStyle(TRUE, TRUE);

	CBCGPStaticLayout *pLayout = static_cast<CBCGPStaticLayout *>(GetLayout());
	if (nullptr != pLayout)
		{
		pLayout->AddAnchor(IDC_IMAGE, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeBoth);
		pLayout->AddAnchor(IDCANCEL, CBCGPStaticLayout::e_MoveTypeBoth, CBCGPStaticLayout::e_SizeTypeNone);
		}

	CheckRadioButton(IDC_DRIVE_LHS_RADIO, IDC_DRIVE_RHS_RADIO, IDC_DRIVE_RHS_RADIO);
	CheckRadioButton(IDC_ROWS_SINGLE_RADIO, IDC_ROWS_DOUBLE_RADIO, IDC_ROWS_SINGLE_RADIO);
	m_sideLinesCombo.SetCurSel(2);
	m_frontLinesCombo.SetCurSel(2);
	m_rearLinesCombo.SetCurSel(2);
	SetDlgItemTextW(IDC_SHORT_PAINT_EDIT, _T("0.5"));
	SetDlgItemTextW(IDC_LENGTH_EDIT, _T("5.0"));
	SetDlgItemTextW(IDC_WIDTH_EDIT, _T("2.5"));
	SetDlgItemTextW(IDC_ANGLE_EDIT, _T("90.0"));
	SetDlgItemTextW(IDC_CAP_WIDTH_EDIT, _T("2.0"));
	SetDlgItemTextW(IDC_CAP_EXT_EDIT, _T("0.5"));
	CheckDlgButton(IDC_SHOWBAYSCHK, BST_CHECKED);
	CheckDlgButton(IDC_SHOWCAPSCHK, BST_CHECKED);
	CheckDlgButton(IDC_SHOWARROWSCHK, BST_CHECKED);
	CheckDlgButton(IDC_SHOWPATHSCHK, BST_UNCHECKED);

	OnChangeParameter();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CmfcDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
		{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
		}
	else
		{
		CBCGPDialog::OnSysCommand(nID, lParam);
		}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CmfcDlg::OnPaint()
{
	if (IsIconic())
		{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
		}
	else
		{
		CBCGPDialog::OnPaint();
		}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CmfcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CmfcDlg::RedrawParking()
{
	std::string params = Serialize();

	std::string buffer;
	if (parking_layout_svg_bytes(buffer, "", params))
		{
		m_svgCtrl.LoadFromBuffer(CString{ buffer.c_str() });
		}

	// Store to file as well
	parking_layout_svg_file("parking.svg", "", params);
}

void CmfcDlg::Gather()
{
	auto GetFloatValue = [this](UINT nID, double dflt)
		{
		CString strValue;
		if (GetDlgItemTextW(nID, strValue))
			{
			if (!strValue.IsEmpty())
				return _tstof(strValue);
			}
		return dflt;
		};

	m_json["driveDirection"] = GetCheckedRadioButton(IDC_DRIVE_LHS_RADIO, IDC_DRIVE_RHS_RADIO) - IDC_DRIVE_LHS_RADIO;
	m_json["rows"] = GetCheckedRadioButton(IDC_ROWS_SINGLE_RADIO, IDC_ROWS_DOUBLE_RADIO) - IDC_ROWS_SINGLE_RADIO;
	m_json["sideLines"] = m_sideLinesCombo.GetCurSel();
	m_json["frontLine"] = m_frontLinesCombo.GetCurSel();
	m_json["rearLine"] = m_rearLinesCombo.GetCurSel();
	m_json["shortPaintLength"] = GetFloatValue(IDC_SHORT_PAINT_EDIT, 0.5);
	m_json["length"] = GetFloatValue(IDC_LENGTH_EDIT, 5.0);
	m_json["width"] = GetFloatValue(IDC_WIDTH_EDIT, 2.5);
	m_json["angle"] = GetFloatValue(IDC_ANGLE_EDIT, 90.0);
	m_json["capWidth"] = GetFloatValue(IDC_CAP_WIDTH_EDIT, 2.0);
	m_json["capFrontExt"] = GetFloatValue(IDC_CAP_EXT_EDIT, 0.5);
	m_json["showBays"] = BST_CHECKED == IsDlgButtonChecked(IDC_SHOWBAYSCHK);
	m_json["showCaps"] = BST_CHECKED == IsDlgButtonChecked(IDC_SHOWCAPSCHK);
	m_json["showArrows"] = BST_CHECKED == IsDlgButtonChecked(IDC_SHOWARROWSCHK);
	m_json["showPaths"] = BST_CHECKED == IsDlgButtonChecked(IDC_SHOWPATHSCHK);
}

std::string CmfcDlg::Serialize() const
{
	return m_json.dump();
}

void CmfcDlg::OnChangeParameter()
{
	Gather();
	RedrawParking();
}
