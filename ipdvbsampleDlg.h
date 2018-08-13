// ipdvbsampleDlg.h : header file
//

#if !defined(AFX_IPDVBSAMPLEDLG_H__20181DA6_E78E_11D3_838D_CE68F49C300A__INCLUDED_)
#define AFX_IPDVBSAMPLEDLG_H__20181DA6_E78E_11D3_838D_CE68F49C300A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CIpdvbsampleDlg dialog

class CIpdvbsampleDlg : public CDialog
{
// Construction
public:
	int HEXtoDEC(CString hex);
	int time;
	char out_string[100];
	int polls, allpolls;

	int mac1, mac2, mac3, mac4, mac5, mac6;

	int cnt;
	
	unsigned char uc_hosts;
	unsigned char uc_host;
	unsigned char uc_id;
	unsigned long ul_trsh;
	
	unsigned char buf[65536];
	unsigned char INETbuf[12];

	CString filename;
	FILE *dataFile;

	//SCSI
	bool mmfound;
	unsigned char host;
	unsigned char id;


	CIpdvbsampleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CIpdvbsampleDlg)
	enum { IDD = IDD_IPDVBSAMPLE_DIALOG };
	CSliderCtrl	m_polltime;
	CString	m_mac;
	int		m_pid;
	int		m_trsh;
	CString	m_polls;
	CString	m_thetime;
	CString	m_status;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIpdvbsampleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CIpdvbsampleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonTofile();
	afx_msg void OnButtonStop();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPDVBSAMPLEDLG_H__20181DA6_E78E_11D3_838D_CE68F49C300A__INCLUDED_)
