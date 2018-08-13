#include "stdafx.h"
#include "MMSYSTEM.H"

#include "wnaspi32.h"
#include "scsidefs.h"
#include "myscsi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

char			txSpeed[100];
bool			box_flag=0;
unsigned long	ASPIStatus;
unsigned long	scs_time, scs_speed;

static SRB_ExecSCSICmd ExecSRB;
HANDLE ASPICompletionEvent;


///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char SCSIHosts()
 {
  unsigned char l_hosts=0;

  ASPIStatus = GetASPI32SupportInfo();
  if (HIBYTE(LOWORD(ASPIStatus)) == SS_COMP)
  {
	  l_hosts =(unsigned char)(LOBYTE(LOWORD(ASPIStatus)));
  }

  return l_hosts;
}


bool SCSICommand()
{
	DWORD ASPIEventStatus;

	SendASPI32Command(&ExecSRB);
	if (ExecSRB.SRB_Status == SS_PENDING)
	{
		ASPIEventStatus = WaitForSingleObject(ASPICompletionEvent, 0xffffffff);
		if (ASPIEventStatus == WAIT_OBJECT_0)  ResetEvent(ASPICompletionEvent);
	}
	CloseHandle(ASPICompletionEvent);
	return (ExecSRB.SRB_Status == SS_COMP);
}

/////////////////////////////////////////////////////////////////////////////////////////////
bool SCSIStopStream(unsigned char host, unsigned char id)
{
	ExecSRB.SRB_Cmd	= SC_EXEC_SCSI_CMD;
	ExecSRB.SRB_HaId = host;
	ASPICompletionEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	ExecSRB.SRB_Flags = SRB_DIR_OUT | SRB_EVENT_NOTIFY;
	ExecSRB.SRB_PostProc = ASPICompletionEvent;
	ExecSRB.SRB_Target = id;
	ExecSRB.SRB_BufPointer = NULL;
	ExecSRB.SRB_BufLen = 0;							// 0 length is "STOP"
	ExecSRB.SRB_SenseLen = SENSE_LEN;
	ExecSRB.SRB_CDBLen = 10;
	ExecSRB.CDBByte[0] = 0x3d;						// Init Stream
	ExecSRB.CDBByte[1] = 6;
	ExecSRB.CDBByte[2] = 0;
	ExecSRB.CDBByte[3] = 0;
	ExecSRB.CDBByte[4] = 0;
	ExecSRB.CDBByte[5] = 0;
	ExecSRB.CDBByte[6] = 0;
	ExecSRB.CDBByte[7] = 0;
	ExecSRB.CDBByte[8] = 0;							// 0 length is "STOP"
	ExecSRB.CDBByte[9] = 0;

	return SCSICommand();
}

/////////////////////////////////////////////////////////////////////////////////////////////
bool SCSIInitStream(unsigned char host, unsigned char id, unsigned int pid, unsigned int trsh, unsigned int mode)
{
	unsigned char buf[256];

	buf[0] = pid>>8;
	buf[1] = pid;
	buf[2] = trsh>>8;
	buf[3] = trsh;
	buf[4] = mode>>8;
	buf[5] = mode;
		
	ExecSRB.SRB_Cmd	= SC_EXEC_SCSI_CMD;
	ExecSRB.SRB_HaId = host;
	ASPICompletionEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	ExecSRB.SRB_Flags = SRB_DIR_OUT | SRB_EVENT_NOTIFY;
	ExecSRB.SRB_PostProc = ASPICompletionEvent;
	ExecSRB.SRB_Target = id;
	ExecSRB.SRB_BufPointer = buf;
	ExecSRB.SRB_BufLen = 6;
	ExecSRB.SRB_SenseLen = SENSE_LEN;
	ExecSRB.SRB_CDBLen = 10;
	ExecSRB.CDBByte[0] = 0x3d;						// Init Stream
	ExecSRB.CDBByte[1] = 6;
	ExecSRB.CDBByte[2] = 0;
	ExecSRB.CDBByte[3] = 0;
	ExecSRB.CDBByte[4] = 0;
	ExecSRB.CDBByte[5] = 0;
	ExecSRB.CDBByte[6] = 0;
	ExecSRB.CDBByte[7] = 0;
	ExecSRB.CDBByte[8] = 6;
	ExecSRB.CDBByte[9] = 0;

	return SCSICommand();
}



