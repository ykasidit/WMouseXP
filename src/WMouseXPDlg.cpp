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

//(Please note that the source code was written when I was just starting to learn programming - it might be low-standard in quality and/or convention.)

#include "stdafx.h"
#include "WMouseXP.h"
#include "WMouseXPDlg.h"
#include "log.h"

#include <bthsdpdef.h>

#include "BluetoothAPIs.h"
#include <ws2bth.h>
#include "SelectPhoneDialog.h"
#include "WMouseXPMobileDevice.h"
#include "OBEX.h"
#include "optionsdialog.h"

#include "bthdef.h"

#include "btstackenum.h"

#include "widcommhandler.h"
#include "BlueSoleilHandler.h"
#include "CustomCommandDialog.h"

#include <shellapi.h>
#include <winuser.h>
#include <windows.h>
#include <Wtsapi32.h>

#include "ToshHandler.h"
#include "sdptypes.h"
#include "tosbtapi.h"

#include "Winsvc.h"



#pragma comment ( lib, "Irprops.lib")//MS


#pragma comment ( lib, "Ws2_32.lib")	
#pragma comment ( lib, "User32.lib")

//#pragma comment ( lib, "BtWdSdkLib.lib")//Broadcom
#pragma comment ( lib, "WidcommSdklib.lib")//Broadcom



#pragma comment ( lib, "btfunc.lib")//BlueSoleil

#pragma comment ( lib, "TosBtAPI.lib")//Tosh
#pragma comment ( lib, "TosBtObexApi.lib")//Tosh

#pragma comment(linker, "/delayload:btfunc.dll")//BlueSoleil
#pragma comment(linker, "/delayload:wbtapi.dll")//Broadcom

#pragma comment(linker, "/delayload:TosBtAPI.dll")//Tosh
#pragma comment(linker, "/delayload:TosBtObexAPI.dll")//Tosh

#pragma comment(linker, "/delayload:irprops.cpl")//MS

#pragma comment ( lib, "delayimp.lib")

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
	CString	m_AboutVersion;
	CString	m_Cpyr;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnButton1();
	virtual BOOL OnInitDialog();
	afx_msg void OnButton2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)		
	m_AboutVersion = _T("");
	m_Cpyr = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_ABOUT_VERSION, m_AboutVersion);
	DDX_Text(pDX, IDC_CPYR, m_Cpyr);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWMouseXPDlg dialog


CWMouseXPDlg* CWMouseXPDlg::curInstance = 0;


HINSTANCE appInstance;
HWND      appWindow;

//////////////////////////////////////////////////

static
BOOL SetRegistryKey (
  char* keyPath,
  char* keyName,
  char* keyData
) {
  HKEY  key;
  long  error;
  DWORD disposition;

  error = RegCreateKeyEx(
    HKEY_CURRENT_USER, keyPath, 0, NULL, REG_OPTION_NON_VOLATILE,
    KEY_ALL_ACCESS, NULL, &key, &disposition
  );
  if ( error ) return FALSE;

  error = RegSetValueEx(
    key, keyName, 0, REG_SZ, (BYTE*) keyData, lstrlen(keyData) + 1
  );
  RegCloseKey(key);
  if ( error ) return FALSE;

  return TRUE;
}

//////////////////////////////////////////////////

static
BOOL GetRegistryKey (
  char* keyPath,
  char* keyName,
  char* keyData
) {
  HKEY  key;
  long  error;
  char  content[1024];
  DWORD type = REG_SZ;
  DWORD size = 1024;

  error = RegOpenKeyEx(
    HKEY_CURRENT_USER, keyPath, 0, KEY_ALL_ACCESS, &key
  );
  if ( error ) return FALSE;

  error = RegQueryValueEx(
    key, keyName, NULL, &type, (BYTE*) content, &size
  );
  RegCloseKey(key);
  if ( error ) return FALSE;

  lstrcpy(keyData, content);
  return TRUE;
}

//////////////////////////////////////////////////


//numpad input type mapper, this would be multiplied with  the EWMouseXPOpcodeMultiplier
//these are all proper numbers that you would mod the opcode to see the input type of the opcode

const int KAlpha=3;
const int KNumeric=5;
const int KBrowser=7;
const int KMedia=11;
const int KLauncher=13;

//WMouseXPOpcodeMultipliers
const int KKeyPressedCode=1;
const int KKeyRepeatedCode=2;
const int KKeyReleasedCode=4;

const int KRequestSSPNG=5; //next 2 bytes tell width, next 2 tell height, next 1 tell zoomout
const int KRequestSSJPG=6; //next 2 bytes tell width, next 2 tell height, next 1 tell zoomout

const int KJoyUp = 1;
		
const int KJoyDown = 2;
		
const int KJoyLeft = 3;
		
const int KJoyRight = 4;

const int KJoyIn = 5;

const  int KJoyLong = 6; //right click


const  int KEsc = 7; 
const  int KPrevSlide = 8;
const  int KStartPresentation = 9;
const  int KPrevTrack = 10; 
const  int KIncreaseVolume = 11;
const  int KNextTrack = 12;
const  int KStop = 13; 
const  int KDecreaseVolume = 14;
const  int KPlay = 15;
const  int KMyComputer = 16; 
const  int KMediaPlayer = 17;
const  int KMute = 18;





const int KStartSlowMoveSleep = (KMaxSpeed*2)+1;


CWMouseXPDlg::~CWMouseXPDlg()
{
	curInstance = NULL;

	delete m_pToolTip;

	delete iWidcommHandler;
	delete iBlueSoleilHandler;
	delete iToshHandler;

/*	if(CWMouseXPDlg::curInstance->iImgEncoded)
	{
		free(CWMouseXPDlg::curInstance->iImgEncoded);
		CWMouseXPDlg::curInstance->iImgEncoded = NULL;
		CWMouseXPDlg::curInstance->iImgEncodedLength = 0;
	}*/
}

bool MSStackSupported()
{
	HMODULE h = LoadLibrary("irprops.cpl");
		if (h == NULL) {				
		return false;
	}
	FreeLibrary(h);	
	return true;
}




CWMouseXPDlg::CWMouseXPDlg(CWnd* pParent /*=NULL*/)
	: CMyDialog(CWMouseXPDlg::IDD, pParent)
{
	
	iNotInitDontShowWindow = true;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


	added = false;

		iWaitingForBTData = false;

	iNFoundDevices = 0;

	
	iSpeedOption = KDefaultSpeed;
	iAccelerationOption = KDefaultAcceleration;	

	CString optionspath = GetProgramDir();
	optionspath += "\\options.dat";

	
	CFile file;

	if(file.Open(optionspath,CFile::modeRead))
	{
		unsigned char options[2];
		if(file.Read(options,2))
		{
			if(options[0] >= 0 && options[0]<=KMaxSpeed)
			{
				iSpeedOption = options[0];
			}			


			if(options[1] >=0 && options[1]<=KMaxAcceleration)
			{
				iAccelerationOption = options[1];
			}

		}

		file.Close();
	}



	//get from file
	

	m_TimerID = 0;
	
	curInstance = this;

	iMoverThreadID = 0;
	prevCode = 0;

	iJoyRepeatedForRightClick = false;

	m_TrialTimerID =0;
	iRegThreadID = 0;

	 m_pToolTip = NULL;

	 mResumeBTConnectionTimer = NULL;

	 //iImgEncoded = NULL;
	 //iImgEncodedLength = 0;

}

void CWMouseXPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWMouseXPDlg)
	DDX_Control(pDX, IDC_CHECK3, m_tc);	
	DDX_Control(pDX, IDC_STEP1LABEL, iStep1Label);
	DDX_Control(pDX, IDC_CHECK1, m_PrevDevCheckBox);
	
	DDX_Control(pDX, IDC_MYPIC2, m_InstallPic);
	DDX_Control(pDX, IDC_BUTTON1, m_InstallOnPhoneButton);
	
	DDX_Control(pDX, IDC_MYPIC, m_loadingGif);
	DDX_Control(pDX, IDC_BUTTON4, iConnectButton);
	DDX_Control(pDX, IDC_STATUSLABEL, iStatusLabel);
	
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWMouseXPDlg, CDialog)
	//{{AFX_MSG_MAP(CWMouseXPDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_CANCELMODE()
	ON_BN_CLICKED(IDC_BUTTON4, OnButton)		
	ON_COMMAND(ID_MENU_ABOUT, OnMenuAbout)
	ON_COMMAND(ID_MENU_HELP, OnMenuHelp)
	
	
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_COMMAND(ID_OPTIONS, OnOptions)
	ON_BN_CLICKED(IDC_CHECK2, OnCheck2)
	ON_COMMAND(ID_OPTIONS_CUSTOMIZECOMMANDS, OnOptionsCustomizecommands)	
	ON_MESSAGE( WM_TRAY_NOTIFY, OnTrayNotify )	
	ON_BN_CLICKED(IDC_CHECK3, OnCheck3)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_TOSHMESSAGES, ToshBtDevSearchRes)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWMouseXPDlg message handlers


long lMonitorsX, lMonitorsY;

BOOL CALLBACK MonitorEnumProc(
HMONITOR hMonitor, // handle to display monitor
HDC hdcMonitor, // handle to monitor DC
LPRECT lprcMonitor, // monitor intersection rectangle
LPARAM dwData // data
)
{
RECT Rect;
MONITORINFO mi;

mi.cbSize = sizeof( mi );
Rect = *lprcMonitor;
GetMonitorInfo( hMonitor, &mi );

//chad edit - make += instead of max because we want whole multiscreen width
lMonitorsX += mi.rcMonitor.right;
lMonitorsX--;//0 based

lMonitorsY = max( lMonitorsY, mi.rcMonitor.bottom );
lMonitorsY--;//0 based

return TRUE;
}


void RefreshMonitorsMetrics( )
{

// work out how big we have to be to cover all the screens.
lMonitorsX = 0;
lMonitorsY = 0;
EnumDisplayMonitors(
NULL, // handle to display DC 
NULL, // clipping rectangle 
MonitorEnumProc, // callback function
0 // data for callback function 
);



}

/*
typedef struct _TakeSSParamStruct
{
	USHORT iW;
	USHORT iH;
	BYTE   iZoom;
} TakeSSParamStruct;
	  


// CreateDib - By Napalm
HBITMAP CreateDib(int nWidth, int nHeight, int nBitDepth){
    LPBYTE lpDib;
    HBITMAP hbmDib;
    BITMAPINFO bmiDib = { {
        sizeof(BITMAPINFOHEADER), nWidth, nHeight, 1, nBitDepth, BI_RGB, 0, 0, 0, 0, 0,
    } };
    
    HDC hdcScreen = GetDC(HWND_DESKTOP);
    hbmDib = CreateDIBSection(hdcScreen, &bmiDib, DIB_PAL_COLORS, (LPVOID *)&lpDib, NULL, 0);
    ReleaseDC(HWND_DESKTOP, hdcScreen);
    
    return hbmDib;
}


BOOL TakeScreenshot(struct _TakeSSParamStruct& aParams){
     
	POINT p ={0};				
	::GetCursorPos(&p);

	int sx = p.x-(aParams.iW/2);
	if(sx<0)
		sx=0;
	int sy = p.y-(aParams.iH/2);
	if(sy<0)
		sy=0;

	//if, just in case, mouse much out of screen
	if(sy>lMonitorsY-1)
		sy = lMonitorsY-1;
	if(sx>lMonitorsX-1)
		sy = lMonitorsX-1;

	CString str;
	str.Format("sx %d, sy %d",sx,sy);
	mydebug::log(str);

		RECT rcDim = { 
          0 , 0, aParams.iW, aParams.iH
     };
     
     HANDLE hFile;
     DWORD dwOut;
     HBITMAP hbmBitmap;
     HDC hdcScreen, hdcBitmap;
     int nCapture, nHeaders, nImageSize;
     
     hbmBitmap = CreateDib(rcDim.right, rcDim.bottom, 24);
     hdcScreen = GetDC(HWND_DESKTOP);
     hdcBitmap = CreateCompatibleDC(hdcScreen);
     ReleaseDC(HWND_DESKTOP, hdcScreen);
     
     nCapture = SaveDC(hdcBitmap);
     SelectObject(hdcBitmap, hbmBitmap);
     
     hdcScreen = GetDC(NULL);
    
     BitBlt(hdcBitmap,0,0,rcDim.right,rcDim.bottom,hdcScreen,sx,sy,SRCCOPY);
    
     RestoreDC(hdcBitmap, nCapture);
     DeleteDC(hdcBitmap);
    
     BITMAP bm; GetObject(hbmBitmap, sizeof(BITMAP), &bm);
     nHeaders = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
     nImageSize = (bm.bmWidthBytes * bm.bmHeight);
    
     BITMAPFILEHEADER bmFile = {
        'MB', nHeaders + nImageSize, 0, 0, nHeaders
     };
    
     BITMAPINFOHEADER bmInfo = {
                      sizeof(bmInfo), bm.bmWidth, bm.bmHeight, bm.bmPlanes,
                      bm.bmBitsPixel, BI_RGB, nImageSize, 0, 0, 0, 0
     };
    
	 BYTE* bmpBuff = NULL;
	 BYTE* bmpBuffStart = NULL; //tmp ptr to start of buff
	 int bmpSize = sizeof(bmFile)+sizeof(bmInfo)+nImageSize;
	 bmpBuff = (BYTE*) malloc(bmpSize);
	 bmpBuffStart = bmpBuff;
	 
	 memcpy(bmpBuff,&bmFile, sizeof(bmFile));
	 bmpBuff+=sizeof(bmFile);

	 memcpy(bmpBuff,&bmInfo, sizeof(bmInfo));
	 bmpBuff+=sizeof(bmInfo);

	 memcpy(bmpBuff,bm.bmBits,nImageSize);
	 bmpBuff+=sizeof(nImageSize);
	 

	 if(bmpBuff)
	 {	 
		CxImage image(bmpBuffStart,bmpSize,CXIMAGE_FORMAT_BMP);
	 
		if (image.IsValid())
		{
			if(!image.IsGrayScale()) 
				image.IncreaseBpp(24);
			//image.SetJpegQuality(80);

			/////////clean old
			if(CWMouseXPDlg::curInstance->iImgEncoded)
			{
				free(CWMouseXPDlg::curInstance->iImgEncoded);
				CWMouseXPDlg::curInstance->iImgEncoded = NULL;
				CWMouseXPDlg::curInstance->iImgEncodedLength = 0;
			}
			///////////////////////
			long imglen=0;
			image.Encode(CWMouseXPDlg::curInstance->iImgEncoded,imglen,CXIMAGE_FORMAT_PNG);		

			//todo: handle case len more than what USHORT can hold
			CWMouseXPDlg::curInstance->iImgEncodedLength = imglen;

			image.Save("c:\\image.png",CXIMAGE_FORMAT_PNG);
			return true;
		}

		 free(bmpBuff);
	 }

     if((hFile = CreateFile(
               filename,FILE_WRITE_DATA,FILE_SHARE_WRITE,NULL,
               CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL
     )) != INVALID_HANDLE_VALUE){
           WriteFile(hFile,&bmFile, sizeof(bmFile), &dwOut, NULL);
           WriteFile(hFile,&bmInfo, sizeof(bmInfo), &dwOut, NULL);
           WriteFile(hFile,bm.bmBits, nImageSize, &dwOut, NULL);
          
           CloseHandle(hFile);
     }
    
     DeleteObject(hbmBitmap);

	 
	 
	 
    
	return false;
     
}

bool CWMouseXPDlg::SendImgEncodedToPhone()
	{
		if(iImgEncoded == NULL || iImgEncodedLength == 0)
		{
			mydebug::log("sendf img null");
			return false;
		}

		//different send command for each stack
			if(curInstance->iStackUsed == EWidcommStack)
			{
					if(iWidcommHandler)
					{
						iImgSendWrittenLength = 0;
						if(iWidcommHandler->RFCOMMWrite(iImgEncoded,iImgEncodedLength,iImgSendWrittenLength))
							return true;
						else
							mydebug::log("sendf rfcommwrite failed");
					}
			}
				
		
		mydebug::log("sendf not widcomm");
		return false;
				
	}
*/

