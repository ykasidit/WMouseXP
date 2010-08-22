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


// WidcommHandler.h: interface for the CBlueSoleilHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__WMOUSEXP_BLUESOLEIL_HANDLER_H__)
#define __WMOUSEXP_BLUESOLEIL_HANDLER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "TidyVector.h"
#include "pictureex.h"
#include "btenginescommons.h"




class CBlueSoleilHandler
{
public:
	
enum ERequestedCommand
{
	EStateIdle,
	ESendFileSession,
	ERemoteControlSession
};

enum EBlueSoleilHandlerState
{
	EHandlerStateIdle,
	EHandlerStateDoingInquiry,
	EHandlerStateDoingServideDiscovery,
	EHandlerStateSendingFile
};


	static CBlueSoleilHandler* GetInstance();
	~CBlueSoleilHandler();
	static bool BlueSoleilSupported();

	bool StartSendFileSession(HWND hwnd, CButton* connectbutton, CPictureEx* icon, int searchinganimrsc, int sendinganimrsc, LPTSTR rsctype, CButton* button,CStatic* label);
	
	bool StartRemoteControlSession(HWND hwnd, CButton* connectbutton, CPictureEx* icon, int searchinganimrsc, int sendinganimrsc, LPTSTR rsctype, CButton* button,CStatic* label, MRemoteControlCaller* caller);

	bool RFCOMMWrite(BYTE* ptr, long len);
	
	void CloseAndRevertEngineState();

	static CBlueSoleilHandler* curInstance;

	int iState; //state

	///////////////////////called from SDKFUNC.CPP
	void DoAddDevice(BLUETOOTH_DEVICE_INFO& dev);
	void OnInquiryComplete();

	//////////////////FOR SerialPorrtProfile connection to mobile...
	void OnData(BYTE* data, int len);
	void OnSPPConnectionStatus(DWORD dwServerHandle, BYTE* lpBdAddr, UCHAR ucStatus,DWORD dwConnetionHandle);
	void OnError(DWORD dwErrorCode);
	//////////////////////////////////////////////
	DWORD iConnectionHandle;
	///////////////////////////////////////////////

	static int StartInquiry(LPVOID that);
	void DoDiscoverWMouseXPThroughDeviceList();

protected:
	
	CBlueSoleilHandler();	

	HWND iHwnd;
	CButton* iButtonToDisable;
	CPictureEx* iIcon;
	int iSearchAnim;
	int iSendAnim;
	CString iAnimRscType;

	CButton* iButton;
	CStatic* iLabel;
	BYTE* iFileData;
	int iFileLen;
	
	TidyVector iFoundDevices;
	TidyVector iWMouseXPDevicesIndex;

	bool iDiscoverCommandSent;
	BYTE iDiscoveringDevice[6];
	BYTE iConnectedDevice[6];

	//BD_ADDR iDiscoveringDevice;
	int iDeviceScn;

	GUID iDiscoveringGUID;
	
	MRemoteControlCaller* iCaller;

	DWORD iInquiryThreadID;

	
#define MAX_SERVICES_PER_DEVICE    1
    

	void RevertToIdle();

	

/*
//WIDCOMM COPPCLIENT
public:

//	void OnProgress(OPP_RESULT_CODE result_code, BD_ADDR bda, WCHAR * string, long current, long total);
	void OnPushResponse(OPP_RESULT_CODE result_code,  BD_ADDR bda, WCHAR * string);

//	void OnPullResponse(OPP_RESULT_CODE result_code , BD_ADDR bda, WCHAR * string);
//	void OnExchangeResponse(OPP_RESULT_CODE result_code, BD_ADDR bda, WCHAR * string);
//	void OnExchangeResponse(OPP_RESULT_CODE result_code, BD_ADDR bda, WCHAR * string, OPP_TRANSACTION_CODE transaction_code);

	
	//WIDDCOMM CBTIF 
public:
	void OnDeviceResponded (BD_ADDR bda, DEV_CLASS devClass, BD_NAME bdName, BOOL bConnected);
    void OnDiscoveryComplete ();
    void OnInquiryComplete (BOOL success, short num_responses);

	
	
	//WIDCOMM CRFCOMMPORT
	void OnEventReceived (UINT32 event_code);
	void OnDataReceived (void *p_data, UINT16 len);
	
////////////////////////////////
*/

	/*From BlueSoleil SDK:
	
	typedef struct _BLUETOOTH_DEVICE_INFO {
	DWORD dwSize;
	BYTE address[DEVICE_ADDRESS_LENGTH];
	BYTE classOfDevice[DEVICE_CLASS_LENGTH];
	CHAR szName[MAX_DEVICE_NAME_LENGTH];
	BOOL bPaired;
} BLUETOOTH_DEVICE_INFO, *PBLUETOOTH_DEVICE_INFO;
	*/
	

	CHAR iTmpObexFilePath[MAX_PATH];//for WIDCOMM

    int iRequestedCommand;
	
	//bool DoDiscoverWMouseXPThroughDeviceList();//return true if not finished through the list yet (there are more to discover)
	int iCurDevice; //current device index in inquired list which we are discovering for wmousexp service

	//CRfCommIf m_RfCommIf;

	bool iUsingPrevDev;

	
	
	
	bool StartBTInteractionTimer();
	bool StopBTInteractionTimer();
	int	m_TimerID;
	
	static void CALLBACK EXPORT TimerProc(
   HWND hWnd,      // handle of CWnd that called SetTimer
   UINT nMsg,      // WM_TIMER
   UINT nIDEvent,   // timer identification
   DWORD dwTime    // system time
									);

	/*int m_num_services_per_device;
#define MAX_SERVICES_PER_DEVICE    20
    CSdpDiscoveryRec    m_si[MAX_SERVICES_PER_DEVICE];

    int m_num_devices;
#define MAX_SERVER_DEVICES 20
    CBdInfo  *           m_pDevInfo[MAX_SERVER_DEVICES];  */

};

#endif // !defined(__WMOUSEXP_BLUESOLEIL_HANDLER_H__)
