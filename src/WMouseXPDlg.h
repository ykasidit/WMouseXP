/*
Copyright (C) 2007-2010 Kasidit Yusuf.

This file is part of WMouseXP.

WMouseXP is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

WMouseXP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with WMouseXP. If not, see <http://www.gnu.org/licenses/>.
*/


// WMouseXPDlg.h : header file
//

#if !defined(AFX_WMOUSEXPDLG_H__BDC293C2_15AE_44D5_857A_6E8BB6123BEB__INCLUDED_)
#define AFX_WMOUSEXPDLG_H__BDC293C2_15AE_44D5_857A_6E8BB6123BEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CWMouseXPDlg dialog




#define WM_TRAY_NOTIFY WM_APP + 1000
#include "mydialog.h"

#include "pictureex.h"
#include "tidyvector.h"
#include "Winsvc.h"

#include "btenginescommons.h"

//#include "BtIfDefinitions.h"
//#include "btwlib.h"

///////////////cximage
//#include "..\cximage600_full\CxImage\ximacfg.h"
//#include "..\cximage600_full\CxImage\ximage.h"
////////////////////

class CWidcommHandler;
class CBlueSoleilHandler;

#define MAX_DEVICE_COUNT 20 
const int INACTIVITY_TIMEOUT_SECONDS = 30;


const int KVirtualKeyLaunchMediaPlayer	= 0x00B5;
const int KVirtualKeyLaunchMyComputer	= 0x00B6;
const int KVirtualKeyLaunchCalculator	= 0x00B7;
const int KVirtualKeyMute				= 0x00AD;
const int KVirtualKeyDecreaseVolume		= 0x00AE;
const int KVirtualKeyIncreaseVolume		= 0x00AF;
const int KVirtualKeyNextTrack			= 0x00B0;
const int KVirtualKeyPrevTrack			= 0x00B1;
const int KVirtualKeyStop				= 0x00B2;
const int KVirtualKeyPlay				= 0x00B3;
const int KVirtualKeyLaunchMail			= 0x00B4;


const int KVirtualKeyBrowserBack		= 0x00A6;
const int KVirtualKeyBrowserNext		= 0x00A7;
const int KVirtualKeyBrowserRefresh		= 0x00A8;
const int KVirtualKeySearch				= 0x00AA;
const int KVirtualKeyBrowserFavorites	= 0x00AB;
const int KVirtualKeyBrowserLaunchHome	= 0x00AC;

enum TNumpads
	{
		ENum1=0,
		ENum2,
		ENum3,
		ENum4,
		ENum5,
		ENum6,
		ENum7,
		ENum8,
		ENum9,
		ENumStar,
		ENum0,
		ENumHash
	};

const int KModMaskCtrl = 1;
const int KModMaskAlt = 2;
const int KModMaskShift = 4;





class CToshHandler;



class CWMouseXPDlg : public CMyDialog,public MRemoteControlCaller
{
 


	//////////////////////////
// Construction
public:
	CWMouseXPDlg(CWnd* pParent = NULL);	// standard constructor
	~CWMouseXPDlg();
// Dialog Data
	//{{AFX_DATA(CWMouseXPDlg)
	enum { IDD = IDD_WMOUSEXP_DIALOG };
	CButton	m_tc;
	CButton	m_ConnectOnStartCheck;
	CStatic	iStep1Label;
	CButton	m_PrevDevCheckBox;
	
	CPictureEx	m_InstallPic;
	CButton	m_InstallOnPhoneButton;
	
	CPictureEx	m_loadingGif;
	CButton	iConnectButton;
	CStatic	iStatusLabel;
	
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWMouseXPDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support	
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CWMouseXPDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnSize(UINT nType, int cx, int cy);	
	afx_msg void OnClose();
	afx_msg void OnCancelMode();
	afx_msg void OnButton();
	afx_msg void OnButton2();
	
	afx_msg void OnMenuAbout();
	afx_msg void OnMenuHelp();
	
	afx_msg void OnButton3();
	
	afx_msg void OnCheck1();
	afx_msg void OnOptions();
	afx_msg void OnCheck2();
	afx_msg void OnOptionsCustomizecommands();
	afx_msg void OnConnectOnStartCheck();
	afx_msg void OnCosCheck();
	afx_msg void OnCheck3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()



public:
		void LoadCustomCommands();
		void SaveCustomCommands();

		bool MSStackRadioFound();


		bool WidcommRadioFound(CWidcommHandler* handler);

	
	
		LONG CWMouseXPDlg::OnTrayNotify( WPARAM wParam, LPARAM lParam );


		void LoadToTray( CWnd    *pWnd, 
	  	                      UINT	  uCallbackMessage,
				              CString sInfoTitle, // title for a balloon ToolTip.
												  // This title appears in boldface above the text.
												  // It can have a maximum of 63 TCHARacters
				              CString sInfo, // the text for a balloon ToolTip, it can have
							                 // a maximum of 255 TCHARacters
				              CString sTip, // the text for a standard ToolTip. 
											// It can have a maximum of 128 TCHARacters, 
											// including the terminating NULL.
			                  int     uTimeout, // in sec.
				              HICON	  icon );

			NOTIFYICONDATA _tnd;
				bool added;



					int SDPGetPort( const char *addr, LPGUID guid );
	
	

	bool StartBTInteractionTimer();
	
	

	bool StopBTInteractionTimer();

	void OnRFCOMMWriteResult(int len, int err, char* desc);

	int RecvViaSocketL(char* buff, int len);//start timer, transfer, throw if failed (failure could be caused by time-out)

	int SendViaSocketL(const char* buff, int len);