BOOL CWMouseXPDlg::OnInitDialog()
{
	ShowWindow(SW_HIDE);

	CString autostarted = __targv[1];
	if(autostarted == "/autostart")
	{
		iAutoStarted = true;
		Sleep(7000);		
	}
	else
		iAutoStarted = false;

	
	this->CenterWindow();
	CDialog::OnInitDialog();
	this->CenterWindow();

	iNotInitDontShowWindow = false;

	
	DefaultCmdKeyMap();
	RefreshMonitorsMetrics();
	LoadCustomCommands();

	iAutoConnectCommand = false;
	//iAutoConnectDelay = 15;

	
	

	

	mydebug::log("//////////////////SESSION BEGIN");

	//////////////////////////////SUPORTED BT STACKS
	iWidcommHandler = CWidcommHandler::GetInstance();	
	
	if(iWidcommHandler)
		mydebug::log("iWidcommHandler");
	
	iBlueSoleilHandler = CBlueSoleilHandler::GetInstance();

	if(iBlueSoleilHandler)
		mydebug::log("iBlueSoleilHandler");

	TToshParams toshparams;
	strcpy(toshparams.iAppName,"WMouseXP");
	toshparams.iHWND = this->GetSafeHwnd();
	toshparams.iStatus = 0;
	iToshHandler = CToshHandler::GetInstance(toshparams);

	if(iToshHandler)
		mydebug::log("iToshHandler");
	
	iMSStackSupported = MSStackSupported();

	if(iMSStackSupported)
		mydebug::log("iMSStackSupported");
	///////////////////////////////////////////////////////
	
	 if(!iWidcommHandler && !iMSStackSupported && !iBlueSoleilHandler && !iToshHandler)
	 {		 	
		mydebug::log("Can't find any compatible Bluetooth drivers installed");

		AfxMessageBox("\tCan't find any compatible Bluetooth drivers installed.\r\n\r\nThis version of WMouseXP is compatible with: BlueSoleil, Toshiba, Microsoft and Broadcom Bluetooth Stacks.");
		CDialog::OnCancel();
		return false;
	 }

	 

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	/*CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}*/

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon



		m_pToolTip = new CToolTipCtrl;
       if(!m_pToolTip->Create(this))
       {
           TRACE("Unable To create ToolTip\n");
           return TRUE;
       }

       //if(m_pToolTip->AddTool(this, "WMouseXP"))
       //{
       //    TRACE("Unable to add Dialog to the tooltip\n");
       //}

       if (m_pToolTip->AddTool(&m_InstallOnPhoneButton,"Send WMouseXP Mobile Side to phone via Bluetooth"))
       {
           TRACE("Unable to add m_InstallOnPhoneButton button to the tooltip\n");
       }


       if (m_pToolTip->AddTool(&iConnectButton,"Make sure Mobile Side shows \"Awaiting PC Side\" first."))
       {
           TRACE("Unable to add iConnectButton button to the tooltip\n");
       }

	   if (m_pToolTip->AddTool(&m_tc,"Start WMouseXP on boot and periodically try to connect to phone automatically..."))
       {
           TRACE("Unable to add iConnectButton button to the tooltip\n");
       }

		m_pToolTip->Activate(TRUE);








	
	/// Determine ownership status at launch.	
	UpdateData(false);

		/*CWMouseXPDlg::curInstance->ShowWindow(SW_SHOW);
	//ShowWindow(appWindow, showWindow);
	CWMouseXPDlg::curInstance->UpdateWindow();*/


	
		
		
	
	if(GetRegistryKey("Software\\WMouseXP1_0", "PrevDev", iPrevDev))
	{
		m_PrevDevCheckBox.EnableWindow(true);
	
		char prevcheck[5];
		
		strcpy(prevcheck,"n");


		if(GetRegistryKey("Software\\WMouseXP1_0", "PrevCheck", prevcheck))
			if(strcmp(prevcheck,"y") == 0 )
				m_PrevDevCheckBox.SetCheck(BST_CHECKED);
	
	}

	bool connectnow = false;
	{ //connect on start checkbox
	char prevcheck[5];
	if(GetRegistryKey("Software\\WMouseXP1_0", "ConnectOnStart", prevcheck))
		if(strcmp(prevcheck,"y") == 0 )
		{
			m_tc.SetCheck(BST_CHECKED);
			connectnow = true;
			StartAutoConnectTimer();//will try every 1 minute
		}
	}

	if(connectnow == false)
	{
		LoadToTray( this, 
		        WM_TRAY_NOTIFY, // user defined 
				"WMouseXP",    
				"Start WMouseXP on phone and press \"Connect\" on PC.",  
				"WMouseXP",
				1, //sec
				AfxGetApp()->LoadIcon( IDR_MAINFRAME) );	
	}
	else
	{
			LoadToTray( this, 
		        WM_TRAY_NOTIFY, // user defined 
				"WMouseXP",    
				"Running in Auto-Connect Mode",  
				"WMouseXP",
				1, //sec
				AfxGetApp()->LoadIcon( IDR_MAINFRAME) );	
	
	}

	//m_loadingGif.Draw();
			//m_loadingGif.Stop();
	
	// TODO: Add extra initialization here
	
	

	
	WTSRegisterSessionNotification(this->GetSafeHwnd(),NOTIFY_FOR_THIS_SESSION);
		

	iNokiaPCSuiteServiceLayerStopped = false;

	iConnectOnStartThreadID = 0;
	if(connectnow)
	{
		this->AddLog("Preparing Auto-Connect...");

		CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DoConnectOnStart,this,0,&iConnectOnStartThreadID);	
		//start thread to call "on button"

	}


	////////////////show window
	if(autostarted == "/autostart")
	{		
		ShowWindow(SW_SHOW);		
		iAutoStarted = true;		
		//ShowWindow(SW_SHOWMINIMIZED);
		PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
	}	
	else
	{
		iAutoStarted = false;	
		ShowWindow(SW_SHOW);
	}
	///////////////////////////


	return TRUE;  // return TRUE  unless you set the focus to a control
}

int CWMouseXPDlg::DoConnectOnStart(void *caller)
{
	CWMouseXPDlg* that = (CWMouseXPDlg*) caller;
	
	Sleep(700);
	/*int maxwaitremain = that->iAutoConnectDelay-1;

	if(maxwaitremain>0 && maxwaitremain<60)
	{
		for(int i=0;i<=maxwaitremain;i++)
		{
			Sleep(1000);
			CString str;
			str.Format("Auto-Connect in... %d Seconds",maxwaitremain-i);
			that->AddLog(str);
		};
	}*/
	
	that->TryConnectSilentlyIfIdle();
	
	return 1;
}

void CWMouseXPDlg::TryConnectSilentlyIfIdle()
{
	CString curStr;
	iConnectButton.GetWindowText(curStr);

	if(curStr == CString("Connect"))
	{
		iAutoConnectCommand = true;	
		StartConnect();
	}
}


bool CWMouseXPDlg::StartAutoConnectTimer()
{
	StopAutoConnectTimer();
	m_AutoConnectTimerID = SetTimer(77/*any identifier because we have only one timer*/, 60 * 1000, AutoConnectTimerProc);

	if(m_AutoConnectTimerID)
		return true;
	
	return false;
}


bool CWMouseXPDlg::StopAutoConnectTimer()
{
	if(m_AutoConnectTimerID && KillTimer(m_AutoConnectTimerID))
	{
		m_AutoConnectTimerID = 0;
		return true;
	}

return false;

}




void CWMouseXPDlg::AutoConnectTimerProc(
   HWND hWnd,      // handle of CWnd that called SetTimer
   UINT nMsg,      // WM_TIMER
   UINT nIDEvent,   // timer identification
   DWORD dwTime    // system time
									)
{

	if(CWMouseXPDlg::curInstance)
		CWMouseXPDlg::curInstance->TryConnectSilentlyIfIdle();

}

LONG CWMouseXPDlg::OnTrayNotify( WPARAM wParam, LPARAM lParam )
{
	

	switch( lParam )
	{
	case WM_LBUTTONDOWN: //on double-click the left mouse button restore the dialog
		
		

		if(IsWindowVisible())
			ShowWindow(SW_SHOWMINIMIZED);
		else
		{
			ShowWindow( SW_SHOW );
			ShowWindow( SW_RESTORE );		
		}

		break;
	case WM_RBUTTONDOWN: //on double-click the left mouse button restore the dialog
	
		

		if(IsWindowVisible())
			ShowWindow(SW_SHOWMINIMIZED);
		else
		{
			ShowWindow( SW_SHOW );
			ShowWindow( SW_RESTORE );		
		}

		break;
	}

	return (0);
}

void CWMouseXPDlg::LoadToTray( CWnd    *pWnd, 
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
				              HICON	  icon )
{
//NOTIFYICONDATA contains information that the system needs to process taskbar status area messages

	ZeroMemory( &_tnd, sizeof( NOTIFYICONDATA ) );

	_tnd.cbSize		      = sizeof( NOTIFYICONDATA );
	_tnd.hWnd			  = pWnd->GetSafeHwnd();
	_tnd.uID			  = 0;
	
	_tnd.uFlags		      = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_INFO;
	//  _tnd.uFlags = NIF_MESSAGE|NIF_ICON;
  //_tnd.uCallbackMessage = MYWM_NOTIFYICON;
 // _tnd.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
			// Flag Description:
			// - NIF_ICON	 The hIcon member is valid.  
	        // - NIF_MESSAGE The uCallbackMessage member is valid. 
	        // - NIF_TIP	 The szTip member is valid. 
	        // - NIF_STATE	 The dwState and dwStateMask members are valid. 
	        // - NIF_INFO	 Use a balloon ToolTip instead of a standard ToolTip. The szInfo, uTimeout, szInfoTitle, and dwInfoFlags members are valid. 
            // - NIF_GUID	 Reserved. 

	_tnd.dwInfoFlags      = NIIF_INFO; // add an icon to a balloon ToolTip
			// Flag Description 
			// - NIIF_ERROR     An error icon. 
			// - NIIF_INFO      An information icon. 
			// - NIIF_NONE      No icon. 
			// - NIIF_WARNING   A warning icon. 
			// - NIIF_ICON_MASK Version 6.0. Reserved. 
			// - NIIF_NOSOUND   Version 6.0. Do not play the associated sound. Applies only to balloon ToolTips 

	_tnd.uCallbackMessage = uCallbackMessage;  
	_tnd.uTimeout         = uTimeout * 1000;
	_tnd.hIcon		   	  = icon;

	_tnd.uVersion = NOTIFYICON_VERSION;

	strcpy( _tnd.szInfoTitle, sInfoTitle );
	strcpy( _tnd.szInfo,      sInfo      );
	strcpy( _tnd.szTip,       sTip       );

	if(!added)
		{
		added = true;
		Shell_NotifyIcon( NIM_ADD, &_tnd ); // add to the taskbar's status area
		}
	else
		Shell_NotifyIcon( NIM_MODIFY, &_tnd ); // add to the taskbar's status area


}

void CWMouseXPDlg::OnSize(UINT nType, int cx, int cy) 
{

	if(iNotInitDontShowWindow)
	{
		ShowWindow(SW_HIDE);	
	}
	else
	{
		if(nType == SIZE_MINIMIZED)
		{
			ShowWindow(SW_HIDE);
			CDialog::OnSize(nType, cx, cy);
		}
		else
		if(nType == SIZE_RESTORED || nType == SIZE_MAXSHOW)
		{	  
			ShowWindow(SW_SHOW);
		}
	}

    CDialog::OnSize(nType, cx, cy);
  	
}

void CWMouseXPDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CWMouseXPDlg::OnDestroy()
{
	//WinHelp(0L, HELP_QUIT);
	CDialog::OnDestroy();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWMouseXPDlg::OnPaint() 
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
HCURSOR CWMouseXPDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}







void CWMouseXPDlg::SimulateKey(int KeyCode, int mod)
{

		INPUT  Input={0};

		
		
		if(mod & KModMaskCtrl)
		{
			ZeroMemory(&Input,sizeof(INPUT));
		  Input.type      = INPUT_KEYBOARD;	  
		  Input.ki.wVk = VK_CONTROL;
		  Input.ki.dwFlags = KEYEVENTF_UNICODE;
		  SendInput(1,&Input,sizeof(INPUT));		  
		  Sleep(20);
		}		
		
		if(mod & KModMaskAlt)
		{
		  ZeroMemory(&Input,sizeof(INPUT));
		  Input.type      = INPUT_KEYBOARD;	  
		  Input.ki.wVk = VK_MENU;
		  Input.ki.dwFlags = KEYEVENTF_UNICODE;
		  SendInput(1,&Input,sizeof(INPUT));
		  Sleep(20);
		}

		if(mod & KModMaskShift)
		{
		  ZeroMemory(&Input,sizeof(INPUT));
		  Input.type      = INPUT_KEYBOARD;	  
		  Input.ki.wVk = VK_SHIFT;
		  Input.ki.dwFlags = KEYEVENTF_UNICODE;
		  SendInput(1,&Input,sizeof(INPUT));
		  Sleep(20);
		}

	  ZeroMemory(&Input,sizeof(INPUT));
	  Input.type      = INPUT_KEYBOARD;	  
	  Input.ki.wVk = KeyCode;	  
	  Input.ki.dwFlags = KEYEVENTF_UNICODE;	  
	  SendInput(1,&Input,sizeof(INPUT));

	  Sleep(20);
	  ZeroMemory(&Input,sizeof(INPUT));
	  Input.type      = INPUT_KEYBOARD;	  
	  Input.ki.wVk = KeyCode;
	  Input.ki.dwFlags = KEYEVENTF_KEYUP;	  
	  SendInput(1,&Input,sizeof(INPUT)); 

	   

		if(mod & KModMaskShift)
		{
			Sleep(20);
		  ZeroMemory(&Input,sizeof(INPUT));
		  Input.type      = INPUT_KEYBOARD;	  
		  Input.ki.wVk = VK_SHIFT;
		  Input.ki.dwFlags = KEYEVENTF_KEYUP;
		  SendInput(1,&Input,sizeof(INPUT));
		  
		}

			
		
		if(mod & KModMaskAlt)
		{
			Sleep(20);
		  ZeroMemory(&Input,sizeof(INPUT));
		  Input.type      = INPUT_KEYBOARD;	  
		  Input.ki.wVk = VK_MENU;
		  Input.ki.dwFlags = KEYEVENTF_KEYUP;
		  SendInput(1,&Input,sizeof(INPUT));
		  
		}
	
		if(mod & KModMaskCtrl)
		{
			Sleep(20);
			ZeroMemory(&Input,sizeof(INPUT));
		  Input.type      = INPUT_KEYBOARD;	  
		  Input.ki.wVk = VK_CONTROL;
		  Input.ki.dwFlags = KEYEVENTF_KEYUP;
		  SendInput(1,&Input,sizeof(INPUT));		  
		  
		}	
}


void CWMouseXPDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default

	

	WTSUnRegisterSessionNotification(this->GetSafeHwnd());

	this->StopMover();
	
	Shell_NotifyIcon( NIM_DELETE, &_tnd ); // delete from the status area

	if(iWidcommHandler)
		iWidcommHandler->CloseAndRevertEngineState();
	
	if(iBlueSoleilHandler)
		iBlueSoleilHandler->CloseAndRevertEngineState();

	if(iToshHandler)
		iToshHandler->CloseAndRevertEngineState();

	if(iNokiaPCSuiteServiceLayerStopped)
	{
		this->StartNokiaPCSuiteServiceLayer();
		iNokiaPCSuiteServiceLayerStopped=false;
	}

	mydebug::log("//////////////////SESSION END");

	exit(0);
}

void CWMouseXPDlg::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

