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


// WidcommHandler.h: interface for the CWidcommHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WIDCOMMHANDLER_H__D8F8C51A_CE1C_4B0C_B855_F25B6F723D9A__INCLUDED_)
#define AFX_WIDCOMMHANDLER_H__D8F8C51A_CE1C_4B0C_B855_F25B6F723D9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BtIfDefinitions.h"
#include "btwlib.h"
#include "TidyVector.h"
#include "pictureex.h"


#include "btenginescommons.h"





class CWidcommHandler : public CBtIf, public COppClient, public CRfCommPort, public MBTEngineHandler
{
public:

	enum ERequestedCommand
{
	EStateIdle,
	ESendFileSession,
	ERemoteControlSession
};

enum EWidcommHandlerState
{
	EHandlerStateIdle,
	EHandlerStateDoingInquiry,
	EHandlerStateDoingServideDiscovery,
	EHandlerStateSendingFile
};


	
	static CWidcommHandler* GetInstance();
	~CWidcommHandler();
	static bool WidcommSupported();

	bool StartSendFileSession(HWND hwnd, CButton* connectbutton, CPictureEx* icon, int searchinganimrsc, int sendinganimrsc, LPTSTR rsctype, CButton* button,CStatic* label, BYTE* fdata, int fdatalen);
	bool StartRemoteControlSession(HWND hwnd, CButton* connectbutton, CPictureEx* icon, int searchinganimrsc, int sendinganimrsc, LPTSTR rsctype, CButton* button,CStatic* label, MRemoteControlCaller* caller);
	
	void CloseAndRevertEngineState();

	bool RFCOMMWrite(BYTE* data, USHORT len, USHORT& lenwritten);

	static CWidcommHandler* curInstance;

	int iState; //state

protected:
	
	CWidcommHandler();	

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
	BD_ADDR iDiscoveringDevice;
	int iDeviceScn;

	GUID iDiscoveringGUID;
	
	MRemoteControlCaller* iCaller;

	
#define MAX_SERVICES_PER_DEVICE    1
    

	void RevertToIdle();

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
	
	void DoAddDevice(BD_ADDR p_bda, BD_NAME bd_name);

	WCHAR iTmpObexFilePath[MAX_PATH];//for WIDCOMM

    int iRequestedCommand;
	
	bool DoDiscoverWMouseXPThroughDeviceList();//return true if not finished through the list yet (there are more to discover)
	int iCurDevice; //current device index in inquired list which we are discovering for wmousexp service

	CRfCommIf m_RfCommIf;

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

#endif // !defined(AFX_WIDCOMMHANDLER_H__D8F8C51A_CE1C_4B0C_B855_F25B6F723D9A__INCLUDED_)
