
// mfcDlg.h : header file
//

#pragma once
#include "BCGPDialog.h"
#include "BCGPComboBox.h"
#include "SVGImageStatic.h"
#include <nlohmann\json.hpp>


// CmfcDlg dialog
class CmfcDlg : public CBCGPDialog
{
// Construction
public:
	CmfcDlg(CWnd *pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange *pDX);	// DDX/DDV support

	void RedrawParking();
	void Gather();
	std::string Serialize() const;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnChangeParameter();

private:
	nlohmann::json m_json;
	CSVGImageStatic m_svgCtrl;
	CBCGPComboBox m_sideLinesCombo;
	CBCGPComboBox m_frontLinesCombo;
	CBCGPComboBox m_rearLinesCombo;

	DECLARE_MESSAGE_MAP()
	};