int CWMouseXPDlg::SDPGetPort( const char *addr, LPGUID guid )
{
	int port = 0;
	
	HANDLE h;
	WSAQUERYSET *qs;
	DWORD flags = 0;
	DWORD qs_len;
	bool done;
	qs_len = sizeof(WSAQUERYSET);
	qs = (WSAQUERYSET*) malloc( qs_len );
	ZeroMemory( qs, qs_len );
	qs->dwSize = sizeof(WSAQUERYSET);
	qs->lpServiceClassId = guid;
	qs->dwNameSpace = NS_BTH;
	qs->dwNumberOfCsAddrs = 0;
	qs->lpszContext = (LPSTR) addr;
	flags = LUP_FLUSHCACHE | LUP_RETURN_ADDR;
	if( SOCKET_ERROR == WSALookupServiceBegin( qs, flags, &h )) {
	return 0;
	}
	done = false;
	while ( ! done ) {
	if( SOCKET_ERROR == WSALookupServiceNext( h, flags, &qs_len, qs ) ) {
	int error = WSAGetLastError();
	if( error == WSAEFAULT ) {
	free(qs);
	qs = (WSAQUERYSET*) malloc( qs_len );
	} else if (error == WSA_E_NO_MORE ) {
	done = true;
	} else {
	return 0;
	}
	} else {
	port =
	((SOCKADDR_BTH*)qs->lpcsaBuffer->RemoteAddr.lpSockaddr)->port;
	}
	}
	free(qs);
	WSALookupServiceEnd( h );

	return port;
}
bool CWMouseXPDlg::StartBTInteractionTimer()
{
	iLastBTInteraction = CTime::GetCurrentTime();
	m_TimerID = SetTimer(19/*any identifier because we have only one timer*/, INACTIVITY_TIMEOUT_SECONDS * 1000, TimerProc);

	if(m_TimerID)
		return true;
	
	return false;
}


bool CWMouseXPDlg::StartTrialTimer()
{
	
	m_TrialTimerID = SetTimer(14/*any identifier because we have only one timer*/, 3*60 * 1000, TrialTimerProc);

	if(m_TrialTimerID)
		return true;
	
	return false;
}


bool CWMouseXPDlg::StopTrialTimer()
{
	if(m_TrialTimerID && KillTimer(m_TrialTimerID))
	{
		m_TrialTimerID = 0;
		return true;
	}

	return false;


}


bool CWMouseXPDlg::StopBTInteractionTimer()
{
	if(m_TimerID && KillTimer(m_TimerID))
	{
		m_TimerID = 0;
		return true;
	}

	return false;


}

int CWMouseXPDlg::SendViaSocketL(const char* buff, int len)
{

	

	bool timer = StartBTInteractionTimer();
		
	if(!timer)
		AddLog("Warning: SendViaSocketL Timer start failed.");

	int senderr = send(iSocket,buff,len,0);

	if(timer)
		StopBTInteractionTimer();

//	CString str;
	//str.Format("Send result: %d",senderr);
	//AddLog(str);
						if(0 == senderr)
						{
							throw "Connection closed by device.";
						}
						if(SOCKET_ERROR == senderr)
						{
							//AddLog("SOCKET_ERROR");
							//closesocket(iSocket);
							//return false;
							throw "Disconnected.";
						}

						if(INVALID_SOCKET == senderr)
						{
							/*AddLog("INVALID_SOCKET");
							closesocket(iSocket);
							return false;*/
							
							throw "Invalid socket error.";
						
						}
	
	return senderr; //number of bytes sent

}

int CWMouseXPDlg::RecvViaSocketL(char* buff, int len)
{
	//AddLog("Connection Active...");
	
	///////////////////////////////////////////////////////////////////////////////////////
	//bool timer = StartBTInteractionTimer(); no timer for WMouseXP Recv ****************************************************************
		
	//if(!timer)
	//	AddLog("Warning: SendViaSocketL Timer start failed.");

	//////////////////////////////////////////////////////////////////////////////////////

	int recverr = recv(iSocket,buff,len,0);

	//////////////////////////////////////////////////////////////////////////////////////
	//if(timer)
	//	StopBTInteractionTimer();
	//////////////////////////////////////////////////////////////////////////////////////
						
	//CString str;
	//str.Format("Receive result: %d",recverr);
	//AddLog(str);


						if(0 == recverr)
						{
							throw "Connection closed by device.";
						}

                        if(SOCKET_ERROR == recverr )
                        {
							/*AddLog("SOCKET_ERROR");
							closesocket(iSocket);
							return false;*/
							throw "Disconnected.";
						}
						
						if(INVALID_SOCKET == recverr)
						{
							/*AddLog("INVALID_SOCKET");
							closesocket(iSocket);
							return false;*/
							
							throw "Invalid socket error.";
						
						}
	
	return recverr; //number of bytes received

					
}


    


const char*  stopReasonSuspend = _T("Disconnected: System Suspend/Standby");
const char*  stopLowBat = _T("Disconnected: Low battery");
const char*  stopLocked = _T("Disconnected: System Locked");
const char*  resComplete = _T("System resumed... waiting for session unlock");
const char*  resumingin15sec= _T("System resumed: Re-connecting in 15 Secs.");


void WINAPI ScanThread (LPVOID lpParameter)
{

	//use try catch throw for easy cleanup?
	//cleanup and revert to previous state if an exception is thrown
	
	
	//let all variables be allocated here?
	//use non-blocking socket reading overloads or create its own thread for it
	
	

	CWMouseXPDlg* that = (CWMouseXPDlg*) lpParameter;
	
	that->iThereWasASession = false;

	that->AddLog("Starting Search Session...");

	that->m_loadingGif.Load(MAKEINTRESOURCE(IDR_GIF),_T("GIF"));
	that->m_loadingGif.ShowWindow(true);
	that->m_loadingGif.Draw();
	
	WSAQUERYSET *qs = NULL;
	

	try
	{
	
	
	

	
		
	HANDLE hRadio;

	BLUETOOTH_FIND_RADIO_PARAMS btfrp;
	ZeroMemory(&btfrp,sizeof(btfrp));

	btfrp.dwSize = sizeof(btfrp);
	
	HBLUETOOTH_RADIO_FIND ret;
	
	ret = BluetoothFindFirstRadio(     
		&btfrp,
		&hRadio
    );

	if(ret>0)
	{
		that->AddLog("Found a compatible Bluetooth Radio.");
	}
	
	else
	{
		char* err = "No (USB) Bluetooth Radios found.\r\n\r\n- Make sure you have a Bluetooth Device running the Micorsoft Stack plugged-in to this computer.";
		if(!that->iAutoConnectCommand)
			AfxMessageBox(err);
		throw err;
	}

	if(!that->InitSocket())
	{				
		throw "Socket Initialization Failed";
	}

	BLUETOOTH_RADIO_INFO rif;
	ZeroMemory(&rif,sizeof(BLUETOOTH_RADIO_INFO));
	
	rif.dwSize = sizeof(BLUETOOTH_RADIO_INFO);

	bool useprevdev = that->m_PrevDevCheckBox.IsWindowEnabled() && that->m_PrevDevCheckBox.GetCheck() == BST_CHECKED;

			if(!useprevdev)
			{

			// prepare the inquiry data structure
			DWORD qs_len = sizeof( WSAQUERYSET );
			qs = (WSAQUERYSET*) malloc( qs_len );
			ZeroMemory( qs, qs_len );
			qs->dwSize = sizeof(WSAQUERYSET);
			qs->dwNameSpace = NS_BTH;
			DWORD flags = LUP_CONTAINERS;
			flags |= LUP_FLUSHCACHE | LUP_RETURN_NAME | LUP_RETURN_ADDR;
			HANDLE h;

			that->AddLog("Seaching for nearby devices...");




			


			// start the device inquiry
			if( SOCKET_ERROR == WSALookupServiceBegin( qs, flags, &h )) {
			
				//free mem before we lose scope
				free( qs );
				qs=NULL;

				if(!(that->iAutoConnectCommand))
					AfxMessageBox("No nearby Bluetooth devices found.\r\n\r\n- Make sure your PC's Bluetooth is plugged-in/started and ready.\r\n- Make sure your phone's Bluetooth is ON and discoverable (\"show to all\",\"Find me!\",etc.).\r\n- Try Again.");



				char* err = "No Devices Found";							
				throw err;
				
				

			}

				that->iNFoundDevices = 0;
				// iterate through the inquiry results
				BTH_ADDR result;
				bool done = false;
				while(! done) 
				{

					if( NO_ERROR == WSALookupServiceNext( h, flags, &qs_len, qs )) 
					{
						char buf[40] = {0};
						ZeroMemory(&buf,40);
						result = ((SOCKADDR_BTH*)qs->lpcsaBuffer->RemoteAddr.lpSockaddr)->btAddr;
						DWORD bufsize = sizeof(buf);
						WSAAddressToString( qs->lpcsaBuffer->RemoteAddr.lpSockaddr,
						sizeof(SOCKADDR_BTH), NULL, buf, &bufsize);						
						//printf("found: %s - %s\n", buf, qs->lpszServiceInstanceName);
						that->AddLog(buf);

						for(unsigned int i=0;i<40;i++)
							that->iDevAddr[that->iNFoundDevices][i] = buf[i];					
						
						that->iDevName[that->iNFoundDevices] = qs->lpszServiceInstanceName;

						//memcpy(&(that->iSockAddr),((SOCKADDR_BTH*)qs->lpcsaBuffer->RemoteAddr.lpSockaddr),sizeof(SOCKADDR_BTH));

						that->iNFoundDevices++;

						if(that->iNFoundDevices < MAX_DEVICE_COUNT)
							;
						else
							done = true;
					} 
					else 
					{
						int error = WSAGetLastError();
						if( error == WSAEFAULT ) 
						{
							free( qs );
							qs= NULL;
							qs = (WSAQUERYSET*) malloc( qs_len );
							
						}
						else if( error == WSA_E_NO_MORE ) 
						{
							CString s;
							s.Format("Search Complete, Found: %d Devices",that->iNFoundDevices);
							that->AddLog(s);
							
							done = true;
						} 
						else 
						{
							
							CString s;
							s.Format("Search error code %d\n", error);
							that->AddLog(s);

							throw "Inquiry error";
							
							done = true;
						}
					}
				}

			free( qs );
			qs=NULL;
			
			
			
			}
			else
			{
				that->iNFoundDevices = 1;
				
				for(int x=0;x<40;x++)
					that->iDevAddr[0][x] = that->iPrevDev[x];
			}

			
			//for each device:sdp then obex
			//gen from http://www.famkruithof.net/uuid/uuidgen
			
			//OLD
			//27b8f85a-1d74-11dc-8314-0800200c9a66
			//27b8f85a-1d74-11dc-8314-0800200c9a66
			
			//85C2055C888B39

			//NEW
			//2bc2b92e-3992-11dc-8314-0800200c9a66
			
			
			
			GUID protocol;


			protocol.Data1 = 0x2bc2b92e;
			protocol.Data2 = 0x3992;
			protocol.Data3 = 0x11dc;
			protocol.Data4[0] = 0x83;
			protocol.Data4[1] = 0x14;
			protocol.Data4[2] = 0x08;
			protocol.Data4[3] = 0x00;
			protocol.Data4[4] = 0x20;
			protocol.Data4[5] = 0x0c;
			protocol.Data4[6] = 0x9a;			
			protocol.Data4[7] = 0x66;

/*
			protocol.Data1 = 0xA55665EE;
			protocol.Data2 = 0x9F91;
			protocol.Data3 = 0x4610;
			protocol.Data4[0] = 0x90;
			protocol.Data4[1] = 0x85;
			protocol.Data4[2] = 0xc2;
			protocol.Data4[3] = 0x05;
			protocol.Data4[4] = 0x5c;
			protocol.Data4[5] = 0x88;
			protocol.Data4[6] = 0x8b;			
			protocol.Data4[7] = 0x39;
				
*/			

			CString portStr;

			TidyVector& vector = CWMouseXPDlg::curInstance->iFoundWMouseXPDevices;
			vector.RemoveAll();


			for(int i=0;i<that->iNFoundDevices;i++)
			{		
				//BECAREFUL, DON'T USE the i variable for other purposes - a hard to check bug would occur!!!!!!!!!!!!!!!!!!

				CString status;
				if(!useprevdev)
					status.Format("Looking for WMouseXP on device #%d",(i+1));
				else
					status = "Looking for WMouseXP on Previous Phone...";
				that->AddLog(status);
				
				char addr[40];
				for(int j=0;j<40;j++)
				{
					addr[j] = (that->iDevAddr)[i][j];
				}

				int port = that->SDPGetPort( addr , &protocol );						
				
				
				if(port == 0)
				{
					//that->AddLog("WMouseXP not found on device...");
						continue;
				}
				else
				{	
					//CString str;
					//str.Format("WMouseXP found at port %d",port);
					that->AddLog("WMouseXP found on device...");																
					vector.Add(new WMouseXPMobileDevice(addr,port,that->iDevName[i].GetBuffer(0) ));
					//add to found list					
				}			
			}




			//if found list is empty then show to user msgbox make sure it's opened and discoverable
			//if found list is only one then just connect to it
			//else show a list to choose from then start comm if idok

			if(vector.GetSize()<=0)
			{
				if(!useprevdev)
				{
					if(!(that->iAutoConnectCommand))
						AfxMessageBox("Can't find any device with WMouseXP running:\r\n\r\n- Make sure your PC's Bluetooth is plugged-in/started and ready.\r\n- Make sure your phone's Bluetooth is ON and discoverable (\"show to all\").\r\n- Make sure your phone's WMouseXP (Mobile Side) is started and displaying \"Awaiting PC Side\".\r\n- Try Again.");
					else
						that->AddLog("Can't find any device with WMouseXP running");
				}
				else
				{
					if(!(that->iAutoConnectCommand))
						AfxMessageBox("Can't connect to WMouseXP on the Previous Phone:\r\n\r\n- Make sure your PC's Bluetooth is plugged-in/started and ready.\r\n- Make sure your phone's Bluetooth is ON and discoverable (\"show to all\").\r\n- Make sure your phone's WMouseXP (Mobile Side) is started and displaying \"Awaiting PC Side\".\r\n- If you don't want to use the previous phone, uncheck the \"Previous Phone\" box.\r\n- Try Again.");
					else
						that->AddLog("Can't connect to WMouseXP on the Previous Phone");
				}
			}
			else
			if(vector.GetSize()==1)
			{			
				that->iConnectButton.EnableWindow(true);
				that->iConnectButton.SetWindowText("Disconnect");

				WMouseXPMobileDevice* dev = (WMouseXPMobileDevice*)vector.GetAt(0);
				
				
				///////////////////////////////////////////////////
				that->StartCommunication(dev->iAddr,dev->iPort); //this can only exit with a leave
				///////////////////////////////////////////////////
			}
			else
			{
				//let the dlg populate the list from our found devices list
				//make a public int to recv the result
				int SelectedDev = 0;

				CSelectPhoneDialog dlg(EMSStack,&(CWMouseXPDlg::curInstance->iFoundWMouseXPDevices),SelectedDev,CWMouseXPDlg::curInstance);

				if(IDOK == dlg.DoModal())
				{
					that->iConnectButton.EnableWindow(true);
					that->iConnectButton.SetWindowText("Disconnect");
					
					if(SelectedDev>=0 && SelectedDev<vector.GetSize())
					{
					
					

					WMouseXPMobileDevice* dev = (WMouseXPMobileDevice*)vector.GetAt(SelectedDev);
					///////////////////////////////////////////////////
					that->StartCommunication(dev->iAddr,dev->iPort); //this can only exit with a leave
					///////////////////////////////////////////////////					
					}
					else
					{
						throw "No devices selected";
					}
				}
				else
					throw "Cancelled by user.";
			}

			








				
				

			

			
		

	}
	catch(void* exception)
	{		
		CString wt;
		that->iStatusLabel.GetWindowText(wt);
		
		if(wt!=stopLocked && wt!=stopReasonSuspend && wt!=stopLowBat)
			that->AddLog((char*)exception);
		
	}


	that->StopMover();
	closesocket(that->iSocket);      		
	WSACleanup();

	delete qs;
	qs=NULL;

	that->RevertToIdle();


	return;

}

