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


// ToshHandler.h: interface for the CToshHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOSHHANDLER_H__BAFE4467_DDDE_4035_84BA_E242C3AB2CF3__INCLUDED_)
#define AFX_TOSHHANDLER_H__BAFE4467_DDDE_4035_84BA_E242C3AB2CF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TidyVector.h"
#include "pictureex.h"
#include "btenginescommons.h"


#include "TosBtAPI.h"// (function prototype definitions, etc.) 
#include "TosBtParam.h"// (InputI/output parameter definitions) 
#include "TosBtNM.h"// (Definitions of notification messages for asynchronous processing) 
#include "TosBtErr.h"// (Error code definitions)
#include "SDPTypes.h"// (Structure type definitions)



enum TToshWndMessages
{
	UM_TOSHMESSAGES = (WM_APP + 0x19),	
};

class TToshParams
{
public:
	HWND iHWND;	
	char iAppName[32];	
	long iStatus;
};

typedef	struct {
	LONG			lSts;
	PBTDEVINFOLIST	pDevInfoList;
} DEVINFO, *PDEVINFO;



class CToshHandler  
{
public:

	enum ERequestedCommand
{
	EStateIdle,
	ESendFileSession,
	ERemoteControlSession
};


	static bool ToshSupported();


	virtual ~CToshHandler();

	void RevertToIdle();

	bool StartSendFileSession(HWND hwnd, CButton* connectbutton, CPictureEx* icon, int searchinganimrsc, int sendinganimrsc, LPTSTR rsctype, CButton* button,CStatic* label, BYTE* fdata, int fdatalen);
	
	bool StartRemoteControlSession(HWND hwnd, CButton* connectbutton, CPictureEx* icon, int searchinganimrsc, int sendinganimrsc, LPTSTR rsctype, CButton* button,CStatic* label, MRemoteControlCaller* caller);
	
	void CloseAndRevertEngineState();

	static CToshHandler* curInstance;

	static CToshHandler* GetInstance(TToshParams& aParams);

	////////to be called from main HWND class
	void HandleMsgDevSearchRes(UINT wParam, LONG lParam);
	/////////////


	int iState; //state

	///////////////////////called from SDKFUNC.CPP
/*	void DoAddDevice(BLUETOOTH_DEVICE_INFO& dev);
	void OnInquiryComplete();

	//////////////////FOR SerialPorrtProfile connection to mobile...
	void OnData(BYTE* data, int len);
	void OnSPPConnectionStatus(DWORD dwServerHandle, BYTE* lpBdAddr, UCHAR ucStatus,DWORD dwConnetionHandle);
	void OnError(DWORD dwErrorCode);
	//////////////////////////////////////////////
	DWORD iConnectionHandle;
	///////////////////////////////////////////////

	static int StartInquiry(LPVOID that);
	void DoDiscoverWMouseXPThroughDeviceList();*/

protected:

	CToshHandler(TToshParams& aParams);	

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
	
	int iDeviceScn;
	GUID iDiscoveringGUID;	
	MRemoteControlCaller* iCaller;
	DWORD iInquiryThreadID;

	bool StartInquiry();
	void OnInquiryComplete();

	/////////////////sdp	
	int iSelectedDev;	
	PBTSDPSSRESULT iSDPSSRESULT;//cleanup

	USHORT iCID;//connection id - if not 0 then close to cleanup
	/////////////////
	
	//////////rfcomm
	USHORT iRFCOMMCID;
	/////////


private:
	
	TToshParams iParams;
	
	CHAR iTmpObexFilePath[MAX_PATH];

    int iRequestedCommand;	
	
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

	DEVINFO iDevInfo;
	BTDEVINFOLIST iPrevDevInfoList;//1 device: previous device - to be pointed by iDevInfo in prevdev checkbox case
	long iStatus;

	bool iOpenedObexApi;

	BYTE* iWmxpToshSDPPattern; //needs cleanup - pointer to allocated bt struct
	DWORD iWmxpToshSDPPatternSize;

	BYTE* iWmxpToshAttrIdList;
	DWORD iWmxpToshAttrIdListSize;

	BTSDPSSARESULT* iMemSDP_SSA_Result;

	int iWmxpscn;

	//HANDLE iHFile;


};

#endif // !defined(AFX_TOSHHANDLER_H__BAFE4467_DDDE_4035_84BA_E242C3AB2CF3__INCLUDED_)
