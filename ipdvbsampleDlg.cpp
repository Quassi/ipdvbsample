// ipdvbsampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ipdvbsample.h"
#include "ipdvbsampleDlg.h"

#include "wnaspi32.h"
#include "scsidefs.h"
#include "myscsi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIpdvbsampleDlg dialog

CIpdvbsampleDlg::CIpdvbsampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIpdvbsampleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIpdvbsampleDlg)
	m_mac = _T("");
	m_pid = 0;
	m_trsh = 0;
	m_polls = _T("");
	m_thetime = _T("");
	m_status = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIpdvbsampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIpdvbsampleDlg)
	DDX_Control(pDX, IDC_SLIDER_POLLTIME, m_polltime);
	DDX_Text(pDX, IDC_EDIT_MAC, m_mac);
	DDX_Text(pDX, IDC_EDIT_PID, m_pid);
	DDX_Text(pDX, IDC_EDIT_TRSH, m_trsh);
	DDX_Text(pDX, IDC_STATIC_POLLS, m_polls);
	DDX_Text(pDX, IDC_STATIC_THETIME, m_thetime);
	DDX_Text(pDX, IDC_STATIC_STATUS, m_status);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIpdvbsampleDlg, CDialog)
	//{{AFX_MSG_MAP(CIpdvbsampleDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_TOFILE, OnButtonTofile)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIpdvbsampleDlg message handlers

BOOL CIpdvbsampleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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
	
	// TODO: Add extra initialization here
	
	//----init for the slider (polltime)
	m_polltime.SetRangeMin(1, FALSE );
	m_polltime.SetRangeMax(1000, FALSE);
	m_polltime.SetPos(50);
	time=m_polltime.GetPos();
	wsprintf(out_string, "%d ms", time);
	m_thetime=CString(out_string);

	//----init the SCSI
	mmfound=false;

	uc_hosts=0;
	uc_host=0;
	uc_id=0;
	
	uc_hosts=SCSIHosts();

	for (uc_host=0; uc_host<uc_hosts; uc_host++ )
	{
		for (uc_id=0; uc_id<16; uc_id++)
		{
			if ( SCSIInquiry( uc_host, uc_id, buf ) )
			{
				if ( buf[8]=='D' && buf[9]=='V' && buf[10]=='B' && buf[11]=='2' && buf[12]=='0' && buf[13]=='0' && buf[14]=='0' )
				{
					mmfound=true;
					host=uc_host;
					id=uc_id;
				}
			}
		}
	}

	if ( !mmfound )
	{
		MessageBox("I couldn't find her...", "Warning", MB_ICONWARNING);
		m_status="I couldn't find her...";
	}
	else
	{
		wsprintf(out_string, "She's on SCSI adapter %d at ID %d", host, id);
		m_status=CString(out_string);
	}

	//----show the polls on the screen
	wsprintf(out_string, "%d / %d", polls, allpolls);
	m_polls=CString(out_string);

	ul_trsh=65536;	//2^16

	m_mac="00:00:00:00:00:00";
	m_pid=512;
	m_trsh=ul_trsh;
		
	UpdateData(false);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CIpdvbsampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIpdvbsampleDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIpdvbsampleDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CIpdvbsampleDlg::OnButtonTofile() 
{
	if ( mmfound )
	{
		UpdateData(true);

		//----convert the MAC-address to decimal format
		char cHex[2];
		CString test="";
		cHex[0]=m_mac[0];  cHex[1]=m_mac[1];  test=CString(cHex); mac1=HEXtoDEC(test);
		cHex[0]=m_mac[3];  cHex[1]=m_mac[4];  test=CString(cHex); mac2=HEXtoDEC(test);
		cHex[0]=m_mac[6];  cHex[1]=m_mac[7];  test=CString(cHex); mac3=HEXtoDEC(test);
		cHex[0]=m_mac[9];  cHex[1]=m_mac[10]; test=CString(cHex); mac4=HEXtoDEC(test);
		cHex[0]=m_mac[12]; cHex[1]=m_mac[13]; test=CString(cHex); mac5=HEXtoDEC(test);
		cHex[0]=m_mac[15]; cHex[1]=m_mac[16]; test=CString(cHex); mac6=HEXtoDEC(test);
		
		//put the settings into INETbuf[] for SCSIInitMAC()
		INETbuf[0]=(m_pid >> 8) & 0xff;	//PID
		INETbuf[1]=m_pid & 0xff;		//PID
		INETbuf[2]=0x00;				//TABLE-ID
		INETbuf[3]=0x3E;				//TABLE-ID
		INETbuf[4]=0xFF;				//Filter
		INETbuf[5]=0xF2;				//Filter	//0xF1 gives all headers with table-ID 0x3E, to find all MAC's
		INETbuf[6]=mac1;				//MAC-01
		INETbuf[7]=mac2;				//MAC-02
		INETbuf[8]=mac3;				//MAC-03
		INETbuf[9]=mac4;				//MAC-04
		INETbuf[10]=mac5;				//MAC-05
		INETbuf[11]=mac6;				//MAC-06

		//check if the MAC has the right format
		if ( m_mac.GetLength()==17 && m_mac[2]==':' && m_mac[5]==':' && m_mac[8]==':' && m_mac[11]==':' && m_mac[14]==':' )
		{
			//init the orange screen
			if ( SCSIInitMAC( host, id, INETbuf ) )
			{
				GetDlgItem(IDC_BUTTON_TOFILE)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
				GetDlgItem(IDC_SLIDER_POLLTIME)->EnableWindow(FALSE);
				GetDlgItem(IDOK)->EnableWindow(FALSE);

				polls=0;
				allpolls=0;

				//----show the polls on the screen
				wsprintf(out_string, "%d / %d", polls, allpolls);
				m_polls=CString(out_string);
		
				//----open a file
				filename="data.bin";
				dataFile=fopen( filename, "a+b" );
				SetTimer(1, time, NULL);

				m_status="polling...";
				UpdateData(false);
			}
			else { m_status="something went wrong, couldn't init the MAC-screen!!!"; UpdateData(false); }
		}
		else { m_status="wrong MAC-format!!! (00:00:00:00:00:00)"; UpdateData(false); }
	}
}