void  CWMouseXPDlg::HandleBTConnected()
{

		m_PrevDevCheckBox.SetCheck(BST_CHECKED);
	char prevcheck[5];	
	strcpy(prevcheck,"y");	
	SetRegistryKey("Software\\WMouseXP1_0", "PrevCheck", prevcheck);



					m_loadingGif.UnLoad();
					m_loadingGif.Load(MAKEINTRESOURCE(IDR_GIF1),_T("GIF"));
					m_loadingGif.Draw();
					
					
					//SendViaSocketL((const char*) str,5);
					
					iPrevDev[39] = NULL; //make sure it's string safe

					SetRegistryKey("Software\\WMouseXP1_0", "PrevDev", iPrevDev);
					
					if(GetRegistryKey("Software\\WMouseXP1_0", "PrevDev", iPrevDev))
						m_PrevDevCheckBox.EnableWindow(true);

					
					//show balloon and minimize
					LoadToTray( this, 
					WM_TRAY_NOTIFY, // user defined 
					"Wireless Control Active...",    
					"WMouseXP has connected to the Mobile Side",  
					"WMouseXP",
					1, //sec
					AfxGetApp()->LoadIcon( IDR_MAINFRAME) );


					if(IsWindowVisible())
					{
						
						ShowWindow(SW_SHOWMINIMIZED);
					}

}

bool CWMouseXPDlg::StartCommunication(char* devaddr, int port) //for microsoft stack
{
	
	//close socket to make sure it is closed, new session
	closesocket(iSocket);    
	WSACleanup();

	if(!InitSocket())
	{
		throw "Initialize MS Bluetooth stack/driver failed";		
	}
	
	SOCKADDR_BTH sa = { 0 };
	int sa_len = sizeof(sa);
	

	if( SOCKET_ERROR == WSAStringToAddress( devaddr, AF_BTH,
	NULL, (LPSOCKADDR) &sa, &sa_len ) ) 
	{
		throw "Convert BT address WSAStringToAddress failed";	
	}

	sa.port = port;

	if( sa.port == 0 ) {
		AddLog("Invalid device port specified");
		return false;
	}	

	bool success = false;

	try{
              
				StartBTInteractionTimer(); //so we don't wait too long for a user to respond, stop attempt after 15 secs
		
				if(SOCKET_ERROR != connect( iSocket, (LPSOCKADDR) &sa, sa_len ))
                {
					StopBTInteractionTimer(); 
					iThereWasASession = true;

					for(int i=0;i<40;i++)
						this->iPrevDev[i] = devaddr[i];
					
					HandleBTConnected();// this uses iPrevDev to store into registry
					
					char buf[512];
					while(1)
					{
					
					ZeroMemory(&buf,512);
					AddLog("Connection Active...");
					int len = this->RecvViaSocketL(buf,512);
					HandleCommand((BYTE*)&buf,len);
					//AddLog(buf);

					}
					
					
						
				}// end if(connect!=SOCKET_ERROR)                                          
				else
      			{			StopBTInteractionTimer();
							throw "Connection Declined or Timed-Out";
				}

	
		
	}
	catch(char* exception)
	{
		//AddLog(exception);
		StopTrialTimer();
		throw exception;
	}
	
	closesocket(iSocket);      
	WSACleanup();

	throw "Connection Closed.";

		
	
	return success;

}


bool CWMouseXPDlg::InitSocket()
{

	if(!AfxSocketInit())
		return false;

      WORD versionRequested = 0x202;
      WSADATA data;	  

      if(!WSAStartup(versionRequested,&data))
      {
            iSocket = socket(AF_BTH,SOCK_STREAM,BTHPROTO_RFCOMM);
            if(iSocket == INVALID_SOCKET)
                  return false;	
			else
			{
				
				
			}  
				return true;
      }
      else
            return false;
}
void CWMouseXPDlg::AddLog(LPCSTR log)
{

	this->iStatusLabel.EnableWindow(true);
	this->iStatusLabel.SetWindowText(log);
	
}


void CWMouseXPDlg::TrialTimerProc(
   HWND hWnd,      // handle of CWnd that called SetTimer
   UINT nMsg,      // WM_TIMER
   UINT nIDEvent,   // timer identification
   DWORD dwTime    // system time
)
{
	if(!curInstance)
		return;

	

	if(!(curInstance->StopTrialTimer()))
		return; //it was already stopped
	
	if(curInstance->iStackUsed == EMSStack)
	{
		closesocket(curInstance->iSocket);
	}
	else
	if(curInstance->iStackUsed == EWidcommStack)
	{
		curInstance->iWidcommHandler->CloseAndRevertEngineState();
		curInstance->RevertToIdle();
	
	}
	else
	if(curInstance->iStackUsed == EBlueSoleilStack)
	{
		curInstance->iBlueSoleilHandler->CloseAndRevertEngineState();
		curInstance->RevertToIdle();	
	}
	else
	if(curInstance->iStackUsed == EToshStack)
	{
		curInstance->iToshHandler->CloseAndRevertEngineState();
		curInstance->RevertToIdle();	
	}

	


	curInstance->MessageBox("Trial-Use session timed-out...\r\n\r\n Please buy the full version by pressing the \"Buy\" button.","WMouseXP TRIAL",MB_OK|MB_TOPMOST|MB_ICONASTERISK);
	curInstance->AddLog("Trial Time-Out...");

	

}


void CWMouseXPDlg::TimerProc(
   HWND hWnd,      // handle of CWnd that called SetTimer
   UINT nMsg,      // WM_TIMER
   UINT nIDEvent,   // timer identification
   DWORD dwTime    // system time
)
{
	
	//check if socket is connected and the iLastBTInteraction is older than INACTIVITY_TIMEOUT_MILLIS then close the socket
	//start new session if still in announce state
	
	if(!curInstance)
		return;

	curInstance->AddLog("Inactivity time-out...");

	if(!(curInstance->StopBTInteractionTimer()))
		return; //it was already stopped

	curInstance->AddLog("pre-check time");
	
	
	CTime now;
	now = CTime::GetCurrentTime();
	
	CTimeSpan span = now - curInstance->iLastBTInteraction;

	if(span.GetTotalSeconds() >= INACTIVITY_TIMEOUT_SECONDS)
		closesocket(curInstance->iSocket);

	

}




  

void CWMouseXPDlg::MouseMoveAbsolute (int x, int y )
{  
  double fScreenWidth    = ::GetSystemMetrics( SM_CXSCREEN )-1; 
  double fScreenHeight  = ::GetSystemMetrics( SM_CYSCREEN )-1; 
  double fx = x*(65535.0f/fScreenWidth);
  double fy = y*(65535.0f/fScreenHeight);
  


	  INPUT  Input={0};
	  Input.type      = INPUT_MOUSE;
	  Input.mi.dwFlags  = MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE;
	  Input.mi.dx = fx;
	  Input.mi.dy = fy;
	  SendInput(1,&Input,sizeof(INPUT));  


}

void CWMouseXPDlg::MouseMoveRelative(int x, int y )
{
	INPUT  Input={0};
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags  = MOUSEEVENTF_MOVE;
	Input.mi.dx = x;
	Input.mi.dy = y;
	SendInput(1,&Input,sizeof(INPUT));  			  
}

bool CWMouseXPDlg::StartMover(bool xAxis, int diff, int sleepTime)
{
	//if active then return
	if(iMoverThreadID!=0)
	{
		return false;
	}
	
	stopMoving = false;
	moveSleep  = sleepTime;
	moveDiff   =  diff;
	moveX = xAxis;
	//createThread
	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Move,this,0,&iMoverThreadID);	

	return true;
}

void CWMouseXPDlg::StopMover()
{
	stopMoving = true;
}

int CWMouseXPDlg::Move(void *caller)
{
		
		int fScreenWidth    = lMonitorsX; //already 0 based so no need to -1	//::GetSystemMetrics( SM_CXSCREEN )-1; 
		int fScreenHeight  = lMonitorsY; 
		
		CWMouseXPDlg* that = (CWMouseXPDlg*) caller;	

		if(that->moveX)
		{
			int count=1;
			int mod;

			while(1)
			{
				

	   			if(that->stopMoving)
				{
					that->iMoverThreadID = 0;
					return 1; 
				}


				
				POINT p ={0};
				
				if(::GetCursorPos(&p))
				{
						if((p.x >= fScreenWidth && that->moveDiff >= 0) || (p.x <=0 && that->moveDiff <= 0))
						{							
							that->iMoverThreadID = 0;														
							return 1; 
						}				
				}


		
			  INPUT  Input={0};
			  Input.type = INPUT_MOUSE;
			  Input.mi.dwFlags  = MOUSEEVENTF_MOVE;
			  Input.mi.dx = (count>10)?(that->moveDiff):((that->moveDiff)/(abs(that->moveDiff)));
			  Input.mi.dy = 0;
			  SendInput(1,&Input,sizeof(INPUT));  			  

				if(that->stopMoving)
				{
					that->iMoverThreadID = 0;
					return 1;
				}
				
				if(that->iAccelerationOption>0)
				{
					mod = (KMaxAcceleration + 1 - that->iAccelerationOption)*19;

					if(mod<2)
						mod = 2;

					if(that->moveSleep>0 && (count%(mod)==0) )
					{
						that->moveSleep -= that->iAccelerationOption;
					}
				
				}

				if(that->moveSleep>0)
					Sleep(that->moveSleep);
				else
					Sleep(1);

				count++;
				
			}	
		}
		else		
		{	
			int count=1;
			int mod;

			while(1)
			{
	   		
				if(that->stopMoving)
				{
					that->iMoverThreadID = 0;
					return 1;
				}


				
				
				POINT p ={0};
				
				if(::GetCursorPos(&p))
				{					
						if((p.y >= fScreenHeight && that->moveDiff >= 0) || (p.y <=0 && that->moveDiff <= 0))
						{							
							that->iMoverThreadID = 0;							
							return 1; 
						}			
				}

			  INPUT  Input={0};
			  Input.type = INPUT_MOUSE;
			  Input.mi.dwFlags  = MOUSEEVENTF_MOVE;
			  Input.mi.dx = 0;
			  Input.mi.dy = (count>10)?(that->moveDiff):((that->moveDiff)/(abs(that->moveDiff)));
			  SendInput(1,&Input,sizeof(INPUT));  			  
				
				if(that->stopMoving)
				{
					that->iMoverThreadID = 0;
					return 1;
				}
				
				if(that->iAccelerationOption>0)
				{
					mod = (KMaxAcceleration + 1 - that->iAccelerationOption)*19;

					if(mod<2)
						mod = 2;

					if(that->moveSleep>0 && (count%(mod)==0) )
					{
						that->moveSleep -= that->iAccelerationOption;
					}
				
				}

				if(that->moveSleep>0)
					Sleep(that->moveSleep);
				else
					Sleep(1);

				count++;

			}	
		}

		
		
		that->iMoverThreadID = 0;
		return 1;
}
	

	  
	 

void CWMouseXPDlg::OnRFCOMMWriteResult(int len, int err, char* desc)
{

}



void CWMouseXPDlg::HandleCommand(BYTE* buf, int len)
{
	if(len<2)
	{
		//mydebug::log("HandleCommand len<2");
		return;
	}

	
	//CString str;
	//str.Format("HandleCommand BEGIN len = %d",len);
	//mydebug::log(str);

	//2 bytes: opcode|param
	

	int pos=0;
	
	while(pos<len)
	{

		//str.Format("pos = %d",pos);
		//mydebug::log(str);


		if(len-pos>=2 && buf[pos]%KMedia == 0)
		{

			if(buf[pos+1] == 0) //do nothing - this is just sent every second to maintain the connection status on mobile 
			{
				pos+=2;
				continue;
			}

			switch(buf[pos]/KMedia)
			{
				case KKeyPressedCode:
							
					if(TryHandleJoyStickEvent(KKeyPressedCode,buf[pos+1]))
						;
					else //numpad
					{
					
					}
				break;
				case KKeyRepeatedCode:
					
					if(TryHandleJoyStickEvent(KKeyRepeatedCode,buf[pos+1]))
						;
					else //numpad
					{
					
					}
				break;
				case KKeyReleasedCode:
					
					if(TryHandleJoyStickEvent(KKeyReleasedCode,buf[pos+1]))
						;
					else //numpad
					{
					
					}
				break;

				/*
				//tmp disable
				case KRequestSSPNG:
					{
						//next 2 bytes tell width, next 2 tell height, next 1 tell zoomout

						//make ss with that width,height,zoomlevel
						if(pos+5<len)
						{
							int w,h,zoom;
							TakeSSParamStruct  params;
							memcpy((BYTE*)&params,buf+pos+1,sizeof(params));		

							//char buff[64];
							//sprintf(buff,"got mcommand w %d h %d z %d",params.iW,params.iH,params.iZoom);
							//mydebug::log(buff);


							if(TakeScreenshot(params) && iImgEncoded && iImgEncodedLength>0)
							{
								//send to phone CWMouseXPDlg::curInstance->iImgEncoded
								//according to length CWMouseXPDlg::curInstance->iImgEncodedLength

									char buff[64];
									sprintf(buff,"SendImgEncodedToPhone pre iImgEncodedLength %d",iImgEncodedLength);
									mydebug::log(buff);

								if(SendImgEncodedToPhone())
									mydebug::log("send to phone post");	
								else
									mydebug::log("send to phone post failed");	
									
									
									for(int j=0;j<10;j++)
									{
									
										Sleep(100);
										sprintf(buff,"now sent %d",iImgSendWrittenLength);										
										mydebug::log(buff);
									}
							}
							else
							{
								mydebug::log("take ss failed");			
								char buff[64];
								sprintf(buff,"iImgEncoded %d iImgEncodedLength %d",iImgEncoded,iImgEncodedLength);
								mydebug::log(buff);
							}
								

							pos += 6; //1 opcode and 5 param
						}
						else
						{
						pos+=1;
						continue;
						}
						
					}
				break;
				
				case KRequestSSJPG:
					{
					
					
					}
				break;*/

			}

			pos+=2;
		}
		else
			pos++;

	//	mydebug::log("HandleCommand END loop");



	}

	//mydebug::log("HandleCommand END func");


		

}


void CWMouseXPDlg::DefaultCmdKeyMap()
{
	iNumpadCmdMap[ENum1] = VK_ESCAPE;
	iNumpadCmdMap[ENum2] = VK_PRIOR;
	iNumpadCmdMap[ENum3] = VK_F5;	

	iNumpadCmdMap[ENum4] = KVirtualKeyLaunchMyComputer;
	iNumpadCmdMap[ENum5] = KVirtualKeyLaunchMediaPlayer;
	iNumpadCmdMap[ENum6] = KVirtualKeyMute;
	
	iNumpadCmdMap[ENum7] = KVirtualKeyStop;	
	iNumpadCmdMap[ENum8] = KVirtualKeyPlay;
	iNumpadCmdMap[ENum9] = KVirtualKeyIncreaseVolume;	

	iNumpadCmdMap[ENumStar] = KVirtualKeyPrevTrack;
	iNumpadCmdMap[ENum0] = KVirtualKeyNextTrack;
	iNumpadCmdMap[ENumHash] = KVirtualKeyDecreaseVolume;

	for(int i=0;i<12;i++)
	{
		iNumpadScripts[i].Empty();
		iNumpadModifiers[i]=0;
	}

}
		