/////////////////////////////////////////////////////////////////////////////////////////////
bool SCSIInquiry(unsigned char host, unsigned char id, unsigned char *buf)
{
	unsigned int cnt;

	for (cnt=0; cnt<=65535; cnt++) buf[cnt]=0;

	ExecSRB.SRB_Cmd	= SC_EXEC_SCSI_CMD;
	ExecSRB.SRB_HaId = host;
	ASPICompletionEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	ExecSRB.SRB_Flags = SRB_DIR_IN | SRB_EVENT_NOTIFY;
	ExecSRB.SRB_PostProc = ASPICompletionEvent;
	ExecSRB.SRB_Target = id;
	ExecSRB.SRB_BufPointer = buf;
	ExecSRB.SRB_BufLen = 36;
	ExecSRB.SRB_SenseLen = SENSE_LEN;
	ExecSRB.SRB_CDBLen = 6;								// setup command length
	ExecSRB.CDBByte[0] = 0x12;							// inquiry
	ExecSRB.CDBByte[1] = 0;								// unit|reserved|EVPD
	ExecSRB.CDBByte[2] = 0;								// page code
	ExecSRB.CDBByte[3] = 0;								// reserved
	ExecSRB.CDBByte[4] = 36;							// transfer length
	ExecSRB.CDBByte[5] = 0;								// control
	
	return SCSICommand();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool SCSIRezero(unsigned char host, unsigned char id)
{
	ExecSRB.SRB_Cmd	= SC_EXEC_SCSI_CMD;
	ExecSRB.SRB_HaId = host;
	ASPICompletionEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	ExecSRB.SRB_Flags = SRB_DIR_OUT | SRB_EVENT_NOTIFY;
	ExecSRB.SRB_PostProc = ASPICompletionEvent;
	ExecSRB.SRB_Target = id;
	ExecSRB.SRB_BufPointer = 0;
	ExecSRB.SRB_BufLen = 0;
	ExecSRB.SRB_SenseLen = SENSE_LEN;
	ExecSRB.SRB_CDBLen = 6;								// setup command length
	ExecSRB.CDBByte[0] = 0x01;							// Rezero
	ExecSRB.CDBByte[1] = 0;								// unit|reserved|EVPD
	ExecSRB.CDBByte[2] = 0;								// page code
	ExecSRB.CDBByte[3] = 0;								// reserved
	ExecSRB.CDBByte[4] = 0;								// transfer length
	ExecSRB.CDBByte[5] = 0;								// control
	
	return SCSICommand();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool SCSIWriteFW(unsigned char host, unsigned char id, unsigned char *buf, unsigned long adr, unsigned long len)
{
	len++;
	len&=0xfffffe;

	ExecSRB.SRB_Cmd	= SC_EXEC_SCSI_CMD;
	ExecSRB.SRB_HaId = host;
	ASPICompletionEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	ExecSRB.SRB_Flags = SRB_DIR_OUT | SRB_EVENT_NOTIFY;
	ExecSRB.SRB_PostProc = ASPICompletionEvent;
	ExecSRB.SRB_Target = id;
	ExecSRB.SRB_BufPointer = buf;
	ExecSRB.SRB_BufLen = len;
	ExecSRB.SRB_SenseLen = SENSE_LEN;
	ExecSRB.SRB_CDBLen = 10;
	ExecSRB.CDBByte[0] = 0x3b;								// write buffer
	ExecSRB.CDBByte[1] = 4;									// real write command
	ExecSRB.CDBByte[2] = (unsigned char)((adr >> 24) & 0xff);
	ExecSRB.CDBByte[3] = (unsigned char)((adr >> 16) & 0xff);
	ExecSRB.CDBByte[4] = (unsigned char)((adr >> 8) & 0xff);
	ExecSRB.CDBByte[5] = (unsigned char)(adr & 0xff);
	ExecSRB.CDBByte[6] = (unsigned char)((len >> 16) & 0xff);
	ExecSRB.CDBByte[7] = (unsigned char)((len >> 8) & 0xff);
	ExecSRB.CDBByte[8] = (unsigned char)(len & 0xff);
	ExecSRB.CDBByte[9] = 0;
	
	return SCSICommand();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool SCSIWrite(unsigned char host, unsigned char id, unsigned char cdb1, unsigned char *buf, unsigned long adr, unsigned long len)
{
	if (!box_flag) return FALSE;

	len++;
	len&=0xfffffe;

	ExecSRB.SRB_Cmd	= SC_EXEC_SCSI_CMD;
	ExecSRB.SRB_HaId = host;
	ASPICompletionEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	ExecSRB.SRB_Flags = SRB_DIR_OUT | SRB_EVENT_NOTIFY;
	ExecSRB.SRB_PostProc = ASPICompletionEvent;
	ExecSRB.SRB_Target = id;
	ExecSRB.SRB_BufPointer = buf;
	ExecSRB.SRB_BufLen = len;
	ExecSRB.SRB_SenseLen = SENSE_LEN;
	ExecSRB.SRB_CDBLen = 10;
	ExecSRB.CDBByte[0] = 0x3b;								// write buffer
	ExecSRB.CDBByte[1] = cdb1;								// real write command
	ExecSRB.CDBByte[2] = (unsigned char)((adr >> 24) & 0xff);
	ExecSRB.CDBByte[3] = (unsigned char)((adr >> 16) & 0xff);
	ExecSRB.CDBByte[4] = (unsigned char)((adr >> 8) & 0xff);
	ExecSRB.CDBByte[5] = (unsigned char)(adr & 0xff);
	ExecSRB.CDBByte[6] = (unsigned char)((len >> 16) & 0xff);
	ExecSRB.CDBByte[7] = (unsigned char)((len >> 8) & 0xff);
	ExecSRB.CDBByte[8] = (unsigned char)(len & 0xff);
	ExecSRB.CDBByte[9] = 0;
	
	return SCSICommand();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SCSIInitMStream(unsigned char host, unsigned char id, unsigned char cdb1, unsigned char *buf, unsigned long adr, unsigned long len)
{
	if (!box_flag) return FALSE;

	len++;
	len&=0xfffffe;

	ExecSRB.SRB_Cmd	= SC_EXEC_SCSI_CMD;
	ExecSRB.SRB_HaId = host;
	ASPICompletionEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	ExecSRB.SRB_Flags = SRB_DIR_OUT | SRB_EVENT_NOTIFY;
	ExecSRB.SRB_PostProc = ASPICompletionEvent;
	ExecSRB.SRB_Target = id;
	ExecSRB.SRB_BufPointer = buf;
	ExecSRB.SRB_BufLen = len;
	ExecSRB.SRB_SenseLen = SENSE_LEN;
	ExecSRB.SRB_CDBLen = 10;
	ExecSRB.CDBByte[0] = 0x3E;								// write buffer
	ExecSRB.CDBByte[1] = cdb1;								// real write command
	ExecSRB.CDBByte[2] = (unsigned char)((adr >> 24) & 0xff);
	ExecSRB.CDBByte[3] = (unsigned char)((adr >> 16) & 0xff);
	ExecSRB.CDBByte[4] = (unsigned char)((adr >> 8) & 0xff);
	ExecSRB.CDBByte[5] = (unsigned char)(adr & 0xff);
	ExecSRB.CDBByte[6] = (unsigned char)((len >> 16) & 0xff);
	ExecSRB.CDBByte[7] = (unsigned char)((len >> 8) & 0xff);
	ExecSRB.CDBByte[8] = (unsigned char)(len & 0xff);
	ExecSRB.CDBByte[9] = 0;
	return SCSICommand();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool SCSIInitMAC( unsigned char host, unsigned char id, unsigned char buf[12] )
{
	unsigned long adr=0;
	unsigned char cdb1=0;
	unsigned long len=12;

	BYTE szBuffer[12];

	szBuffer[0]=buf[0];		//PID
	szBuffer[1]=buf[1];		//PID
	szBuffer[2]=buf[2];		//TABLE-ID
	szBuffer[3]=buf[3];		//TABLE-ID
	szBuffer[4]=buf[4];		//Filter
	szBuffer[5]=buf[5];		//Filter
	szBuffer[6]=buf[6];		//MAC-01
	szBuffer[7]=buf[7];		//MAC-02
	szBuffer[8]=buf[8];		//MAC-03
	szBuffer[9]=buf[9];		//MAC-04
	szBuffer[10]=buf[10];	//MAC-05
	szBuffer[11]=buf[11];	//MAC-06
	len=12;
	
	ExecSRB.SRB_Cmd	= SC_EXEC_SCSI_CMD;
	ExecSRB.SRB_HaId = host;
	ASPICompletionEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	ExecSRB.SRB_Flags = SRB_DIR_OUT | SRB_EVENT_NOTIFY;
	ExecSRB.SRB_PostProc = ASPICompletionEvent;
	ExecSRB.SRB_Target = id;
	ExecSRB.SRB_BufPointer = szBuffer;
	ExecSRB.SRB_BufLen = len;
	ExecSRB.SRB_SenseLen = SENSE_LEN;
	ExecSRB.SRB_CDBLen = 10;
	ExecSRB.CDBByte[0] = 0x3F;								// write buffer
	ExecSRB.CDBByte[1] = cdb1;								// real write command
	ExecSRB.CDBByte[2] = (unsigned char)((adr >> 24) & 0xff);
	ExecSRB.CDBByte[3] = (unsigned char)((adr >> 16) & 0xff);
	ExecSRB.CDBByte[4] = (unsigned char)((adr >> 8) & 0xff);
	ExecSRB.CDBByte[5] = (unsigned char)(adr & 0xff);
	ExecSRB.CDBByte[6] = (unsigned char)((len >> 16) & 0xff);
	ExecSRB.CDBByte[7] = (unsigned char)((len >> 8) & 0xff);
	ExecSRB.CDBByte[8] = (unsigned char)(len & 0xff);
	ExecSRB.CDBByte[9] = 0;
	
	return SCSICommand();
}



bool SCSIWriteBig(unsigned char host, unsigned char id, unsigned char cdb1, unsigned char *buf, unsigned long adr, unsigned long rest)
{
	unsigned long	len;

	if (!box_flag) return FALSE;

	while(rest>0)
		{
		len=rest;
		if (rest>65536) len=65536;
		if (!SCSIWrite(host,id, cdb1,  buf, adr, len)) return FALSE;
		adr+=len;
		buf+=len;
		rest-=len;
		};
 	return TRUE;
}


bool SCSIRead(unsigned char host, unsigned char id, unsigned char cdb1, unsigned char *buf, unsigned long adr, unsigned long len)
{
	//unsigned long adr=0xd0000;	// memory location of channels on DVB98
	//unsigned char cdb1=0;
	//BYTE buf[12];
	//unsigned long len=0;

	ExecSRB.SRB_Cmd	= SC_EXEC_SCSI_CMD;
	ExecSRB.SRB_HaId = host;
	ASPICompletionEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	ExecSRB.SRB_Flags = SRB_DIR_IN | SRB_EVENT_NOTIFY;
	ExecSRB.SRB_PostProc = ASPICompletionEvent;
	ExecSRB.SRB_Target = id;
	ExecSRB.SRB_BufPointer = buf;
	ExecSRB.SRB_BufLen = len;
	ExecSRB.SRB_SenseLen = SENSE_LEN;
	ExecSRB.SRB_CDBLen = 10;
	ExecSRB.CDBByte[0] = 0x3c;								// read buffer (from box);
	ExecSRB.CDBByte[1] = cdb1;								// mode
	ExecSRB.CDBByte[2] = (unsigned char)((adr >> 24) & 0xff);
	ExecSRB.CDBByte[3] = (unsigned char)((adr >> 16) & 0xff);
	ExecSRB.CDBByte[4] = (unsigned char)((adr >> 8) & 0xff);
	ExecSRB.CDBByte[5] = (unsigned char)(adr & 0xff);
	ExecSRB.CDBByte[6] = (unsigned char)((len >> 16) & 0xff);
	ExecSRB.CDBByte[7] = (unsigned char)((len >> 8) & 0xff);
	ExecSRB.CDBByte[8] = (unsigned char)(len & 0xff);
	ExecSRB.CDBByte[9] = 0;
	
	return SCSICommand();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool SCSIReadBig(unsigned char host, unsigned char id, unsigned char cdb1, unsigned char *buf, unsigned long adr, unsigned long rest)
{
	unsigned long len;

	if (!box_flag) return FALSE;

	while(rest>0)
	{
		len=rest;
		if (rest>65536) len=65536;
		//if (!SCSIRead(host,id, cdb1,  buf, adr, len)) return FALSE;
		adr+=len;
		buf+=len;
		rest-=len;
	}

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////////////////////
unsigned char SCSISpeed(unsigned char host, unsigned char id, unsigned char *buf)
{
	int nBytesRemaining;
	int nTransferCount;
	DWORD dwStartTime, dwEndTime;

	dwStartTime = timeGetTime();
	nBytesRemaining = 1024 * 1024;
	do
	{
		nTransferCount = nBytesRemaining;
		if (nTransferCount > 65536) nTransferCount = 65536;

		ExecSRB.SRB_Cmd	= SC_EXEC_SCSI_CMD;
		ExecSRB.SRB_HaId = host;
		ASPICompletionEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
		ExecSRB.SRB_Flags = SRB_DIR_OUT | SRB_EVENT_NOTIFY;
		ExecSRB.SRB_PostProc = ASPICompletionEvent;
		ExecSRB.SRB_Target = id;
		ExecSRB.SRB_BufPointer = buf;
		ExecSRB.SRB_BufLen = nTransferCount;
		ExecSRB.SRB_SenseLen = SENSE_LEN;
		ExecSRB.SRB_CDBLen = 10;
		ExecSRB.CDBByte[0] = 0x3b;								// write buffer
		ExecSRB.CDBByte[1] = 0;									// test mode
		ExecSRB.CDBByte[2] = 0;
		ExecSRB.CDBByte[3] = 0;
		ExecSRB.CDBByte[4] = 0;
		ExecSRB.CDBByte[5] = 0;
		ExecSRB.CDBByte[6] = (unsigned char)((nTransferCount >> 16) & 0xff);
		ExecSRB.CDBByte[7] = (unsigned char)((nTransferCount >> 8) & 0xff);
		ExecSRB.CDBByte[8] = (unsigned char)(nTransferCount & 0xff);
		ExecSRB.CDBByte[9] = 0;

		if (!SCSICommand()) return 1;

		nBytesRemaining -= nTransferCount;
	} while (nBytesRemaining != 0);

	dwEndTime = timeGetTime();
	scs_time  = dwEndTime - dwStartTime;
	scs_speed = (1024 * 1024) / (dwEndTime - dwStartTime);

	return 0;
	//return scs_speed;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
void SCSIChangeChannel(unsigned char host, unsigned char id, int nChannel)
{
	BYTE szBuffer[2];

	szBuffer[0] = (nChannel >> 8) & 0xff;
	szBuffer[1] = nChannel & 0xff;

	ExecSRB.SRB_Cmd	= SC_EXEC_SCSI_CMD;
	ExecSRB.SRB_HaId = host;
	ASPICompletionEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	ExecSRB.SRB_Flags = SRB_DIR_OUT | SRB_EVENT_NOTIFY;
	ExecSRB.SRB_PostProc = ASPICompletionEvent;
	ExecSRB.SRB_Target = id;
	ExecSRB.SRB_BufPointer = szBuffer;
	ExecSRB.SRB_BufLen = 2;
	ExecSRB.SRB_SenseLen = SENSE_LEN;
	ExecSRB.SRB_CDBLen = 10;
	ExecSRB.CDBByte[0] = 0x3d;		// setup streaming mode
	ExecSRB.CDBByte[1] = 1;
	ExecSRB.CDBByte[2] = 0;
	ExecSRB.CDBByte[3] = 0;
	ExecSRB.CDBByte[4] = 0;
	ExecSRB.CDBByte[5] = 0;
	ExecSRB.CDBByte[6] = 0;
	ExecSRB.CDBByte[7] = 0;
	ExecSRB.CDBByte[8] = 2;
	ExecSRB.CDBByte[9] = 0;

	SCSICommand();
}



///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char SCSISetup()
{
	/*
	unsigned char host=0;
	
	int mmfound=0;

	DWORD (*pSendASPI32Command) (LPSRB);
	DWORD (*pGetASPI32SupportInfo) (VOID);
	
	HANDLE hASPI = NULL;

	DWORD dwASPIStatus;
	SRB_HAInquiry HAInquiry;
	int nNumAdapters;
	int i, j;

	DWORD dwType = REG_SZ;
	int nSCSIState = 0;

	hASPI = LoadLibrary("wnaspi32.dll");
	if (hASPI == NULL)
	{
		return 0;
	}
	
	//-- deze 2 gaan fout
	pSendASPI32Command = (void *)GetProcAddress(hASPI, "SendASPI32Command");
	pGetASPI32SupportInfo = (void *)GetProcAddress(hASPI, "GetASPI32SupportInfo");
	//-- deze 2 gaan fout
	
	if ( (pSendASPI32Command == NULL) || (pGetASPI32SupportInfo == NULL) )
	{
		//FreeLibrary(hASPI);
		hASPI = NULL;
		return 0;
	}
		
	dwASPIStatus = pGetASPI32SupportInfo();
	if (HIBYTE(LOWORD(dwASPIStatus)) == SS_COMP)
		nNumAdapters = (LOWORD(LOBYTE(dwASPIStatus)));
	else
		nNumAdapters = 0;
	if (nNumAdapters == 0)
	{
		//FreeLibrary(hASPI);
		hASPI = NULL;
		return 0;
	}

	// Search through each adapter
	for (i = 0; i < nNumAdapters; i++)
	{
		// Find out SCSI id on this adapter
		HAInquiry.SRB_Cmd = SC_HA_INQUIRY;
		HAInquiry.SRB_HaId = (unsigned char)i;
		HAInquiry.SRB_Flags = 0;		
		pSendASPI32Command(&HAInquiry);

		// Now scan each device except our adapter. Since the
		// d-box is narrow SCSI, don't care about max SCSI id
		for (j = 0; j < 8; j++)
		{
			if (j != (int)HAInquiry.HA_SCSI_ID)
			{
				char szInquiryData[40];
				static SRB_ExecSCSICmd ExecSRB;
				HANDLE ASPICompletionEvent;
				DWORD ASPIEventStatus;
				char * szCompare = &szInquiryData[8];
				char * szVersion = &szInquiryData[32];
				char szDVB98Ident[] = "OVERFLOW DVB98 SCSI";
				char szDVB2000Ident[] = "DVB2000";

				ExecSRB.SRB_Cmd	= SC_EXEC_SCSI_CMD;
				ExecSRB.SRB_HaId = LOBYTE(i);
				ASPICompletionEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
				ExecSRB.SRB_Flags = SRB_DIR_IN | SRB_EVENT_NOTIFY;
				ExecSRB.SRB_PostProc = ASPICompletionEvent;
				ExecSRB.SRB_Target = LOBYTE(j);
				ExecSRB.SRB_BufPointer = (unsigned char *)szInquiryData;
				ExecSRB.SRB_BufLen = 36;
				ExecSRB.SRB_SenseLen = SENSE_LEN;
				ExecSRB.SRB_CDBLen = 6;
				ExecSRB.CDBByte[0] = SCSI_INQUIRY; 
				ExecSRB.CDBByte[1] = 0;
				ExecSRB.CDBByte[2] = 0;
				ExecSRB.CDBByte[3] = 0;
				ExecSRB.CDBByte[4] = 36;
				ExecSRB.CDBByte[5] = 0;

				pSendASPI32Command(&ExecSRB);
				if (ExecSRB.SRB_Status == SS_PENDING)
				{
					ASPIEventStatus = WaitForSingleObject(ASPICompletionEvent, 0xffffffff);
					if (ASPIEventStatus == WAIT_OBJECT_0) 
						ResetEvent(ASPICompletionEvent);

				}
				CloseHandle(ASPICompletionEvent);
				if (ExecSRB.SRB_Status == SS_COMP)
				{
					// Found it!
					//MessageBox( hwnd, "Yep, Mediamaster found...", szAppName, MB_OK | MB_ICONWARNING );
					mmfound=1;
					
					if (   (memcmp(szCompare, szDVB98Ident, sizeof(szDVB98Ident) - 1) == 0)
					    || (memcmp(szCompare, szDVB2000Ident, sizeof(szDVB2000Ident) - 1) == 0) )
					{
						char szTemp[100];

						//szVersion[4] = '\0';
						//sscanf(szVersion, "%d.%d", &nDVB2KMajor, &nDVB2KMinor);
						
						//nAdapterID = i;
						//nSCSIID = j;

						wsprintf(szTemp, "Mediamaster located on SCSI adapter %d at ID %d", i, j);
						//UpdateStatus(szTemp);
						return 0;
					}
				}
			}
		}

		//if ( mmfound==0 && i==nNumAdapters ) MessageBox( hwnd, "Can't find the Mediamaster...", szAppName, MB_OK | MB_ICONSTOP );
	}

	return host;
	*/
	return 0;
}

void SCSISpecialCommand(unsigned char host, unsigned char id, unsigned char command, CString sData, unsigned char cData)
{
	BYTE szBuffer[20];
	byte buflen=0;
	int lengte=0, i=0;

	if (cData==0)
	{	//OSD
		lengte=sData.GetLength();
		buflen=8;
	
		for (i=0; i<8; i++)
		{
			szBuffer[i]=' ';
		}

		szBuffer[0]=command;
		for (i=1; i<=lengte; i++)
		{
			szBuffer[i]=sData[i-1];
		}
	}
	else if (sData=="")
	{	//channelchange
		buflen=3;

		for (i=0; i<8; i++)
		{
			szBuffer[i]=' ';
		}

		szBuffer[0]=command;
		szBuffer[1]=0x00;//(cData >> 8) & 0xff;
		szBuffer[2]=0x08;//cData & 0xff;
	}
	
	if (sData=="" | cData==0)
	{
		ExecSRB.SRB_Cmd	= SC_EXEC_SCSI_CMD;
		ExecSRB.SRB_HaId = host;
		ASPICompletionEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
		ExecSRB.SRB_Flags = SRB_DIR_OUT | SRB_EVENT_NOTIFY;
		ExecSRB.SRB_PostProc = ASPICompletionEvent;
		ExecSRB.SRB_Target = id;
		ExecSRB.SRB_BufPointer = szBuffer;
		ExecSRB.SRB_BufLen = buflen;
		ExecSRB.SRB_SenseLen = SENSE_LEN;
		ExecSRB.SRB_CDBLen = 0x0A;
		ExecSRB.CDBByte[0] = 0x3B;
		ExecSRB.CDBByte[1] = 0x30;
		ExecSRB.CDBByte[2] = 0;
		ExecSRB.CDBByte[3] = 0;
		ExecSRB.CDBByte[4] = 0;
		ExecSRB.CDBByte[5] = 0;
		ExecSRB.CDBByte[6] = 0;
		ExecSRB.CDBByte[7] = 0;
		ExecSRB.CDBByte[8] = buflen;
		ExecSRB.CDBByte[9] = 0;

		SCSICommand();
	}
}