void CIpdvbsampleDlg::OnButtonStop() 
{
	if ( mmfound )
	{
		SCSIStopStream( host, id );		//kill the orange screen

		GetDlgItem(IDC_BUTTON_TOFILE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER_POLLTIME)->EnableWindow(TRUE);
		GetDlgItem(IDOK)->EnableWindow(TRUE);

		KillTimer(1);

		fclose( dataFile );

		m_status="";
		UpdateData(false);
	}
}

void CIpdvbsampleDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	time=m_polltime.GetPos();
	wsprintf(out_string, "%d ms", time);
	m_thetime=CString(out_string);
	UpdateData(false);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CIpdvbsampleDlg::OnTimer(UINT nIDEvent) 
{
	allpolls++;

	for (cnt=0; cnt<m_trsh; cnt++) buf[cnt]=0;	//clean buf[]
				
	if ( SCSIRead( host, id, 6, buf, 0, m_trsh ) )
	{
		//THE INTERNETDATA IS NOW IN buf[]
		//
		//SO YOU HAVE TO WORK ON THIS DATA
		//
		//FOR EXAMPLE CALL A FUNCTION:
		//WorkOnTheData();
		//
		//NOW I PUT IT ALL IN A FILE:
		fwrite( buf, sizeof( char ), m_trsh, dataFile );

		polls++;
	}

	//----show the polls on the screen
	wsprintf(out_string, "%d / %d", polls, allpolls);
	m_polls=CString(out_string);
		
	UpdateData(false);

	CDialog::OnTimer(nIDEvent);
}

int CIpdvbsampleDlg::HEXtoDEC(CString hex)
{
	char cHex[2];
	int dec=0;

	cHex[0]=hex[0];
	cHex[1]=hex[1];

	if (cHex[0]=='0') dec+=0;
	if (cHex[0]=='1') dec+=16;
	if (cHex[0]=='2') dec+=32;
	if (cHex[0]=='3') dec+=48;
	if (cHex[0]=='4') dec+=64;
	if (cHex[0]=='5') dec+=80;
	if (cHex[0]=='6') dec+=96;
	if (cHex[0]=='7') dec+=112;
	if (cHex[0]=='8') dec+=128;
	if (cHex[0]=='9') dec+=144;
	if (cHex[0]=='a' || cHex[0]=='A') dec+=160;
	if (cHex[0]=='b' || cHex[0]=='B') dec+=176;
	if (cHex[0]=='c' || cHex[0]=='C') dec+=192;
	if (cHex[0]=='d' || cHex[0]=='D') dec+=208;
	if (cHex[0]=='e' || cHex[0]=='E') dec+=224;
	if (cHex[0]=='f' || cHex[0]=='F') dec+=240;

	if (cHex[1]=='0') dec+=0;
	if (cHex[1]=='1') dec+=1;
	if (cHex[1]=='2') dec+=2;
	if (cHex[1]=='3') dec+=3;
	if (cHex[1]=='4') dec+=4;
	if (cHex[1]=='5') dec+=5;
	if (cHex[1]=='6') dec+=6;
	if (cHex[1]=='7') dec+=7;
	if (cHex[1]=='8') dec+=8;
	if (cHex[1]=='9') dec+=9;
	if (cHex[1]=='a' || cHex[1]=='A') dec+=10;
	if (cHex[1]=='b' || cHex[1]=='B') dec+=11;
	if (cHex[1]=='c' || cHex[1]=='C') dec+=12;
	if (cHex[1]=='d' || cHex[1]=='D') dec+=13;
	if (cHex[1]=='e' || cHex[1]=='E') dec+=14;
	if (cHex[1]=='f' || cHex[1]=='F') dec+=15;

	return dec;
}