bool CWMouseXPDlg::TryHandleJoyStickEvent(int presstype,int code)
{
	if(prevCode != code)
		StopMover();
	
	prevCode = code;

	switch(code)
	{
		case KJoyUp:
			if(presstype == KKeyPressedCode)
			{
				
				//AddLog("UpPressed");
				StartMover(false,-2,(KStartSlowMoveSleep-(iSpeedOption*2)));				
				
			}
			else
			if(presstype == KKeyRepeatedCode)
			{
				//AddLog("UpRepeated");

				/*if(moveSleep>0)
				{
					moveSleep-=iAccelerationOption;
				}*/
				/*POINT pos;
				if(::GetCursorPos(&pos))
				{
					pos.y -= 30;
					MouseMoveAbsolute(pos.x,pos.y);
				}*/

				//MouseMoveRelative(0,-30);

			}
			else
			if(presstype == KKeyReleasedCode)
			{
				//AddLog("UpReleased");

				StopMover();
			}
		break;


		case KJoyDown:
				if(presstype == KKeyPressedCode)
			{
				//AddLog("DownPressed");
			/*	POINT pos;
				if(::GetCursorPos(&pos))
				{
					pos.y += 10;
					MouseMoveAbsolute(pos.x,pos.y);
				}*/

				StartMover(false,2,(KStartSlowMoveSleep-(iSpeedOption*2)));

			}
			else
			if(presstype == KKeyRepeatedCode)
			{
				//AddLog("DownRepeated");
				/*POINT pos;
				if(::GetCursorPos(&pos))
				{
					pos.y += 30;
					MouseMoveAbsolute(pos.x,pos.y);
				}*/
				
				/*if(moveSleep>0)
				{
					moveSleep-=iAccelerationOption;
				}*/
			}
			else
			if(presstype == KKeyReleasedCode)
			{
				StopMover();
			}
		break;

		
		
		case KJoyLeft:
			if(presstype == KKeyPressedCode)
			{
				//AddLog("LeftPressed");
				StartMover(true,-2,(KStartSlowMoveSleep-(iSpeedOption*2)));
			}
			else
			if(presstype == KKeyRepeatedCode)
			{
				//AddLog("LeftRepeated");
				/*if(moveSleep>0)
				{
					moveSleep-=iAccelerationOption;
				}*/
			}
			else
			if(presstype == KKeyReleasedCode)
			{
				//AddLog("LeftReleased");
				StopMover();
			}
			break;

		case KJoyRight:
			if(presstype == KKeyPressedCode)
			{
				//AddLog("RightPressed");
				POINT pos;				
				StartMover(true,2,(KStartSlowMoveSleep-(iSpeedOption*2)));
			}
			else
			if(presstype == KKeyRepeatedCode)
			{
				/*if(moveSleep>0)
				{
					moveSleep-=iAccelerationOption;
				}*/
			}
			else
			if(presstype == KKeyReleasedCode)
			{
				StopMover();
			}
		break;
		
		
		case KJoyIn:
			
			if(presstype == KKeyPressedCode)
			{
				iJoyRepeatedForRightClick = false;
			}
			else
			if(presstype == KKeyRepeatedCode && !iJoyRepeatedForRightClick) //fresh first repeat only
			{
								
				iJoyRepeatedForRightClick =true;	
				
				//AddLog("Right Click Pressed");

				INPUT    Input={0};
			  // left down 
			  Input.type      = INPUT_MOUSE;
			  Input.mi.dwFlags  = MOUSEEVENTF_RIGHTDOWN;
			  ::SendInput(1,&Input,sizeof(INPUT));

			  // left up
			  ::ZeroMemory(&Input,sizeof(INPUT));
			  Input.type      = INPUT_MOUSE;
			  Input.mi.dwFlags  = MOUSEEVENTF_RIGHTUP;
			  ::SendInput(1,&Input,sizeof(INPUT));

			}
			else
			if(presstype == KKeyReleasedCode && !iJoyRepeatedForRightClick)
			{
				//AddLog("OK Pressed");

				INPUT    Input={0};
			  // left down 
			  Input.type      = INPUT_MOUSE;
			  Input.mi.dwFlags  = MOUSEEVENTF_LEFTDOWN;
			  ::SendInput(1,&Input,sizeof(INPUT));

			  // left up
			  ::ZeroMemory(&Input,sizeof(INPUT));
			  Input.type      = INPUT_MOUSE;
			  Input.mi.dwFlags  = MOUSEEVENTF_LEFTUP;
			  ::SendInput(1,&Input,sizeof(INPUT));

			}
			
		break;		
		
		case KEsc:
			if(presstype == KKeyPressedCode)
			{
				SimulateKey(iNumpadCmdMap[ENum1],iNumpadModifiers[ENum1]);
			}
			break;
		case KPrevSlide:
			if(presstype == KKeyPressedCode || presstype == KKeyRepeatedCode)
			{
				SimulateKey(iNumpadCmdMap[ENum2],iNumpadModifiers[ENum2]);
			}
			break;
		case KStartPresentation:
			if(presstype == KKeyPressedCode)
			{
				SimulateKey(iNumpadCmdMap[ENum3],iNumpadModifiers[ENum3]);
			}
			break;
		case KPrevTrack:
			if(presstype == KKeyPressedCode|| presstype == KKeyRepeatedCode)
			{
				SimulateKey(iNumpadCmdMap[ENumStar],iNumpadModifiers[ENumStar]);
			}
			break;
		case KIncreaseVolume:
			if(presstype == KKeyPressedCode|| presstype == KKeyRepeatedCode)
			{
				SimulateKey(iNumpadCmdMap[ENum9],iNumpadModifiers[ENum9]);
			}
			break;
		case KNextTrack:
			if(presstype == KKeyPressedCode|| presstype == KKeyRepeatedCode)
			{
				SimulateKey(iNumpadCmdMap[ENum0],iNumpadModifiers[ENum0]);
			}	
			break;
		case KStop:
			if(presstype == KKeyPressedCode)
			{
				SimulateKey(iNumpadCmdMap[ENum7],iNumpadModifiers[ENum7]);
			}
			break; 
		case KDecreaseVolume:
			if(presstype == KKeyPressedCode|| presstype == KKeyRepeatedCode)
			{
				SimulateKey(iNumpadCmdMap[ENumHash],iNumpadModifiers[ENumHash]);
			}
			break;
		case KPlay:
			if(presstype == KKeyPressedCode)
			{
				SimulateKey(iNumpadCmdMap[ENum8],iNumpadModifiers[ENum8]);
			}
			break;

		case KMyComputer:
			if(presstype == KKeyPressedCode)
			{
				SimulateKey(iNumpadCmdMap[ENum4],iNumpadModifiers[ENum4]);
			}
			break;
		case KMediaPlayer:
			if(presstype == KKeyPressedCode)
			{
				SimulateKey(iNumpadCmdMap[ENum5],iNumpadModifiers[ENum5]);
			}
			break;
		case KMute:
			if(presstype == KKeyPressedCode)
			{
				SimulateKey(iNumpadCmdMap[ENum6],iNumpadModifiers[ENum6]);
			}
			break;
		
		default:	 
			return false;
	}
	
	return true;

	
}



void CWMouseXPDlg::OnButton() 
{
	// TODO: Add your control notification handler code here
	
	iAutoConnectCommand = false;	
	StartConnect();	
}


void CWMouseXPDlg::StartConnect()
{

	CString curStr;
	iConnectButton.GetWindowText(curStr);

	if(curStr == CString("Connect"))
	{
		mydebug::log("Connect button");
		iConnectButton.EnableWindow(false);
		mydebug::log("w0");
		iConnectButton.SetWindowText("Please Wait...");
		m_InstallOnPhoneButton.EnableWindow(false);
		mydebug::log("w1");
		StopNokiaPCSuiteServiceLayer();		
		AddLog("Starting connection...");
		mydebug::log("w2");
		StopResumeBTConnectionTimer();
		mydebug::log("w3");
		iThereWasASession = false;

		int stacktouse=EMSStack; 	

		////////////
		bool toshradiopresent =  false;
		long toshradiostat;
		if(iToshHandler)
		{
			mydebug::log("w4");
		BTLOCALDEVINFO2 toshradio={0};
		mydebug::log("w5");
		toshradiopresent = BtGetLocalInfo2(&toshradio,&toshradiostat);
		mydebug::log("w6");
		}
		/////////////
		
		if(iWidcommHandler && WidcommRadioFound(iWidcommHandler))
		{
			stacktouse=EWidcommStack;			
		}
		else if(iBlueSoleilHandler)
		{
			stacktouse=EBlueSoleilStack;		
		}
		else if(iToshHandler && toshradiopresent)
		{
			stacktouse=EToshStack;		
			mydebug::log("w7");
		}
		else if(iMSStackSupported && MSStackRadioFound())
		{
			stacktouse=EMSStack; 
		}	
		else
		{		
			if(!iAutoConnectCommand)
				AfxMessageBox("No compatible Bluetooth (USB Dongle) found on this PC - Please insert/start it.\r\n\r\nWMouseXP is compatible with Bluetooth drivers (stacks) of BlueSoleil, Toshiba, Microsoft and WIDCOMM/Broadcom.");
			else
				AddLog("No compatible Bluetooth (USB Dongle) found on this PC");

			RevertToIdle();
			return;
		}
		//if MS dll exists, &&  inserted then use it
		//else
		//if WIDCOMM dll exists && inserted then use it
		//else
		//BSL
		//else
		//show error that 	
		//No compatible Bluetooth (USB Dongle) found on this PC. 
		//WMouseXP is compatible with Bluetooth drivers from either Microsoft or WIDCOMM or BlueSoleil
		//WMouseXP is NOT for Toshiba drivers.

		if(stacktouse == EMSStack)
		{
			mydebug::log("Connect using MSStack");

			iStackUsed = EMSStack;

		
			CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ScanThread,this,0,&iThreadID);	
		}
		else
		if(	stacktouse == EWidcommStack)
		{
			mydebug::log("Connect using WidcommStack");

			iStackUsed = EWidcommStack;
				
		
			if(!(iWidcommHandler->StartRemoteControlSession(appWindow,&m_InstallOnPhoneButton,&m_loadingGif,IDR_GIF,IDR_GIF1,_T("GIF"),&iConnectButton,&iStatusLabel,this)))
			{
				RevertToIdle();
				AddLog("Start WIDCOMM engine part failed, try re-starting WMouseXP.");				
				//revert
			}		
		}
		else
		if(	stacktouse == EBlueSoleilStack)
		{	

			mydebug::log("Connect using BlueSoleilStack");

			iStackUsed = EBlueSoleilStack;
	
			if(!(iBlueSoleilHandler->StartRemoteControlSession(appWindow,&m_InstallOnPhoneButton,&m_loadingGif,IDR_GIF,IDR_GIF1,_T("GIF"),&iConnectButton,&iStatusLabel,this)))
			{
				RevertToIdle();
				AddLog("Start BlueSoleil engine part failed, try re-starting WMouseXP.");				
				//revert
			}				
			
		}
		else if( stacktouse == EToshStack)
		{	

			mydebug::log("Connect using ToshibaStack");

			iStackUsed = EToshStack;
			mydebug::log("w8");
	
			mydebug::log("w9");

			if(!(iToshHandler->StartRemoteControlSession(appWindow,&m_InstallOnPhoneButton,&m_loadingGif,IDR_GIF,IDR_GIF1,_T("GIF"),&iConnectButton,&iStatusLabel,this)))
			{
				mydebug::log("w10");
				RevertToIdle();
				mydebug::log("w11");
				AddLog("Start Toshiba Bluetooth engine failed, try re-starting WMouseXP.");				
				mydebug::log("w12");
				//revert
			}				
			
		}
		


	}
	else	//disconnect
	{
		mydebug::log("Disconnect button");

		if(iStackUsed == EMSStack)
		{
			closesocket(iSocket);      
			WSACleanup();
			StopTrialTimer();
		}
		else
		if(iStackUsed == EWidcommStack)
		{
			iWidcommHandler->CloseAndRevertEngineState();
			AddLog("Disconnected");
			RevertToIdle();
			StopTrialTimer();

			if(MSStackSupported()) //vista widcomm is based on MS
			{
			closesocket(iSocket);      
			WSACleanup();			
			}

		}
		else
		if(	iStackUsed == EBlueSoleilStack)
		{	
			iBlueSoleilHandler->CloseAndRevertEngineState();
			AddLog("Disconnected");
			RevertToIdle();
			StopTrialTimer();			
		}
		else
		if(	iStackUsed == EToshStack)
		{	
			iToshHandler->CloseAndRevertEngineState();
			AddLog("Disconnected");
			RevertToIdle();
			StopTrialTimer();			
		}

	}

}

void CWMouseXPDlg::RevertToIdle()
{
	StopMover();

	if(iStackUsed == EMSStack)
		{
	closesocket(iSocket);      		
	WSACleanup();	
		}
	
	iConnectButton.EnableWindow(true);
	iConnectButton.SetWindowText("Connect");
	m_InstallOnPhoneButton.EnableWindow(true);

	m_loadingGif.Stop();
	m_loadingGif.ShowWindow(false);
	m_loadingGif.UnLoad();

	if(iThereWasASession)
	{
		LoadToTray( this, 
					WM_TRAY_NOTIFY, // user defined 
					"Wireless Control Ended...",    
					"WMouseXP has been disconnected from the Mobile Side",  
					"WMouseXP",
					1, //sec
					AfxGetApp()->LoadIcon( IDR_MAINFRAME) );

		iThereWasASession = false;
		
	}

	/*if(iNokiaPCSuiteServiceLayerStopped)
	{
		this->StartNokiaPCSuiteServiceLayer();
		iNokiaPCSuiteServiceLayerStopped=false;
	}*/

}


void CWMouseXPDlg::OnData(void *p_data, UINT len) //from WIDCOMM
{

	HandleCommand((BYTE*)p_data,len);
	//AddLog("Connection Active...");

}

/*
void CWMouseXPDlg::OnConnectEvent(UINT32 event_code,unsigned char* devaddr) //from WIDCOMM
{

	if (PORT_EV_CONNECTED & event_code)
    {
			iThereWasASession = true;
			
			DWORD bufsize = 40;
			
			
			//BD_ADDR_LEN
			//convert to MS format and copy to iPrevDev, for example: (00:11:9F:D9:F1:79)
			

			sprintf(iPrevDev,"(%02X:%02X:%02X:%02X:%02X:%02X)",devaddr[0],devaddr[1],devaddr[2],devaddr[3],devaddr[4],devaddr[5]);




			 

			
			iConnectButton.EnableWindow(true);
			iConnectButton.SetWindowText("Disconnect");

			HandleBTConnected();//this would start the trial timer if necessary
			AddLog("Connection Active...");	
    }
	else    
    if ( PORT_EV_CONNECT_ERR & event_code )
    {
        	iStatusLabel.SetWindowText("Connection closed.");
			StopTrialTimer();
			iWidcommHandler->CloseAndRevertEngineState();			
			RevertToIdle();
    }
	else
	{
	    	iStatusLabel.SetWindowText("Unknown WIDCOMM event.");	
			//Close();
			//RevertToIdle();
	}
}*/


void CWMouseXPDlg::OnConnected(BYTE* devaddr, int len)
{

	mydebug::log("OnConnected begin");

	

	
			
	DWORD bufsize = 40;
	
	
	//BD_ADDR_LEN
	//convert to MS format and copy to iPrevDev, for example: (00:11:9F:D9:F1:79)
	
	if(len == 6)
	{
		sprintf(iPrevDev,"(%02X:%02X:%02X:%02X:%02X:%02X)",devaddr[0],devaddr[1],devaddr[2],devaddr[3],devaddr[4],devaddr[5]);
	}





	 

	
	iConnectButton.EnableWindow(true);
	iConnectButton.SetWindowText("Disconnect");

	if(iThereWasASession)		
	{
		mydebug::log("OnConnected but iThereWasASession is true");		
	}
	else
	{
		HandleBTConnected();//this would start the trial timer if necessary
	}

	iThereWasASession = true;
	

	AddLog("Connection Active...");	

	mydebug::log("OnConnected end");

}

void CWMouseXPDlg::OnDisconnected()
{
	mydebug::log("OnDisconnected begin");

		iStatusLabel.SetWindowText("Connection closed.");
		StopTrialTimer();
		

		if(iStackUsed == EWidcommStack)
		{
			mydebug::log("EWidcommStack CloseAndRevertEngineState Begin");
			iWidcommHandler->CloseAndRevertEngineState();
			mydebug::log("EWidcommStack CloseAndRevertEngineState End");
			AddLog("Disconnected");
			
		}
		else
		if(	iStackUsed == EBlueSoleilStack)
		{	
			mydebug::log("EBlueSoleilStack CloseAndRevertEngineState Begin");
			iBlueSoleilHandler->CloseAndRevertEngineState();
			mydebug::log("EBlueSoleilStack CloseAndRevertEngineState End");
			AddLog("Disconnected");
		}
		else
		if(	iStackUsed == EToshStack)
		{	
			mydebug::log("EToshStack CloseAndRevertEngineState Begin");
			iToshHandler->CloseAndRevertEngineState();
			mydebug::log("EToshStack CloseAndRevertEngineState End");
			AddLog("Disconnected");
		}


		RevertToIdle();

		mydebug::log("OnDisconnected end");
		
}

void CAboutDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here

	ShellExecute( GetSafeHwnd(), "open", "http://www.gnu.org/philosophy/free-sw.html", NULL, NULL, SW_SHOWNORMAL);
	
}

void CAboutDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
		
	
	
	CDialog::OnLButtonDblClk(nFlags, point);
}

void CAboutDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CAboutDlg::OnButton1() 
{
	
	
	ShellExecute( GetSafeHwnd(), "open", "http://www.clearevo.com/", NULL, NULL, SW_SHOWNORMAL);



}


CString GetVersionInfo(HMODULE hLib, CString csEntry)
{
  CString csRet;

  if (hLib == NULL)
    hLib = AfxGetResourceHandle();
  
  HRSRC hVersion = FindResource( hLib, 
    MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION );
  if (hVersion != NULL)
  {
    HGLOBAL hGlobal = LoadResource( hLib, hVersion ); 
    if ( hGlobal != NULL)  
    {  
  
      LPVOID versionInfo  = LockResource(hGlobal);  
      if (versionInfo != NULL)
      {
        DWORD vLen,langD;
        BOOL retVal;    
    
        LPVOID retbuf=NULL;
    
        static char fileEntry[256];

        sprintf(fileEntry,"\\VarFileInfo\\Translation");
        retVal = VerQueryValue(versionInfo,fileEntry,&retbuf,(UINT *)&vLen);
        if (retVal && vLen==4) 
        {
          memcpy(&langD,retbuf,4);            
          sprintf(fileEntry, "\\StringFileInfo\\%02X%02X%02X%02X\\%s",
                  (langD & 0xff00)>>8,langD & 0xff,(langD & 0xff000000)>>24, 
                  (langD & 0xff0000)>>16, csEntry);            
        }
        else 
          sprintf(fileEntry, "\\StringFileInfo\\%04X04B0\\%s", 
            GetUserDefaultLangID(), csEntry);

        if (VerQueryValue(versionInfo,fileEntry,&retbuf,(UINT *)&vLen)) 
          csRet = (char*)retbuf;
      }
    }

    UnlockResource( hGlobal );  
    FreeResource( hGlobal );  
  }

  return csRet;
}

// Re-formats a string formatted as "m,n,o,p" to format as "m.nop"

CString FormatVersion(CString cs)
{
  CString csRet;

  if (!cs.IsEmpty())
  {
    cs.TrimRight();
    int iPos = cs.Find(',');
    if (iPos == -1)
      return "";
    cs.TrimLeft();
    cs.TrimRight();
    csRet.Format("%s.", cs.Left(iPos));

    while (1)
    {
      cs = cs.Mid(iPos + 1);
      cs.TrimLeft();
      iPos = cs.Find(',');
      if (iPos == -1)
      {
        csRet +=cs;
        break;
      }
      csRet += cs.Left(iPos);
    }
  }

  return csRet;
}



// Loads "ProductVersion" from resource formats it and keeps it in mind

CString GetProductVersionX()
{
  CString ret;
    ret = FormatVersion(GetVersionInfo(NULL, "ProductVersion"));

  return ret;
}


BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	mydebug::InitPath();
	mydebug::log("oninit");
	
	CString upverstr = GetProductVersionX();
	CString cpyverstr = upverstr;

	mydebug::log(upverstr);

	if(upverstr.GetLength()==5)
	{
		upverstr.Insert(3," Release ");
	}

	if(cpyverstr.GetLength()==5)
	{
		cpyverstr.Delete(3,2);
		cpyverstr.Insert(0, "WMouseXP ");
	}

	m_AboutVersion = "WMouseXP ";
	m_AboutVersion += upverstr;
	
	m_Cpyr = cpyverstr;
	m_Cpyr += " Copyright (C) 2007-2010 Kasidit Yusuf. All rights reserved.";
	this->UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}





	  



void CWMouseXPDlg::OnMenuAbout() 
{
	
	

	CAboutDlg dlgAbout;
	dlgAbout.DoModal();	

	
	
	
	
}





void CWMouseXPDlg::OnMenuHelp() 
{
	// TODO: Add your command handler code here	
	ShellExecute( GetSafeHwnd(), "open", "http://www.clearevo.com/wmousexp/", NULL, NULL, SW_SHOWNORMAL);			
}








bool CWMouseXPDlg::SendFile(char* devaddr, int port, BYTE* dataptr, DWORD datalen)
{
	//close socket to make sure it is closed, new session
	closesocket(iSocket);    
	WSACleanup();

	if(!InitSocket())
	{
		throw "Initialize MS Bluetooth stack/driver failed";		
	}
	
	SOCKADDR_BTH sa = { 0 };
	int sa_len = sizeof(sa);
	

	if( SOCKET_ERROR == WSAStringToAddress( devaddr, AF_BTH,
	NULL, (LPSOCKADDR) &sa, &sa_len ) ) 
	{
		throw "Convert BT address WSAStringToAddress failed";	
	}

	sa.port = port;

	if( sa.port == 0 ) {
		iStep1Label.SetWindowText("Invalid device OBEX port specified");
	return false;
	}	


	

	//open socket

	

	//Made new send and recv functions implements a timer, see SendViaSocketL and RevcViaSocketL
	
	//try catch handles when send and recv fails
	
	



	
	//make safe cleanup when exceptions occur
	
	
	
	OBEXPutCommand* opc = NULL;
	
	OBEXPacket* obexPktToSend = NULL; //file part packet
	
	
	bool success = false;

	try{
              
				iStep1Label.SetWindowText("Waiting for phone to accept file...");

				StartBTInteractionTimer(); //so we don't wait too long for a user to respond
				
				if(SOCKET_ERROR != connect( iSocket, (LPSOCKADDR) &sa, sa_len ))
                  {
						//Send OBEX Connect				
						
						StopBTInteractionTimer();  	

						m_InstallPic.Draw();

					    OBEXConnectPacket obexConnectPkt(Opcode_Connect,0x4000);				
				
                        BYTE recvBuff[512];												

						SendViaSocketL((const char*) (obexConnectPkt.wholePacket),obexConnectPkt.packetLength);
						
						
						bool startedPut	= false;
						USHORT theirMaxOBEXPacketSize = 0;					

						
						//See response, then Send all OBEX file packets											
						
						int nreceived = RecvViaSocketL((char*)recvBuff,512);						
						
						

                        if(recvBuff[0] == RspCode_Success)
                        {
                              	
							iStep1Label.SetWindowText("Connect OBEX Success");		
							
															
						while(1)
						{

							if(startedPut)
								nreceived = RecvViaSocketL((char*)recvBuff,512);
							
							
							OBEXPacket obexRcvdPkt(recvBuff,nreceived);
							
							
							if(!startedPut)
							{
									
									if(obexRcvdPkt.wholePacket->code == RspCode_Success)
									{
										iStep1Label.SetWindowText("OBEX connection successful");
										
										BEUS tmo(((BYTE*)(obexRcvdPkt.wholePacket->data))+2);					
										theirMaxOBEXPacketSize = tmo.getValUSHORT(); 
										
										char buf[50] = {0};
										sprintf(buf,"Their max OBEX packet size: %d",theirMaxOBEXPacketSize);
										iStep1Label.SetWindowText(buf);
										
										  
										
										//file->Open("card.vcf",CFile::modeRead|CFile::typeBinary);
										//file->Open("img.jpg",CFile::modeRead|CFile::typeBinary);
										//if(opc)
										//	delete opc;

										 opc = new OBEXPutCommand("WMouseXP.jar",theirMaxOBEXPacketSize,dataptr,datalen);							
										
										//sprintf(buf,"N OBEX puts to send: %d",opc->nPacketsToSend);
										iStep1Label.SetWindowText("Sending File...");
										//devicesLB->DrawAnimatedRects()

										
													obexPktToSend = opc->CgetNextPacket();
													
													SendViaSocketL((const char*) (obexPktToSend->wholePacket),obexPktToSend->packetLength);
													
													delete obexPktToSend;
													obexPktToSend = NULL;

													startedPut = true;

									}

								
							}						
							else
							{
									if(obexRcvdPkt.wholePacket->code == RspCode_Success)
										{
											//send obex disc
											iStep1Label.SetWindowText("File sent via Bluetooth");
											success = true;

											OBEXPacket dsc(Opcode_Disconnect,NULL,0);
												
											//AddLog("Sending OBEX Disconnect Request");
										    SendViaSocketL((const char*) (dsc.wholePacket),dsc.packetLength);					
											//AddLog("OBEX Disconnect Sent");					
											iStep1Label.SetWindowText("File sent via Bluetooth");
											break;

										}
										else
									if(obexRcvdPkt.wholePacket->code == RspCode_Continue)
										{
											obexPktToSend = opc->CgetNextPacket();	
											
											SendViaSocketL((const char*) (obexPktToSend->wholePacket), obexPktToSend->packetLength);

											delete obexPktToSend;
											obexPktToSend = NULL;//avoid double deletion
										
										}
									else
										{
											throw "Unknown OBEX Response";
											//break; throw already broke...
										}
								
								
							}//end else of if(!started)
								
						
							}//end while	
							
							


						}//end if(recvBuff[0] == RspCode_Success)			
						else
						{
							throw("OBEX handshake failed.");							
							
						}	
                        

				}// end if(connect!=SOCKET_ERROR)                                          
				else
      			{			StopBTInteractionTimer();
							throw "Connection Declined or Timed-Out";
				}


	
	
		
	}
	catch(char* exception)
	{
		iStep1Label.SetWindowText(exception);
	}
	
	
	delete obexPktToSend;
	delete opc;


	closesocket(iSocket);      
	WSACleanup();

	return success;

}