	void AddLog(LPCSTR log);

	bool GetFStatus(LPCSTR fn, CFileStatus& fstatus );	
	void SimulateKey(int KeyCode, int mod=0);

	CString GetProgramDir()
{
    CString RtnVal;
    char    FileName[MAX_PATH];
    GetModuleFileName(AfxGetInstanceHandle(), FileName, MAX_PATH);
    RtnVal = FileName;
    RtnVal = RtnVal.Left(RtnVal.ReverseFind('\\'));
    return RtnVal;
};

	char iDevAddr[MAX_DEVICE_COUNT][40];
	CString iDevName[MAX_DEVICE_COUNT];	
	
	//SOCKADDR_BTH iSockAddr;

	CAnimateCtrl *iPlayer;	

	CString iFPath;
	
	DWORD iThreadID ;
	DWORD iRegThreadID;

	bool iWaitingForBTData;

	int m_TimerID;
	
	CTime iLastBTInteraction;	

	static void CALLBACK EXPORT TimerProc(
   HWND hWnd,      // handle of CWnd that called SetTimer
   UINT nMsg,      // WM_TIMER
   UINT nIDEvent,   // timer identification
   DWORD dwTime    // system time
									);


	


	bool InitSocket();

	int iNFoundDevices;
	
	SOCKET iSocket;
	
	static CWMouseXPDlg* curInstance;

	bool StartCommunication(char* devaddr, int port);		

	void MouseMoveAbsolute(int x, int y );
	
	void HandleCommand(BYTE* buf, int len);
	bool TryHandleJoyStickEvent(int presstype, int code);
	void MouseMoveRelative(int x, int y );

	
	//mouse move thread values	
	DWORD iMoverThreadID ;
	static int Move(void *caller);
	bool stopMoving;
	bool moveX;
	int moveDiff;
	int moveSleep;
	bool StartMover(bool xAxis, int diff, int sleepTime);
	void StopMover();

	DWORD iConnectOnStartThreadID;
	static int DoConnectOnStart(void *caller);	
	
	int prevCode;
	
	bool iJoyRepeatedForRightClick;
				
	TidyVector iFoundWMouseXPDevices;	

	bool SendFile(char* devaddr, int port, LPTSTR fpath);

	bool SendFile(char* devaddr, int port, BYTE* dataptr, DWORD datalen);

	void  HandleBTConnected();//set iprevdev instead

	 CToolTipCtrl* m_pToolTip;

	 char iPrevDev[40];

	 CWidcommHandler *iWidcommHandler;
	 bool iMSStackSupported;
	 CBlueSoleilHandler *iBlueSoleilHandler;
	 CToshHandler* iToshHandler;

	 //toshiba message handlers
	 long ToshBtDevSearchRes(UINT wParam, LONG lParam);
	 //////////

	 int iStackUsed;

	 ///////////////////////From MRemoteControlCaller
	void OnData(void *p_data, UINT len);	
	void OnConnected(BYTE* addr, int len);
	void OnDisconnected();
	/////////////////////////////////////////

	void StopNokiaPCSuiteServiceLayer();
	void StartNokiaPCSuiteServiceLayer();
	bool iNokiaPCSuiteServiceLayerStopped;

	
	SERVICE_STATUS status;	

	 /*
	//WIDDCOMM CBTIF 
public:
	void OnDeviceResponded (BD_ADDR bda, DEV_CLASS devClass, BD_NAME bdName, BOOL bConnected);
    void OnDiscoveryComplete ();
    void OnInquiryComplete (BOOL success, short num_responses);

    int m_num_services_per_device;
#define MAX_SERVICES_PER_DEVICE    20
    CSdpDiscoveryRec    m_si[MAX_SERVICES_PER_DEVICE];

    int m_num_devices;
#define MAX_SERVER_DEVICES 20
    CBdInfo  *           m_pDevInfo[MAX_SERVER_DEVICES];  */ 

	 

	bool StartResumeBTConnectionTimer();
	bool StopResumeBTConnectionTimer();
	static void CALLBACK EXPORT ResumeBTConnectionProc(
   HWND hWnd,      // handle of CWnd that called SetTimer
   UINT nMsg,      // WM_TIMER
   UINT nIDEvent,   // timer identification
   DWORD dwTime    // system time
									);
	int mResumeBTConnectionTimer;
	


	void RevertToIdle();

	bool iThereWasASession;


	int iSpeedOption;
	int iAccelerationOption;	

	
	void DefaultCmdKeyMap();
	
	int iNumpadCmdMap[12];
	CString iNumpadScripts[12];
	int iNumpadModifiers[12];


	void StartConnect();
	///////persistant connector
	bool iAutoConnectCommand;
	int m_AutoConnectTimerID;
	bool StartAutoConnectTimer();
	bool StopAutoConnectTimer();
	void TryConnectSilentlyIfIdle();
	static void CALLBACK EXPORT AutoConnectTimerProc(
   HWND hWnd,      // handle of CWnd that called SetTimer
   UINT nMsg,      // WM_TIMER
   UINT nIDEvent,   // timer identification
   DWORD dwTime    // system time
									);

	int iAutoConnectDelay;
	//////////////////

	bool iNotInitDontShowWindow;

	bool iAutoStarted;

	
	/* Omitted - screenshot scaling
	CxImage iCxImage;
	BYTE* iImgEncoded;
	USHORT iImgEncodedLength;
	USHORT iImgSendWrittenLength;

	bool SendImgEncodedToPhone();
	
	*/

	
	
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WMOUSEXPDLG_H__BDC293C2_15AE_44D5_857A_6E8BB6123BEB__INCLUDED_)