void WINAPI SendJarThread (LPVOID lpParameter)
{



	//use try catch throw for easy cleanup?
	//cleanup and revert to previous state if an exception is thrown
	
	
	//let all variables be allocated here?
	//use non-blocking socket reading overloads or create its own thread for it
	
	bool thereWasaSession = false;

	
	
					


	CWMouseXPDlg* that = (CWMouseXPDlg*) lpParameter;
	
	that->iConnectButton.EnableWindow(false);
	that->iStep1Label.SetWindowText("Starting Search Session...");

	that->m_InstallPic.Load(MAKEINTRESOURCE(IDR_GIF),_T("GIF"));
	that->m_InstallPic.ShowWindow(true);
	that->m_InstallPic.Draw();
	
	WSAQUERYSET *qs = NULL;
	

	try
	{
	
	
	

	
		
	HANDLE hRadio;

	BLUETOOTH_FIND_RADIO_PARAMS btfrp;
	ZeroMemory(&btfrp,sizeof(btfrp));

	btfrp.dwSize = sizeof(btfrp);
	
	HBLUETOOTH_RADIO_FIND ret;
	
	ret = BluetoothFindFirstRadio(     
		&btfrp,
		&hRadio
    );

	if(ret>0)
	{
		that->iStep1Label.SetWindowText("Found a compatible Bluetooth Radio.");
		
		//MS BT STACK
	}
	
	else
	{
		//TODO: TRY FIND WIDCOMM STACK BT USB
		//that->StartInquiry();

		char* err = "No (USB) Bluetooth found on this PC.\r\n\r\n- Make sure you have a Bluetooth Device running the Micorsoft Stack plugged-in to this computer.";
		if(!(that->iAutoConnectCommand))
			AfxMessageBox(err);
		else
			that->AddLog("No (USB) Bluetooth Radios found on this PC");
		throw err;
	}

	if(!that->InitSocket())
	{				
		throw "Unsupported Bluetooth Driver: WMouseXP works on: Toshiba, BlueSoleil, Microsoft and Broadcom Bluetooth Stacks...";
	}


	BLUETOOTH_RADIO_INFO rif;
	ZeroMemory(&rif,sizeof(BLUETOOTH_RADIO_INFO));
	
	rif.dwSize = sizeof(BLUETOOTH_RADIO_INFO);


			// prepare the inquiry data structure
			DWORD qs_len = sizeof( WSAQUERYSET );
			qs = (WSAQUERYSET*) malloc( qs_len );
			ZeroMemory( qs, qs_len );
			qs->dwSize = sizeof(WSAQUERYSET);
			qs->dwNameSpace = NS_BTH;
			DWORD flags = LUP_CONTAINERS;
			flags |= LUP_FLUSHCACHE | LUP_RETURN_NAME | LUP_RETURN_ADDR;
			HANDLE h;

			that->iStep1Label.SetWindowText("Seaching for nearby devices...");


			// start the device inquiry
			if( SOCKET_ERROR == WSALookupServiceBegin( qs, flags, &h )) {
			
				//free mem before we lose scope
				free( qs );
				qs=NULL;

				if(!(that->iAutoConnectCommand))
					AfxMessageBox("No nearby Bluetooth devices found.\r\n\r\n- Make sure your PC's Bluetooth is plugged-in/started and ready.\r\n- Make sure your phone's Bluetooth is ON and discoverable (\"show to all\",\"Find me!\",etc.).\r\n- Try Again.");
				else
					that->AddLog("No nearby Bluetooth devices found.");

				char* err = "No Devices Found";							
				throw err;
				
				

			}

				that->iNFoundDevices = 0;
				// iterate through the inquiry results
				BTH_ADDR result;
				bool done = false;
				while(! done) 
				{

					if( NO_ERROR == WSALookupServiceNext( h, flags, &qs_len, qs )) 
					{
						char buf[40] = {0};
						ZeroMemory(&buf,40);
						result = ((SOCKADDR_BTH*)qs->lpcsaBuffer->RemoteAddr.lpSockaddr)->btAddr;
						DWORD bufsize = sizeof(buf);
						WSAAddressToString( qs->lpcsaBuffer->RemoteAddr.lpSockaddr,
						sizeof(SOCKADDR_BTH), NULL, buf, &bufsize);						
						//printf("found: %s - %s\n", buf, qs->lpszServiceInstanceName);
						that->iStep1Label.SetWindowText(buf);

						for(unsigned int i=0;i<40;i++)
							that->iDevAddr[that->iNFoundDevices][i] = buf[i];					
						
						that->iDevName[that->iNFoundDevices] = qs->lpszServiceInstanceName;

						//memcpy(&(that->iSockAddr),((SOCKADDR_BTH*)qs->lpcsaBuffer->RemoteAddr.lpSockaddr),sizeof(SOCKADDR_BTH));

						that->iNFoundDevices++;

						if(that->iNFoundDevices < MAX_DEVICE_COUNT)
							;
						else
							done = true;

					} 
					else 
					{
						int error = WSAGetLastError();
						if( error == WSAEFAULT ) 
						{
							free( qs );
							qs= NULL;
							qs = (WSAQUERYSET*) malloc( qs_len );
							
						}
						else if( error == WSA_E_NO_MORE ) 
						{
							CString s;
							s.Format("Search Complete, Found: %d Devices",that->iNFoundDevices);
							that->iStep1Label.SetWindowText(s);
							
							done = true;
						} 
						else 
						{
							
							CString s;
							s.Format("Search error code %d\n", error);
							that->iStep1Label.SetWindowText(s);

							throw "Inquiry error";
							
							done = true;
						}
					}
				}

			free( qs );
			qs=NULL;


			GUID protocol;

				//OBEX OBJECT PUSH GUID
			protocol.Data1 =	0x00001105;
			protocol.Data2 =	0x0000;
			protocol.Data3 =	0x1000;
			protocol.Data4[0] = 0x80;
			protocol.Data4[1] = 0x00;
			protocol.Data4[2] = 0x00;
			protocol.Data4[3] = 0x80;
			protocol.Data4[4] = 0x5F;
			protocol.Data4[5] = 0x9B;
			protocol.Data4[6] = 0x34;
			protocol.Data4[7] = 0xFB;


			


			CString portStr;

			TidyVector& vector = CWMouseXPDlg::curInstance->iFoundWMouseXPDevices; //well, it's not WMouseXPDevices but devices we want to send WMouseXP to
			vector.RemoveAll();


			for(int i=0;i<that->iNFoundDevices;i++)
			{		
				//BECAREFUL, DON'T USE the i variable for other purposes - a hard to check bug would occur!!!!!!!!!!!!!!!!!!

			/*	CString status;
				status.Format("Looking for OBEX Push on device #%d",(i+1));
				that->iStep1Label.SetWindowText(status);
				
				char addr[40];
				for(int j=0;j<40;j++)
				{
					addr[j] = (that->iDevAddr)[i][j];
				}

				int port = that->SDPGetPort( addr , &protocol );						
				
				
				if(port == 0)
				{
					that->iStep1Label.SetWindowText("OBEX Push not found on device...");
						continue;
				}
				else*/
				{	
					//CString str;
					//str.Format("WMouseXP found at port %d",port);
					//that->iStep1Label.SetWindowText("OBEX Push found on device...");																
					//vector.Add(new WMouseXPMobileDevice(addr,port,that->iDevName[i].GetBuffer(0) ));

					char addr[40];
				for(int j=0;j<40;j++)
				{
					addr[j] = (that->iDevAddr)[i][j];
				}


					vector.Add(new WMouseXPMobileDevice(addr,0,that->iDevName[i].GetBuffer(0) ));
					//add to found list					
				}			
			}




			//if found list is empty then show to user msgbox make sure it's opened and discoverable
			//if found list is only one then just connect to it
			//else show a list to choose from then start comm if idok

			if(vector.GetSize()<=0)
			{
				//AfxMessageBox("No Devices with OBEX Push found.\r\n\r\n- Make sure your PC's Bluetooth is plugged-in/started and ready.\r\n- Make sure your phone's Bluetooth is ON and discoverable (\"show to all\").\r\n- Try Again.\r\n\r\n- See \"Help\" for other installation methods:\r\n- Open \"wap.clearevo.com\" in your mobile's WAP browser and download WMouseXP directly.\r\n- Go to the installation path of this program and send the \"WMouseXP.jar\" via Bluetooth or phone's PC-Suite manually.");			
				if(!(that->iAutoConnectCommand))
					AfxMessageBox("No nearby Bluetooth devices found.\r\n\r\n- Make sure your PC's Bluetooth is plugged-in/started and ready.\r\n- Make sure your phone's Bluetooth is ON and discoverable (\"show to all\").\r\n- Try Again.\r\n\r\nPlease see \"Help\" for other installation methods:\r\n- Open \"wap.clearevo.com\" in your mobile's WAP browser and download WMouseXP directly.\r\n- Go to the installation path of this program and send the \"WMouseXP.jar\" via Bluetooth or phone's PC-Suite manually.");			
				else
					that->AddLog("No nearby Bluetooth devices found.");
			}
			else
			{
				//let the dlg populate the list from our found devices list
				//make a public int to recv the result
				int SelectedDev = 0;
				
				that->m_InstallPic.Stop();

				CSelectPhoneDialog dlg(EMSStack,&(CWMouseXPDlg::curInstance->iFoundWMouseXPDevices),SelectedDev,CWMouseXPDlg::curInstance);

				if(IDOK == dlg.DoModal())
				{
					//that->m_InstallOnPhoneButton.EnableWindow(true);
					//that->m_InstallOnPhoneButton.SetWindowText("Disconnect");

					if(SelectedDev>=0 && SelectedDev<vector.GetSize())
					{

					
					thereWasaSession = true;

					WMouseXPMobileDevice* dev = (WMouseXPMobileDevice*)vector.GetAt(SelectedDev);






					//////////////////////////////////Get OOP port	
					CString status;
					status = "Looking for OBEX Push on device";
					that->iStep1Label.SetWindowText(status);
					
					char addr[40];
					for(int j=0;j<40;j++)
					{
						addr[j] = (dev->iAddr)[j];
					}

					int port = that->SDPGetPort( addr , &protocol );	
					//////////////////////////////////////////

					if(port<=0)
					{

						AfxMessageBox("OBEX Obeject Push not found on device.\r\n\r\nPlease see \"Help\" for other installation methods:\r\n- Open \"wap.clearevo.com\" in your mobile's WAP browser and download WMouseXP directly.\r\n- Go to the installation path of this program and send the \"WMouseXP.jar\" via Bluetooth or phone's PC-Suite manually.");
						throw "OBEX Object Push not found on device.";
					}

					dev->iPort = port;


					///////////////////////////////////////////////////
					//make jar and send to device


					HGLOBAL hResourceLoaded;		// handle to loaded resource 
					HRSRC hRes;						// handle/ptr. to res. info. 
					BYTE* lpResLock;				// pointer to resource data 
					DWORD dwSizeRes;
					

					hRes = FindResource( NULL, MAKEINTRESOURCE(IDR_JAR3), "JAR" );
					
					if(hRes==NULL)
						AfxMessageBox("rsc not found");
					// loads the specified resource into global memory. 
					hResourceLoaded = LoadResource( NULL, hRes ); 

					// get a pointer to the loaded resource!
					lpResLock = (BYTE*)LockResource( hResourceLoaded ); 

					// determine the size of the resource, so we know how much to write out to file!  
					dwSizeRes = SizeofResource( NULL, hRes );

					
					that->m_InstallPic.UnLoad();
					that->m_InstallPic.Load(MAKEINTRESOURCE(IDR_GIF1),_T("GIF"));
					
					
					that->iStep1Label.SetWindowText("Please accept file on phone...");																

					that->SendFile(dev->iAddr,dev->iPort,lpResLock,dwSizeRes);
					
					

					//that->StartCommunication(dev->iAddr,dev->iPort); //this can only exit with a leave
					///////////////////////////////////////////////////					
					}
					else
					{
						throw "No devices selected";
					}
				}
				else
					throw "Cancelled by user.";
			}

			








				
				

			

			
		

	}
	catch(void* exception)
	{		
		
		that->iStep1Label.SetWindowText((char*)exception);
		//AfxMessageBox((char*)exception);
	}

	that->StopMover();
	closesocket(that->iSocket);      		
	WSACleanup();
	
	delete qs;
	qs=NULL;

	that->iConnectButton.EnableWindow(true);
	that->iConnectButton.SetWindowText("Connect");

	that->m_InstallOnPhoneButton.EnableWindow(true);
	that->m_InstallOnPhoneButton.SetWindowText(KNotInstalledOnPhoneStr);

	that->iStep1Label.SetWindowText("- Make sure it shows \"Awaiting PC Side\".");

	that->m_InstallPic.Stop();
	that->m_InstallPic.ShowWindow(false);
	that->m_InstallPic.UnLoad();

	//CWMouseXPDlg::curInstance->StartNokiaPCSuiteServiceLayer(); start and stop too frequently makes inquiry fail
	
	return;

}





bool CWMouseXPDlg::MSStackRadioFound()
{
	HANDLE hRadio;

	BLUETOOTH_FIND_RADIO_PARAMS btfrp;
	ZeroMemory(&btfrp,sizeof(btfrp));

	btfrp.dwSize = sizeof(btfrp);
	
	HBLUETOOTH_RADIO_FIND ret;
	
	ret = BluetoothFindFirstRadio(     
		&btfrp,
		&hRadio
    );

	if(ret>0)
	{
		
		return true;
	}
	else
	{
		
		return false;
	}
}

bool CWMouseXPDlg::WidcommRadioFound(CWidcommHandler* handler)
{
	if(!handler)
		return false;

	return handler->IsStackServerUp() && handler->IsDeviceReady();

}


void CWMouseXPDlg::StopNokiaPCSuiteServiceLayer()
{
	if(iNokiaPCSuiteServiceLayerStopped)
		return;

	//////////////////////////
					
			SC_HANDLE ScManager,ScService;

			if((ScManager=::OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS))!=NULL)
			{
				if((ScService=::OpenService(ScManager,"ServiceLayer",SERVICE_STOP))!=NULL)
				{
					this->AddLog("Pausing NokiaPCSuite BT (Resumes when Exit)");
					ControlService(ScService,SERVICE_CONTROL_STOP,&status);			
					iNokiaPCSuiteServiceLayerStopped = true;
					
					Sleep(3000);

					int maxwaitremain = 6;
					for(int i=1;i<=maxwaitremain;i++)
					{
						Sleep(1000);
						CString str;
						str.Format("Please wait... (%d/%d)",i,maxwaitremain);
						AddLog(str);
					};

					AddLog("");

					CloseServiceHandle(ScService);
				}							

				CloseServiceHandle(ScManager);
			}
	//////////////////////////
	
}

void CWMouseXPDlg::StartNokiaPCSuiteServiceLayer()
{
	if(!iNokiaPCSuiteServiceLayerStopped)
		return;

	//////////////////////////
			SC_HANDLE ScManager,ScService;
			SERVICE_STATUS status;			
			if((ScManager=::OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS))!=NULL)
			{
				if((ScService=::OpenService(ScManager,"ServiceLayer",SERVICE_START))!=NULL)
				{
					StartService(ScService,0,NULL);					
					iNokiaPCSuiteServiceLayerStopped = false;
				}			
			}
	//////////////////////////
}


void CWMouseXPDlg::OnButton1() //install it on phone
{	  
	//AfxMessageBox(mydebug::iPath);

	CString curStr;
	m_InstallOnPhoneButton.GetWindowText(curStr);

	

	if(curStr == CString(KNotInstalledOnPhoneStr))
	{

		this->m_InstallOnPhoneButton.EnableWindow(false);
		this->m_InstallOnPhoneButton.SetWindowText("Please Wait...");
		StopNokiaPCSuiteServiceLayer();		
		//AddLog("Starting Bluetooth Search..."); this is for sending files - dont change the lower label

	//choose the appropriate BT stack

	//Check all the BT stacks installed, if only 1 then use it, if none, show error and return

	//If more than one BT stack found then:	
	//for each installed stack, check the available BT USB radios and use the first one found.
	//if not found then show error and return
	int stacktouse=EMSStack; 

	iThereWasASession = false;

	////////////
	bool toshradiopresent =  false;
	long toshradiostat;
	if(iToshHandler)
	{
	BTLOCALDEVINFO2 toshradio={0};
	toshradiopresent = BtGetLocalInfo2(&toshradio,&toshradiostat);
	}
	/////////////
	
	if(iWidcommHandler && WidcommRadioFound(iWidcommHandler))
	{
		stacktouse=EWidcommStack;		
	}
	else if(iBlueSoleilHandler)
	{
		stacktouse=EBlueSoleilStack;		
	}
	else if(iToshHandler && toshradiopresent)
	{
		stacktouse=EToshStack;		
	}
	else if(iMSStackSupported && MSStackRadioFound())
	{
		stacktouse=EMSStack; 
	}	
	else
	{	
		AfxMessageBox("No compatible Bluetooth (USB Dongle) found on this PC - Please insert/start it.\r\n\r\nWMouseXP is compatible with Bluetooth drivers (stacks) of BlueSoleil, Toshiba, Microsoft and WIDCOMM/Broadcom.");
		RevertToIdle();
		return;
	}
	//if MS dll exists, &&  inserted then use it
	//else
	//if WIDCOMM dll exists && inserted then use it
	//else
	//show error that 	
	//No compatible Bluetooth (USB Dongle) found on this PC. 
	


	





	if(stacktouse == EMSStack)
	{
		
		//NoteToDeveloper("WMouseXP will now try to send the Mobile Side program to your mobile phone via Bluetooth (MS Stack).\r\n\r\nIf either the sending or the installation on your phone fails,\r\n see the Help menu for other installation methods.");

		this->m_InstallOnPhoneButton.EnableWindow(false);
		this->m_InstallOnPhoneButton.SetWindowText("Please Wait...");

		CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)SendJarThread,this,0,&iThreadID);	
	}
	else
	if(	stacktouse == EBlueSoleilStack)
	{	
		;//CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)WidcommSendJarThread,this,0,&iThreadID);	
		//NoteToDeveloper("WMouseXP will now try to send the Mobile Side program to your mobile phone via Bluetooth (WIDCOMM).\r\n\r\nIf either the sending or the installation on your phone fails,\r\n see the Help menu for other installation methods.");

		
		HGLOBAL hResourceLoaded;		// handle to loaded resource 
		HRSRC hRes;						// handle/ptr. to res. info. 
		BYTE* lpResLock;				// pointer to resource data 
		DWORD dwSizeRes;
		

		hRes = FindResource( NULL, MAKEINTRESOURCE(IDR_JAR3), "JAR" );
		
		if(hRes==NULL)
		{
			AfxMessageBox("rsc not found");
			return;
		}

		// loads the specified resource into global memory. 
		hResourceLoaded = LoadResource( NULL, hRes ); 

		// get a pointer to the loaded resource!
		lpResLock = (BYTE*)LockResource( hResourceLoaded ); 

		// determine the size of the resource, so we know how much to write out to file!  
		dwSizeRes = SizeofResource( NULL, hRes );
			

		
		this->m_InstallOnPhoneButton.EnableWindow(false);
		this->m_InstallOnPhoneButton.SetWindowText("Please Wait...");

		if(!(iBlueSoleilHandler->StartSendFileSession(appWindow,&iConnectButton,&m_InstallPic,IDR_GIF,IDR_GIF1,_T("GIF"),&m_InstallOnPhoneButton,&iStep1Label,lpResLock, dwSizeRes)))
		{
				this->iConnectButton.EnableWindow(true);
				this->iConnectButton.SetWindowText("Connect");

				this->m_InstallOnPhoneButton.EnableWindow(true);
				this->m_InstallOnPhoneButton.SetWindowText(KNotInstalledOnPhoneStr);

				this->m_InstallPic.Stop();
				this->m_InstallPic.ShowWindow(false);
				this->m_InstallPic.UnLoad();
				
				AfxMessageBox("Search for nearby devices failed.\r\n\r\nDevice might not be available at this time.");
		}
	}
	else
	if(stacktouse == EWidcommStack)
	{
		;//CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)WidcommSendJarThread,this,0,&iThreadID);	
		//NoteToDeveloper("WMouseXP will now try to send the Mobile Side program to your mobile phone via Bluetooth (WIDCOMM).\r\n\r\nIf either the sending or the installation on your phone fails,\r\n see the Help menu for other installation methods.");

		HGLOBAL hResourceLoaded;		// handle to loaded resource 
		HRSRC hRes;						// handle/ptr. to res. info. 
		BYTE* lpResLock;				// pointer to resource data 
		DWORD dwSizeRes;
		

		hRes = FindResource( NULL, MAKEINTRESOURCE(IDR_JAR3), "JAR" );
		
		if(hRes==NULL)
		{
			AfxMessageBox("Internal resource not found, try Re-download and Install from www.ClearEvo.com");
			return;
		}

		// loads the specified resource into global memory. 
		hResourceLoaded = LoadResource( NULL, hRes ); 

		// get a pointer to the loaded resource!
		lpResLock = (BYTE*)LockResource( hResourceLoaded ); 

		// determine the size of the resource, so we know how much to write out to file!  
		dwSizeRes = SizeofResource( NULL, hRes );
			

		
		this->m_InstallOnPhoneButton.EnableWindow(false);
		this->m_InstallOnPhoneButton.SetWindowText("Please Wait...");

		if(!(iWidcommHandler->StartSendFileSession(appWindow,&iConnectButton,&m_InstallPic,IDR_GIF,IDR_GIF1,_T("GIF"),&m_InstallOnPhoneButton,&iStep1Label,lpResLock, dwSizeRes)))
		{
				this->iConnectButton.EnableWindow(true);
				this->iConnectButton.SetWindowText("Connect");

				this->m_InstallOnPhoneButton.EnableWindow(true);
				this->m_InstallOnPhoneButton.SetWindowText(KNotInstalledOnPhoneStr);

				this->m_InstallPic.Stop();
				this->m_InstallPic.ShowWindow(false);
				this->m_InstallPic.UnLoad();
				
				AfxMessageBox("Search for nearby devices failed.\r\n\r\nDevice might not be available at this time.");
		}

	}
	else if(stacktouse == EToshStack)
	{
			
		HGLOBAL hResourceLoaded;		// handle to loaded resource 
		HRSRC hRes;						// handle/ptr. to res. info. 
		BYTE* lpResLock;				// pointer to resource data 
		DWORD dwSizeRes;
		

		hRes = FindResource( NULL, MAKEINTRESOURCE(IDR_JAR3), "JAR" );
		
		if(hRes==NULL)
		{
			AfxMessageBox("Internal resource not found, try Re-download and Install from www.ClearEvo.com");
			return;
		}

		// loads the specified resource into global memory. 
		hResourceLoaded = LoadResource( NULL, hRes ); 

		// get a pointer to the loaded resource!
		lpResLock = (BYTE*)LockResource( hResourceLoaded ); 

		// determine the size of the resource, so we know how much to write out to file!  
		dwSizeRes = SizeofResource( NULL, hRes );
			

		
		this->m_InstallOnPhoneButton.EnableWindow(false);
		this->m_InstallOnPhoneButton.SetWindowText("Please Wait...");

		if(!(iToshHandler->StartSendFileSession(appWindow,&iConnectButton,&m_InstallPic,IDR_GIF,IDR_GIF1,_T("GIF"),&m_InstallOnPhoneButton,&iStep1Label,lpResLock, dwSizeRes)))
		{
				this->iConnectButton.EnableWindow(true);
				this->iConnectButton.SetWindowText("Connect");

				this->m_InstallOnPhoneButton.EnableWindow(true);
				this->m_InstallOnPhoneButton.SetWindowText(KNotInstalledOnPhoneStr);

				this->m_InstallPic.Stop();
				this->m_InstallPic.ShowWindow(false);
				this->m_InstallPic.UnLoad();
				
				AfxMessageBox("Search for nearby devices failed.\r\n\r\nDevice might not be available at this time.");
		}
	}

	}

}



BOOL CWMouseXPDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class	
	
	
	if (NULL != m_pToolTip && iConnectButton.IsWindowEnabled() )
            m_pToolTip->RelayEvent(pMsg);

	
    
	return CMyDialog::PreTranslateMessage(pMsg);
}

void CWMouseXPDlg::OnCheck1() 
{
	// TODO: Add your control notification handler code here

	int code = m_PrevDevCheckBox.GetCheck();
	char prevcheck[5];
	
	if(code == BST_CHECKED)
		strcpy(prevcheck,"y");
	else
		strcpy(prevcheck,"n");

	SetRegistryKey("Software\\WMouseXP1_0", "PrevCheck", prevcheck);
	
}



/*
void CWMouseXPDlg::OnDeviceResponded (BD_ADDR bda, DEV_CLASS devClass, BD_NAME bdName, BOOL bConnected)
{
    
    // Add the device address and name (if any)
    // to the Server List.  It is OK to pass
    // duplicates.  This method will ignore them.
    //
    //p_Dlg->DoAddDevice(bda, bdName);
	MessageBox("dev");
}

void CWMouseXPDlg::OnDiscoveryComplete ()
{
    //
    // Discovery is now complete, so set event to
    // unblock the Discovery worker thread.
    //
    //SetEvent(p_Dlg->m_hEventStopDiscoveryThread);
}

void CWMouseXPDlg::OnInquiryComplete (BOOL success, short num_responses)
{
	MessageBox("complete");
    /*CString msg;
	if (m_start_discovery || m_discovery_in_progress)
	{
    msg.Format("Redundant Inquiry Complete for %d devices", num_responses);
    p_Dlg->SetDlgItemText(IDC_STATUS_TEXT, msg);
	Sleep (3000);
	}
	else
	{
		msg.Format("Starting service discovery cycle for %d server device%c", m_num_devices, m_num_devices == 1 ? ' ' : 's');
    p_Dlg->SetDlgItemText(IDC_STATUS_TEXT, msg);
	Sleep (3000);
    m_start_discovery = TRUE;   // timer will start discovery
	}*/
//}



void CWMouseXPDlg::OnOptions() 
{
	// TODO: Add your command handler code here
	COptionsDialog dlgAbout(iSpeedOption,iAccelerationOption);
	dlgAbout.DoModal();	
	//save the iSpeedPixels, iAccelerationPixels

	CString optionspath = GetProgramDir();
	optionspath += "\\options.dat";
	
	CFile file;	
	if(file.Open(optionspath,CFile::modeWrite|CFile::modeCreate))
	{
		unsigned char options[2];		
		options[0] = iSpeedOption;					
		options[1] = iAccelerationOption;
		file.Write(options,2);
		file.Flush();
		file.Close();
	}

	

}

void CWMouseXPDlg::LoadCustomCommands()
{
	CString optionspath = GetProgramDir();
	optionspath += "\\custcmd.dat";
	
	CFile file;	
	if(file.Open(optionspath,CFile::modeRead))
	{	
		file.Read(iNumpadCmdMap,sizeof(iNumpadCmdMap));		
		file.Close();
	}

	optionspath = GetProgramDir();
	optionspath += "\\custmod.dat";
	
	if(file.Open(optionspath,CFile::modeRead))
	{	
		file.Read(iNumpadModifiers,sizeof(iNumpadModifiers));		
		file.Close();
	}
}

void CWMouseXPDlg::SaveCustomCommands()
{
	CString optionspath = GetProgramDir();
	optionspath += "\\custcmd.dat";
	
	CFile file;	
	if(file.Open(optionspath,CFile::modeWrite|CFile::modeCreate))
	{	
		file.Write(iNumpadCmdMap,sizeof(iNumpadCmdMap));
		file.Flush();
		file.Close();
	}

	optionspath = GetProgramDir();
	optionspath += "\\custmod.dat";	
	
	if(file.Open(optionspath,CFile::modeWrite|CFile::modeCreate))
	{	
		file.Write(iNumpadModifiers,sizeof(iNumpadModifiers));
		file.Flush();
		file.Close();
	}
}





LRESULT CWMouseXPDlg::WindowProc( UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class

	
	


	if(message == WM_POWERBROADCAST  )
	{		
		
		CString stopreason;

		if(wParam ==  PBT_APMSUSPEND || wParam == PBT_APMSTANDBY)
		{

			CString curStr;
			iConnectButton.GetWindowText(curStr); //see if already connected or not

			bool isconnected = (curStr != CString("Connect"));

			if(isconnected)
			{
			AddLog(stopReasonSuspend); //so we can auto-resume
			}			

			//////////////// stop and cleanup
			if(iStackUsed == EMSStack)
			{
				closesocket(iSocket);      
				WSACleanup();
				StopTrialTimer();				
			}
			else
			if(iStackUsed == EWidcommStack)
			{
				iWidcommHandler->CloseAndRevertEngineState();				
				RevertToIdle();
				StopTrialTimer();

				if(MSStackSupported()) //vista widcomm is based on MS
				{
				closesocket(iSocket);      
				WSACleanup();			
				}

			}
			else
			if(	iStackUsed == EBlueSoleilStack)
			{	
				iBlueSoleilHandler->CloseAndRevertEngineState();				
				RevertToIdle();
				StopTrialTimer();			
			}
			else
			if(iStackUsed == EToshStack)
			{
				iToshHandler->CloseAndRevertEngineState();
				RevertToIdle();
				StopTrialTimer();			
			}

			///////////////


			
			if(isconnected) //re-set again for MS case where the Log might be overwritten
			{
			AddLog(stopReasonSuspend); //so we can auto-resume
			}

			return TRUE;
		}
		else
		if(wParam ==  PBT_APMBATTERYLOW)
		{

			CString curStr;
			iConnectButton.GetWindowText(curStr); //see if already connected or not

			bool isconnected = (curStr != CString("Connect"));

			if(isconnected)
			{
			AddLog(stopLowBat); //so we can auto-resume
			}

			//////////////// stop and cleanup
			if(iStackUsed == EMSStack)
			{
				closesocket(iSocket);      
				WSACleanup();
				StopTrialTimer();				
			}
			else
			if(iStackUsed == EWidcommStack)
			{
				iWidcommHandler->CloseAndRevertEngineState();				
				RevertToIdle();
				StopTrialTimer();

				if(MSStackSupported()) //vista widcomm is based on MS
				{
				closesocket(iSocket);      
				WSACleanup();			
				}

			}
			else
			if(	iStackUsed == EBlueSoleilStack)
			{	
				iBlueSoleilHandler->CloseAndRevertEngineState();				
				RevertToIdle();
				StopTrialTimer();			
			}
			else
			if(iStackUsed == EToshStack)
			{
				iToshHandler->CloseAndRevertEngineState();
				RevertToIdle();
				StopTrialTimer();			
			}
			///////////////

			
			
			if(isconnected)
			{
			AddLog(stopLowBat); //so we can auto-resume
			}
			
		}		
		else if(wParam == PBT_APMRESUMECRITICAL || wParam == PBT_APMRESUMESUSPEND|| wParam == PBT_APMRESUMESTANDBY || wParam == PBTF_APMRESUMEFROMFAILURE )
		{

			//try 
			CString labeltext;
			this->iStatusLabel.GetWindowText(labeltext);

			if(labeltext == stopReasonSuspend || labeltext == stopLowBat)
			{				
				AddLog(resComplete);
			}

			return TRUE;
		
		}
		
	}
	else		
	if(message == WM_WTSSESSION_CHANGE )
	{	

		if(wParam == WTS_SESSION_UNLOCK)
		{		

			//try 
			CString labeltext;
			this->iStatusLabel.GetWindowText(labeltext);	

			if(labeltext == resComplete || labeltext == stopLocked) //if unlocked after resume or system standby
			{					
				AddLog(resumingin15sec);	

				HWND hwnd = this->GetSafeHwnd();

				if (IsIconic()) 
				{
					ShowWindow(SW_SHOW);
					ShowWindow(SW_RESTORE);
				}         
				SetForegroundWindow();

				StartResumeBTConnectionTimer();				
			}			
		
		}
		else
		if(wParam == WTS_SESSION_LOCK)
		{
		
			CString curStr;
			iConnectButton.GetWindowText(curStr); //see if already connected or not			

			bool isconnected = (curStr != CString("Connect"));

			if(isconnected)
			{
			AddLog(stopLocked); //so we can auto-resume
			}

			//////////////// stop and cleanup
			if(iStackUsed == EMSStack)
			{
				closesocket(iSocket);      
				WSACleanup();
				StopTrialTimer();				
			}
			else
			if(iStackUsed == EWidcommStack)
			{
				iWidcommHandler->CloseAndRevertEngineState();				
				RevertToIdle();
				StopTrialTimer();

				if(MSStackSupported()) //vista widcomm is based on MS
				{
				closesocket(iSocket);      
				WSACleanup();			
				}

			}
			else
			if(	iStackUsed == EBlueSoleilStack)
			{	
				iBlueSoleilHandler->CloseAndRevertEngineState();				
				RevertToIdle();
				StopTrialTimer();			
			}
			else
			if(iStackUsed == EToshStack)
			{
				iToshHandler->CloseAndRevertEngineState();
				RevertToIdle();
				StopTrialTimer();			
			}
			///////////////


			if(isconnected)
			{
			AddLog(stopLocked); //so we can auto-resume
			}
			
			
			return TRUE;
		
		
		}
		





		
	
	}
	

	return CDialog::WindowProc(message, wParam, lParam);
}



bool CWMouseXPDlg::StartResumeBTConnectionTimer()
{
	mResumeBTConnectionTimer = SetTimer(38/*any identifier because we have only one timer*/, 15 * 1000, ResumeBTConnectionProc);

	if(mResumeBTConnectionTimer)
		return true;
	
	return false;
}



bool CWMouseXPDlg::StopResumeBTConnectionTimer()
{
	if(mResumeBTConnectionTimer && KillTimer(mResumeBTConnectionTimer))
	{
		mResumeBTConnectionTimer = 0;
		return true;
	}

	return false;


}

void CALLBACK EXPORT CWMouseXPDlg::ResumeBTConnectionProc(
   HWND hWnd,      // handle of CWnd that called SetTimer
   UINT nMsg,      // WM_TIMER
   UINT nIDEvent,   // timer identification
   DWORD dwTime    // system time
   )
									
{
	if(!(CWMouseXPDlg::curInstance))
		return;

	CWMouseXPDlg::curInstance->mResumeBTConnectionTimer = NULL;

			CString labeltext;
			CWMouseXPDlg::curInstance->iStatusLabel.GetWindowText(labeltext);


			if(labeltext == resumingin15sec) //if waiting to reconnect
			{									
				CWMouseXPDlg::curInstance->OnButton();
			}

			
			CWMouseXPDlg::curInstance->KillTimer(nIDEvent);

}





void CWMouseXPDlg::OnCheck2() 
{
	// TODO: Add your control notification handler code here
	
}

void CWMouseXPDlg::OnOptionsCustomizecommands() 
{
	// TODO: Add your command handler code here
	CCustomCommandDialog dlg;
	dlg.DoModal();
}


long CWMouseXPDlg::ToshBtDevSearchRes(UINT wParam, LONG lParam)
{
	if(this->iToshHandler)
	{
	/*	CString str;
		str.Format("%x  %x",wParam,lParam);
		AfxMessageBox(str);*/

		
			
		iToshHandler->HandleMsgDevSearchRes(wParam,lParam);
	}

	return 1;
}



void CWMouseXPDlg::OnCheck3() 
{	
	// TODO: Add your control notification handler code here
	int code = m_tc.GetCheck();
	char prevcheck[5];
	
	if(code == BST_CHECKED)
	{
		strcpy(prevcheck,"y");
		StartAutoConnectTimer();
	}
	else
	{
		strcpy(prevcheck,"n");
		StopAutoConnectTimer();
	}

	SetRegistryKey("Software\\WMouseXP1_0", "ConnectOnStart", prevcheck);


	////////////auto-start

	HKEY hKey;
	CString sKeyName;
	unsigned char szFilePathOri[MAX_PATH];
	unsigned char szFilePath[MAX_PATH];

	GetModuleFileName(AfxGetInstanceHandle(), (char *)szFilePathOri, MAX_PATH);
	strcpy((char *)szFilePath,"\"");
	strcat((char *)szFilePath,(char *)szFilePathOri);
	strcat((char *)szFilePath,"\"");
	strcat((char *)szFilePath," /autostart");



//	AfxMessageBox((char *)szFilePath);


	LONG lnRes = RegOpenKeyEx(
           HKEY_LOCAL_MACHINE,  // handle of open key
             // The following is the address of name of subkey to open
           "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
           0L,KEY_WRITE,
           &hKey            // address of handle of open key
       );

	// now add program path to the RUN key
	lstrcpy( (char *) szFilePath , LPCTSTR(szFilePath) );
	

	if( ERROR_SUCCESS == lnRes )
	{
		if(code == BST_CHECKED)
		{
		
		   lnRes = RegSetValueEx(hKey,
								 LPCTSTR( "WMouseXP" ),  // handle of the opened
													   // key to set value for
								 0,
								 REG_SZ,
								 szFilePath,   //value data
								 MAX_PATH );

		   if(ERROR_SUCCESS == lnRes)
			   ;
		   else
		   {
			   AfxMessageBox("Auto-start on boot set failed");
			   mydebug::log("Auto-start on boot set failed");
		   }
		   
		}
		else
		{
			 lnRes = RegDeleteValueA(hKey, LPCTSTR( "WMouseXP" ));

		   if(ERROR_SUCCESS == lnRes)
			   ;
		   else
		   {
			   AfxMessageBox("Auto-start on boot remove failed");
			   mydebug::log("Auto-start on boot remove failed");
		   }
		   
		
		}

	}
	else
	{
		AfxMessageBox("Opening of auto-start on boot settings failed");
		mydebug::log("Opening of auto-start on boot settings failed");
	}

	
	//////////////
}

